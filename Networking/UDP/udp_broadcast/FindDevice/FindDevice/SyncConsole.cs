using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FindDevice
{
    static class SyncConsole
    {
        static object syncObject = new object();

        public static void Print(string s)
        {
            lock (syncObject)
            {
                Console.WriteLine(s);
            }
        }
    }
}
