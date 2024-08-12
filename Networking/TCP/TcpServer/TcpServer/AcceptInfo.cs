using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace TcpServer
{
    /// <summary>
    /// Class is used as parameter for AcceptCallback for Server listening socket.
    /// </summary>
    class AcceptInfo
    {
        public Socket Socket { get; set; }
        public int Id { get; set; }
    }
}
