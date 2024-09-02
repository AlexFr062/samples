using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FindDevice
{
    static class Utilities
    {
        public static string ToHexArray(byte[] bytes)
        {
            int length = 0;

            if (bytes != null) length = bytes.Length;

            StringBuilder sb = new StringBuilder();

            sb.Append($"{length} bytes. ");

            for (int i = 0; i < length; ++i)
            {
                sb.Append($"{((int)bytes[i]):X2} ");
            }

            return sb.ToString();
        }
    }
}
