using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TcpServer
{
    /// <summary>
    /// Arguments for internal event raised synchronously
    /// from ClientSocket.
    /// </summary>
    class DisconnectedEventArgs : EventArgs
    {
        public ClientSocket ClientSocket { get; set; }
    }
}
