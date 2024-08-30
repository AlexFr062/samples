//  Based on STM32 NUCLEO-F429ZI LwIP_HTTP_Server_Netconn_RTOS code example.

/*
 *
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
#include "main.h"


#include "lwip/sys.h"
#include "lwip/api.h"



#if 0
// Incomment for debugging
static void dump_data(char* prefix, char* data, int len)
{
    int i;

    if ( data == NULL ||  len == 0 )
    {
        printf("%s <no data>\n", prefix);
        return;
    }

    printf("%s ", prefix);

    for(i = 0; i < len; ++i)
    {
        printf(" %02X", (int)data[i]);
    }

    printf("\n");
}
#endif

/*-----------------------------------------------------------------------------------*/
void tcpserver_thread()
{
    struct netconn *conn, *newconn;
    err_t err, accept_err;
    struct netbuf *buf;
    void *data;
    u16_t len;

    while(g_lwipInitFinished == 0) {osDelay(10);}        // wait for LWIP ready

#if 1
    printf("%s started\n", __FUNCTION__);
#endif

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    if (conn != NULL)
    {
        // Bind connection
        err = netconn_bind(conn, NULL, LISTENING_PORT);

        if (err == ERR_OK)
        {
            /* Tell connection to go into listening mode. */
            netconn_listen(conn);

#if 1
            printf("%s listening port %d\n", __FUNCTION__, LISTENING_PORT);
#endif

            while (1)
            {
                /* Grab new connection. */
                accept_err = netconn_accept(conn, &newconn);

                /* Process the new connection. */
                if (accept_err == ERR_OK)
                {
                    printf("%s connection accepted\n", __FUNCTION__);

                    while (netconn_recv(newconn, &buf) == ERR_OK)
                    {
                        do
                        {
                            netbuf_data(buf, &data, &len);

                            //dump_data("received", data, len);

                            netconn_write(newconn, data, len, NETCONN_COPY);        // echo

                            if (len)
                            {
                                // Bit 0 of every received byte: 0 - LED1 off, 1 - LED1 on.
                                // Handle only the last byte, it defines the final state.
                                // GPIO_PIN_RESET = 0 - off
                                // PIO_PIN_SET = 1 - on

                                HAL_GPIO_WritePin(
                                    LD1_GPIO_Port,
                                    LD1_Pin,
                                    (GPIO_PinState)((((uint8_t*)data)[len-1]) & 1)
                                    );
                            }

                        } while (netbuf_next(buf) >= 0);

                        netbuf_delete(buf);
                    }

                    /* Close connection and discard connection identifier. */
                    netconn_close(newconn);
                    netconn_delete(newconn);

                    printf("%s connection closed\n", __FUNCTION__);
                }
                else
                {
                    printf("%s netconn_accept failed. err = %d\n", __FUNCTION__, (int)accept_err);
                }
            }
        }
        else
        {
            printf("%s netconn_bind failed. err = %d\n", __FUNCTION__, (int)err);

            netconn_delete(newconn);
        }
    }
    else
    {
        printf("%s conn = NULL\n", __FUNCTION__);
    }

    printf("%s ended\n", __FUNCTION__);   // should never be printed
}




