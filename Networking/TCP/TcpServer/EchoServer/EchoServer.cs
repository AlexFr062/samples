using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;

using TcpServer;

namespace EchoServer
{
    /// <summary>
    /// Echo server based on TcpServer.Server class.
    /// </summary>
    class EchoServer
    {
        public void Run(int port, AddressFamily family)
        {
            SyncConsole.Print($"EchoServer port {port}, Address family {family}");

            Server server = new Server();
            server.LogMessage += Server_LogMessage;
            server.ClientConnection += Server_ClientConnection;
            server.DataReceived += Server_DataReceived;

            try
            {
                server.Start(port, family);
            }
            catch (ServerException ex)
            {
                SyncConsole.Print(ex.Message);
                return;
            }

            SyncConsole.Print("Echo server is running. Press any other key to stop");

            Console.ReadKey(true);  // true - don't display the key

            server.Stop();
        }

        private void Server_LogMessage(object sender, LogMessageEventArgs e)
        {
            SyncConsole.Print($"Server: {e.Message}");
        }

        private void Server_ClientConnection(object sender, ClientConnectionEventArgs e)
        {
            SyncConsole.Print($"Client {e.ClientId} {(e.EventType == ConnectionEventType.Connected ? "Connected" : "disconnected")}");
        }

        private void Server_DataReceived(object sender, DataReceivedEventArgs e)
        {
            //SyncConsole.Print($"{e.Data.Length} bytes received from client {e.ClientId}: {Utilities.ByteArrayToHexString(e.Data)}");

            Server server = (Server)sender;

            try
            {
                server.Send(e.Data, e.ClientId);
            }
            catch (ServerException ex)
            {
                SyncConsole.Print($"Error sending {e.Data.Length} bytes to client {e.ClientId}: {ex.Message}");
            }
        }
    }
}
