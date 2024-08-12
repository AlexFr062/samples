using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TcpServer
{
    public class DataReceivedEventArgs : EventArgs
    {
        public byte[] Data { get; internal set; }
        public int ClientId { get; internal set; }
    }
}
