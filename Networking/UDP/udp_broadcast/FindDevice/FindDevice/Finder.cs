using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.Threading;

namespace FindDevice
{
    internal class Finder
    {
        const int Port = 8;
        const int DetectionPacketLength = 10;
        const byte Packet0 = 0xA0;
        const byte Packet1 = 0x50;
        const int PacketsToSend = 10;
        const int SleepBetweenPackets = 100;

        byte[] DectectionRequest = new byte[DetectionPacketLength]
        {
            Packet0, Packet1, 0, 0, 0, 0, 0, 0, 0, 0
        };

        List<Task> tasks = new List<Task>();
        List<DeviceInfo> devices = new List<DeviceInfo>();
        object syncObject = new object();                   // devices access synchronization


        public void Run()
        {
            SyncConsole.Print($"Finder started, Port {Port}");

            foreach (var i in System.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces())
            {
                foreach (var ua in i.GetIPProperties().UnicastAddresses)
                {
                    Task task = Task.Run(() => MakeSearch(ua, i.Name, i.Description));
                    tasks.Add(task);
                }
            }

            Task.WaitAll(tasks.ToArray());
            SyncConsole.Print(string.Empty);

            if (devices.Count == 0)
            {
                SyncConsole.Print("Device not fount");
            }

            foreach (var device in devices)
            {
                SyncConsole.Print($"Device: {device.DeviceIpAddress} {device.DeviceSubnetMask}  Host: {device.HostIpAddress}  [{device.AdapterName}]");
            }
        }

        void MakeSearch(UnicastIPAddressInformation addressInfo, string adapterName, string adapterDescription)
        {
            SyncConsole.Print($"{adapterName} {addressInfo.Address}");

            if (!(addressInfo.Address.AddressFamily == AddressFamily.InterNetwork))
            {
                SyncConsole.Print("Not IPv4 address, ignored");
                return;
            }

            SendBroadcast(addressInfo.Address, addressInfo.IPv4Mask, adapterName, adapterDescription);
        }

        void SendBroadcast(IPAddress address, IPAddress mask, string adapterName, string adapterDescription)
        {
            UdpClient udp;

            try
            {
                udp = new UdpClient(new IPEndPoint(address, 0));  // 0 - any available port
            }
            catch (SocketException ex)
            {
                SyncConsole.Print($"Host IP: {address} Error creating UDP client: {ex.Message}");
                return;
            }

            udp.EnableBroadcast = true;

            StartListening(udp, address, mask, adapterName, adapterDescription);

            IPEndPoint ip = new IPEndPoint(IPAddress.Broadcast, Port);

            for (int i = 0; i < PacketsToSend; ++i)
            {
                try
                {
                    udp.Send(DectectionRequest, DectectionRequest.Length, ip);
                }
                catch (SocketException ex)
                {
                    SyncConsole.Print($"Host IP: {address} Error sending UDP packet: {ex.Message}");
                    break;
                }

                Thread.Sleep(SleepBetweenPackets);
            }

            udp.Close();
        }

        void StartListening(UdpClient udp, IPAddress address, IPAddress mask, string adapterName, string adapterDescription)
        {
            udp?.BeginReceive(Receive, new StateInfo
            {
                UdpClient = udp,
                Address = address,
                SubnetMask = mask,
                AdapterName = adapterName,
                AdapterDescription = adapterDescription
            });
        }

        void Receive(IAsyncResult ar)
        {
            StateInfo state = null;

            if (ar != null && ar.AsyncState != null)
            {
                state = ar.AsyncState as StateInfo;
            }

            if (state == null)
            {
                SyncConsole.Print("Receive: cannot get AsyncState");
                return;
            }

            UdpClient client = state.UdpClient;

            if (client == null)
            {
                SyncConsole.Print("Receive: state.UdpClient = null");
                return;
            }

            IPEndPoint remoteIP = null;
            byte[] bytes = null;


            try
            {
                bytes = client.EndReceive(ar, ref remoteIP);
            }
            catch (ArgumentNullException)
            {
                SyncConsole.Print("Receive: ArgumentNullException");
                return;
            }
            catch (ArgumentException)
            {
                SyncConsole.Print("Receive: ArgumentException");
                return;
            }
            catch (InvalidOperationException)
            {
                SyncConsole.Print("Receive: InvalidOperationException");
                return;
            }
            catch (SocketException)
            {
                SyncConsole.Print("Receive: SocketException");
                return;
            }
            catch (NullReferenceException)
            {
                SyncConsole.Print("Receive: NullReferenceException");
                return;
            }

            if (remoteIP != null && bytes != null && bytes.Length != 0)
            {
                HandleReply(state.Address, state.SubnetMask, state.AdapterName, state.AdapterDescription, remoteIP, bytes);
            }

            // Continue listening
            try
            {
                StartListening(state.UdpClient, state.Address, state.SubnetMask, state.AdapterName, state.AdapterDescription);
            }
            catch (InvalidOperationException)
            {
                SyncConsole.Print("Receive, StartListening: InvalidOperationException");
            }
        }

        void HandleReply(IPAddress hostAddress, IPAddress hostSubnetMask, string adapterName, string adapterDescription, IPEndPoint remoteEndpoint, byte[] reply)
        {
            if (!(remoteEndpoint.AddressFamily == AddressFamily.InterNetwork))
            {
                SyncConsole.Print("HandleReply: not IPv4 address");
                return;
            }

            if (reply.Length != DetectionPacketLength)
            {
                SyncConsole.Print($"Unexpected reply length {reply.Length}");
                return;
            }

            SyncConsole.Print($"Received from {remoteEndpoint.Address}: {Utilities.ToHexArray(reply)}");

            if (reply[0] == Packet0 && reply[1] == Packet1)
            {
                // OK
            }
            else
            {
                SyncConsole.Print("HandleReply: unexbected bytes 0,1");
                return;
            }

            byte[] ip4 = new byte[4];
            byte[] mask4 = new byte[4];

            Buffer.BlockCopy(reply, 2, ip4, 0, 4);
            Buffer.BlockCopy(reply, 6, mask4, 0, 4);

            DeviceInfo info = new DeviceInfo
            {
                HostIpAddress = hostAddress,
                HostSubnetMask = hostSubnetMask,
                DeviceIpAddress = new IPAddress(ip4),
                DeviceSubnetMask = new IPAddress(mask4),
                AdapterName = adapterName
            };

            // Add to the list
            lock (syncObject)
            {
                foreach (DeviceInfo i in devices)
                {
                    if (i.HostIpAddress.Equals(info.HostIpAddress) &&
                        i.HostSubnetMask.Equals(info.HostSubnetMask) &&
                        i.DeviceIpAddress.Equals(info.DeviceIpAddress) &&
                        i.DeviceSubnetMask.Equals(info.DeviceSubnetMask) &&
                        i.AdapterName == info.AdapterName)
                    {
                        return;         // duplicated
                    }
                }

                devices.Add(info);
            }
        }
    }
}

