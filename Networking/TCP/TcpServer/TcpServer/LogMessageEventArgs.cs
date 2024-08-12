using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TcpServer
{
    public enum LogMessageType
    {
        Normal,
        Optional
    }

    public class LogMessageEventArgs : EventArgs
    {
        public LogMessageType MessageType { get; internal set; }
        public string Message { get; internal set; }
    }
}
