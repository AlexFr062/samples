using System.Net.Sockets;
using System.Net;
using System.Security;


namespace TcpServer
{
    /// <summary>
    /// TCP server.
    /// Thread safety:
    ///     Initialization, cleanup - not thread safe.
    ///     Send, Broadcast etc. - function used when server is initialized, are thread safe.
    /// 
    /// Events: all events are raised asynchronously.
    /// 
    /// Supports IPv4 and IPv6.
    /// 
    /// Server behavior can be customized by client, using server events and public methods.
    /// For example, to make an echo server, client may call Send function from DataReceived event.
    /// To make client stress test, client may call Send/Broadcast messages periodically
    /// all time the server is running.
    /// </summary>
    public class Server
    {
        #region Class members

        private Task task;
        private AutoResetEvent acceptEvent = new AutoResetEvent(false);
        private Socket listeningSocket;
        private List<ClientSocket> clients = new List<ClientSocket>();
        private object clientSync = new object();           // for clients access synchronization

        #endregion

        #region Events

        public event EventHandler<LogMessageEventArgs> LogMessage;
        public event EventHandler<ClientConnectionEventArgs> ClientConnection;
        public event EventHandler<DataReceivedEventArgs> DataReceived;

        #endregion

        #region Initialization, Stop

        /// <summary>
        /// Start server.
        /// </summary>
        /// <param name="port">TCP port to listen</param>
        /// <exception cref="ServerException">ServerException</exception>
        public void Start(int port, AddressFamily family)
        {
            if (task != null)
            {
                throw new ServerException("Server is already running");
            }

            if (family == AddressFamily.InterNetwork || family == AddressFamily.InterNetworkV6)
            {
            }
            else
            {
                throw new ServerException("Parameter family can be InterNetwork (IPv4) or InterNetworkV6 (IPv6)");
            }

            try
            {
                listeningSocket = new Socket(family,
                      SocketType.Stream, ProtocolType.Tcp);

                LogOptional("Socket is created");

                // IPAddress.Any - all available network adapters.
                listeningSocket.Bind(new IPEndPoint(
                    (family == AddressFamily.InterNetwork ? IPAddress.Any : IPAddress.IPv6Any),
                    port));

                LogOptional("Socket is bound");

                listeningSocket.Listen(100);

                LogOptional("Socket is listening");
            }
            catch (SocketException ex)
            {
                HandleException(ex);
            }
            catch (SecurityException ex)
            {
                HandleException(ex);
            }

            task = Task.Run(() => { ServerTask(); });
        }

        /// <summary>
        /// Stop server.
        /// </summary>
        public void Stop()
        {
            if (task != null)
            {
                try
                {
                    listeningSocket.Close();
                }
                catch (Exception ex)    // precaution, exception type thrown by close is not documented
                {
                    Log($"listeningSocket.Close. Exception: {ex.Message}");
                }

                task.Wait();
                task.Dispose();
                task = null;
            }
        }

        #endregion

        #region Properties

        public int ClientCount
        {
            get
            {
                lock (clientSync)
                {
                    return clients.Count;
                }
            }
        }

        #endregion

        #region Public methods

        /// <summary>
        /// Send data to all connected clients
        /// </summary>
        /// <param name="data">Data to send</param>
        /// <exception cref="ServerException">ServerException</exception>
        public void Broadcast(byte[] data)
        {
            if (data == null || data.Length == 0)
            {
                throw new ServerException("Empty data");
            }

            if (task == null)
            {
                throw new ServerException("Server is not connected");
            }

            ClientSocket[] cl = null;

            lock (clientSync)
            {
                cl = clients.ToArray();
            }

            if (cl == null || cl.Length == 0)
            {
                throw new ServerException("No clients connected");
            }

            foreach (var v in cl)
            {
                v.Send(data);
            }
        }

        /// <summary>
        /// Send data to client
        /// </summary>
        /// <param name="data">Data to send</param>
        /// <param name="clientId">Client ID</param>
        /// <exception cref="ServerException">ServerException</exception>
        public void Send(byte[] data, int clientId)
        {
            if (data == null || data.Length == 0)
            {
                throw new ServerException("Empty data");
            }

            if (task == null)
            {
                throw new ServerException("Server is not connected");
            }

            ClientSocket cl = null;

            lock (clientSync)
            {
                cl = clients.Find((ClientSocket s) => { return s.Id == clientId; });
            }

            if (cl == null)
            {
                throw new ServerException($"Client {clientId} is not found");
            }

            cl.Send(data);
        }


