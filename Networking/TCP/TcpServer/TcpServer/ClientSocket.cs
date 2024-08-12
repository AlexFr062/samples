using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace TcpServer
{
    /// <summary>
    /// Client socket
    /// (server socket for single client handling)
    /// </summary>
    class ClientSocket
    {
        private Socket socket;
        const int bufferSize = 1024;
        private byte[] buffer = new byte[bufferSize];
        private int id;
        private Server server;

        public event EventHandler<DisconnectedEventArgs> Disconnected;
        public event EventHandler<DataReceivedEventArgs> DataReceived;

        public int Id
        {
            get { return id; }
        }

        public ClientSocket(Server server, Socket socket, int id)
        {
            this.server = server;
            this.socket = socket;
            this.id = id;
        }

        public void Start()
        {
            BeginReceive();
        }

        void BeginReceive()
        {
            try
            {
                socket.BeginReceive(
                            buffer,                         // location for received data
                            0,                              // buffer offset
                            bufferSize,                     // number of bytes to receive
                            0,                              // socket flags
                            new AsyncCallback(ReadCallback),// callback 
                            null);
            }
            catch (ObjectDisposedException)
            {
                server.LogOptional($"ClientSocket {id}. BeginReceive: ObjectDisposedException");
                CloseSocket();
                return;
            }
            catch (SocketException ex)
            {
                server.LogOptional($"ClientSocket {id}. BeginReceive: SocketException {ex.Message}");
                CloseSocket();
                return;
            }
        }

        void ReadCallback(IAsyncResult ar)
        {
            int bytesRead;

            try
            {
                bytesRead = socket.EndReceive(ar);

                if (bytesRead == 0)
                {
                    server.LogOptional($"ClientSocket {id}. ReadCallback, EndReceive: bytesRead = 0, closing the socket");
                    CloseSocket();
                    return;
                }
            }
            catch (ObjectDisposedException)
            {
                server.LogOptional($"ClientSocket {id}. ReadCallback, EndReceive. ObjectDisposedException");
                CloseSocket();
                return;
            }
            catch (SocketException ex)
            {
                server.LogOptional($"ClientSocket {id}. ReadCallback, EndReceive. SocketException: {ex.Message}");
                CloseSocket();
                return;
            }

            // Copy data (we need buffer for the next receive operation) and raise DataReceived event
            byte[] data = new byte[bytesRead];
            Buffer.BlockCopy(buffer, 0, data, 0, bytesRead);

            var v = DataReceived;
            v?.Invoke(this, new DataReceivedEventArgs { Data = data, ClientId = id });

            // Continue to receive next data
            BeginReceive();
        }

        // Send data asynchronously
        public void Send(byte[] data)
        {
            try
            {
                socket.BeginSend(data, 0, data.Length, 0,
                    new AsyncCallback(SendCallback),
                    null);
            }
            catch (ObjectDisposedException)
            {
                server.LogOptional($"ClientSocket {id}. BeginSend. ObjectDisposedException");
                CloseSocket();
                return;
            }
            catch (SocketException ex)
            {
                server.LogOptional($"ClientSocket {id}. BeginSend. SocketException: {ex.Message}");
                CloseSocket();
                return;
            }

            return;
        }

        void SendCallback(IAsyncResult ar)
        {
            int bytesSent;

            try
            {
                bytesSent = socket.EndSend(ar);
            }
            catch (ObjectDisposedException)
            {
                server.LogOptional($"ClientSocket {id}. SendCallback, EndSend. ObjectDisposedException");
                CloseSocket();
                return;
            }
            catch (NullReferenceException)
            {
                server.LogOptional($"ClientSocket {id}. SendCallback, EndSend. NullReferenceException");
                CloseSocket();
                return;
            }
            catch (SocketException ex)
            {
                server.LogOptional($"ClientSocket {id}. SendCallback, EndSend. SocketException: {ex.Message}");
                CloseSocket();
                return;
            }

            //server.LogOptional($"ClientSocket {id}. {bytesSent} bytes sent");
        }

        void CloseSocket()
        {
            try
            {
                socket.Shutdown(SocketShutdown.Both);
            }
            catch (ObjectDisposedException)
            {
                server.LogOptional($"ClientSocket {id}. Shutdown. ObjectDisposedException");
            }
            catch (NullReferenceException)
            {
                server.LogOptional($"ClientSocket {id}. Shutdown. NullReferenceException");
            }
            catch (SocketException ex)
            {
                server.LogOptional($"ClientSocket {id}. Shutdown. SocketException: {ex.Message}");
            }

            try
            {
                socket.Close();
            }
            catch (Exception ex)    // precaution, exception type thrown by Close is not documented
            {
                server.LogOptional($"ClientSocket {id}. Close. Exception: {ex.Message}");
            }

            socket = null;

            var v = Disconnected;
            v?.Invoke(this, new DisconnectedEventArgs { ClientSocket = this });
        }
    }
}
