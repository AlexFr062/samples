using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace EchoServer
{
    static class Utilities
    {
        public static string ByteArrayToHexString(byte[] data)
        {
            StringBuilder sb = new StringBuilder();

            if (data != null)
            {
                foreach (byte b in data)
                {
                    sb.Append($"{(int)b:X2} ");
                }
            }

            return sb.ToString();
        }
    }
}
