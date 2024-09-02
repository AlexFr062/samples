using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;


namespace FindDevice
{
    /// <summary>
    /// State for asynchronous UDP socket receive call
    /// </summary>
    class StateInfo
    {
        public UdpClient UdpClient { get; set; }
        public IPAddress Address { get; set; }          // host IP address
        public IPAddress SubnetMask { get; set; }       // host subnet mask
        public string AdapterName { get; set; }         // host network adapter name
        public string AdapterDescription { get; set; }  // host network adapter desctiption
    }
}
