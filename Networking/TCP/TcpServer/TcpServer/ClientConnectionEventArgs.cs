using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TcpServer
{
    public enum ConnectionEventType
    {
        Connected,
        Disconnected
    }

    public class ClientConnectionEventArgs : EventArgs
    {
        public ConnectionEventType EventType { get; internal set; }
        public int ClientId { get; internal set; }
    }
}
