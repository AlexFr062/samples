/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

#include "main.h"		// add this to use IP address and subnet mask

#define UDPECHO_THREAD_PRIO  ( tskIDLE_PRIORITY + 4 )

# if 0
static struct netconn *conn;
static struct netbuf *buf;
static ip_addr_t *addr;
static unsigned short port;
/*-----------------------------------------------------------------------------------*/

static void udpecho_thread(void *arg)
{
  err_t err, recv_err;
  
  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_UDP);
  if (conn!= NULL)
  {
    err = netconn_bind(conn, IP_ADDR_ANY, 7);
    if (err == ERR_OK)
    {
      while (1) 
      {
        recv_err = netconn_recv(conn, &buf);
      
        if (recv_err == ERR_OK) 
        {
          addr = netbuf_fromaddr(buf);
          port = netbuf_fromport(buf);
          netconn_connect(conn, addr, port);
          buf->addr.addr = 0;
          netconn_send(conn,buf);
          netbuf_delete(buf);
        }
      }
    }
    else
    {
      netconn_delete(conn);
    }
  }
}
/*-----------------------------------------------------------------------------------*/
void udpecho_init(void)
{
  sys_thread_new("udpecho_thread", udpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE,UDPECHO_THREAD_PRIO );
}

#else


static void udpbroadcast_thread(void *arg);

void udpecho_init(void)
{
    // Call our udpbroadcast_thread instead of udpecho_thread from the sample
    sys_thread_new("udpbroadcast_thread", udpbroadcast_thread, NULL, DEFAULT_THREAD_STACKSIZE,UDPECHO_THREAD_PRIO );
}

#define DETECTION_PORT 8
#define DETECTION_PACKET_LENGTH 10		// 0xA0, 0x50, IP address, network mask
#define PACKET0 0xA0
#define PACKET1 0x50

// Detection reply - hard-coded in this example.
static uint8_t reply[DETECTION_PACKET_LENGTH] =
{
        PACKET0, PACKET1,
        IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3,
        NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3
};

static void Error_Handler(void)
{
  while (1)
  {
  }
}

static void udpbroadcast_thread(void *arg)
{
    struct netbuf detectionBuf = {0};
    struct netconn *conn;
    struct netbuf *buf;
    unsigned short port;
    int32_t ipDetect;
    int32_t firstBuf;
    void* data;
    uint8_t* data8;
    u16_t len;

    if (netbuf_ref(&detectionBuf, reply, sizeof(reply)) != ERR_OK) Error_Handler();

    conn = netconn_new(NETCONN_UDP);

    if ( conn == NULL ) Error_Handler();

    if (netconn_bind(conn, IP_ADDR_ANY, DETECTION_PORT) != ERR_OK) Error_Handler();

    for(;;)
    {
        // Receive UDP packet
        if (netconn_recv(conn, &buf) != ERR_OK) Error_Handler();

        port = netbuf_fromport(buf);

        ipDetect = 0;			// 1 - this is detection packet
        firstBuf = 0;			// 1 - this is not the first datagram sub-buffer (in the case of a long datagaram)

        // Read all UDP packet and decide whether this is detection request
        do
        {
            netbuf_data(buf, &data, &len);

            if ( firstBuf == 0 )
            {
                if ( data != NULL  &&  len == DETECTION_PACKET_LENGTH )
                {
                    data8 = (uint8_t*)data;

                    if ( data8[0] == PACKET0 && data8[1] == PACKET1 )
                    {
                        ipDetect = 1;	// this is IP detection request
                    }
                }

                firstBuf = 1;
            }

        } while (netbuf_next(buf) >= 0);

        if ( ipDetect == 1 )
        {
            // Send detection reply
            netconn_sendto(conn, &detectionBuf, IP_ADDR_BROADCAST, port);
        }

        netbuf_delete(buf);
    }

}


#endif

#endif /* LWIP_NETCONN */
