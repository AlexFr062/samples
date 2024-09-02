using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;

namespace FindDevice
{
    class DeviceInfo
    {
        public IPAddress HostIpAddress { get; internal set; }
        public IPAddress HostSubnetMask { get; internal set; }
        public IPAddress DeviceIpAddress { get; internal set; }
        public IPAddress DeviceSubnetMask { get; internal set; }
        public string AdapterName { get; set; }                     // host network adapter name
    }
}