        #endregion

        #region TCP Server

        void ServerTask()
        {
            int clientID = 0;

            // Accept incoming connections
            for (; ; )
            {
                try
                {
                    listeningSocket.BeginAccept(
                        new AsyncCallback(AcceptCallback),
                        new AcceptInfo { Socket = listeningSocket, Id = clientID });
                }
                catch (ObjectDisposedException)
                {
                    LogOptional("Listening socket is closed");
                    break;
                }
                catch (SocketException ex)
                {
                    LogOptional($"Listening socket exception: {ex.Message}");
                    break;
                }

                ++clientID;

                // Wait until a connection is made before continuing.  
                acceptEvent.WaitOne();
            }

            LogOptional("ServerTask ended");
        }

        void AcceptCallback(IAsyncResult ar)
        {
            // Signal ServerTask to continue.  
            acceptEvent.Set();

            Socket handler = null;
            AcceptInfo info = (AcceptInfo)ar.AsyncState;

            try
            {
                handler = info.Socket.EndAccept(ar);
            }
            catch (ObjectDisposedException)
            {
                Log("EndAccept: ObjectDisposedException");
                return;
            }
            catch (SocketException ex)
            {
                Log($"EndAccept: SocketException {ex.Message}");
                return;
            }

            // Create socket, subscribe to event and add to the list
            ClientSocket clientSocket = new ClientSocket(this, handler, info.Id);
            clientSocket.Disconnected += ClientSocket_Disconnected;
            clientSocket.DataReceived += ClientSocket_DataReceived;

            lock (clientSync)
            {
                clients.Add(clientSocket);
            }

            RaiseClientConnection(ConnectionEventType.Connected, info.Id);

            // Now start the socket
            clientSocket.Start();
        }

        /// <summary>
        /// Event handler, raised from ClientSocket synchronously
        /// </summary>
        private void ClientSocket_Disconnected(object sender, DisconnectedEventArgs e)
        {
            bool result = false;

            lock (clientSync)
            {
                result = clients.Remove(e.ClientSocket);
            }

            if (!result)
            {
                LogOptional($"Client {e.ClientSocket.Id} is not found in the clients list");
            }

            // Raise ClientConnection asynchronously
            RaiseClientConnection(ConnectionEventType.Disconnected, e.ClientSocket.Id);
        }

        /// <summary>
        /// Event handler, raised from ClientSocket synchronously
        /// </summary>
        private void ClientSocket_DataReceived(object sender, DataReceivedEventArgs e)
        {
            // Raise DataReceived asynchronously
            RaiseDataReceived(e);
        }

        #endregion

        #region Raise events

        // Raise LogMessage event asynchronously
        internal void Log(string message, LogMessageType type)
        {
            Task.Run(() =>
            {
                var v = LogMessage;
                v?.Invoke(this, new LogMessageEventArgs { Message = message, MessageType = type });
            });
        }

        internal void Log(string message)
        {
            Log(message, LogMessageType.Normal);
        }

        internal void LogOptional(string message)
        {
            Log(message, LogMessageType.Optional);
        }

        // Raise ClientConnection event asynchronously
        void RaiseClientConnection(ConnectionEventType type, int clientId)
        {
            Task.Run(() =>
            {
                var v = ClientConnection;
                v?.Invoke(this, new ClientConnectionEventArgs { EventType = type, ClientId = clientId });
            });
        }

        // Raise DataReceived event asynchronously.
        // Note: Replaced with synchronous version, asynchronous can change the data order for client.
        void RaiseDataReceived(DataReceivedEventArgs args)
        {
            //Task.Run(() =>
            //{
            var v = DataReceived;
            v?.Invoke(this, args);
            //});
        }

        #endregion

        #region Other functions

        static void HandleException(Exception ex)
        {
            throw new ServerException(ex.Message, ex);
        }

        #endregion
    }
}
