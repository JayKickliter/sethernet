/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __ZEPHYR__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#else

#include <kernel.h>
#include <net/socket.h>
#include <uart.h>

#endif

#define PORT 4241

/* struct uart_config const UART0_CONFIG = { */

/* }; */


/* My simple test was hardwiring PTC14/15 at J199 pins 3-4 for a hardware echo test. */

void
main(void) {
    int                serv;
    struct sockaddr_in bind_addr;
    static int         counter;

    struct device * uart = device_get_binding("UART_0");
    if (!uart) {
        printf("Could not get a uart\n");
        exit(1);
    }

    serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serv < 0) {
        printf("error: socket: %d\n", errno);
        exit(1);
    }

    bind_addr.sin_family      = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_addr.sin_port        = htons(PORT);

    if (bind(serv, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        printf("error: bind: %d\n", errno);
        exit(1);
    }

    if (listen(serv, 5) < 0) {
        printf("error: listen: %d\n", errno);
        exit(1);
    }

    printf("Single-threaded TCP echo server waits for a connection on port "
           "%d...\n",
           PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t          client_addr_len = sizeof(client_addr);
        char               addr_str[INET_ADDRSTRLEN];
        int                client =
            accept(serv, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client < 0) {
            printf("error: accept: %d\n", errno);
            continue;
        }

        inet_ntop(client_addr.sin_family,
                  &client_addr.sin_addr,
                  addr_str,
                  sizeof(addr_str));
        printf("Connection #%d from %s\n", counter++, addr_str);

        while (1) {
            char buf[128];
            int  len = recv(client, buf, sizeof(buf), 0);

            if (len <= 0) {
                if (len < 0) {
                    printf("error: recv: %d\n", errno);
                }
                break;
            }

            for (int i = 0; i < len; ++i) {
                uart_poll_out(uart, buf[i]);
            }
        }

        close(client);
        printf("Connection from %s closed\n", addr_str);
    }
}
