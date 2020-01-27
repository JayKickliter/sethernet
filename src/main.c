#include <drivers/uart.h>
#include <errno.h>
#include <kernel.h>
#include <net/socket.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 4241

void
serial_task(void);

void
net_task(void);

K_THREAD_DEFINE(serial /* name */,
                1024 /* stack_size */,
                serial_task /* entry */,
                NULL /* p1 */,
                NULL /* p2 */,
                NULL /* p3 */,
                5 /* prio */,
                0 /* options */,
                K_NO_WAIT /* delay */);

K_THREAD_DEFINE(net /* name */,
                1024 /* stack_size */,
                net_task /* entry */,
                NULL /* p1 */,
                NULL /* p2 */,
                NULL /* p3 */,
                5 /* prio */,
                0 /* options */,
                K_NO_WAIT /* delay */);

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

/* The following is just a placeholder and is not thread-safe. */
void
serial_task(void) {
    struct device * uart = device_get_binding("UART_0");
    if (!uart) {
        printf("Could not get a uart\n");
        exit(1);
    }

    for (uint32_t i = 0;; ++i) {
        k_sleep(1000);
        printf("%u serial_task\n", i);
    }
}

void
net_task(void) {
    for (uint32_t i = 0;; ++i) {
        k_sleep(1010);
        printf("%u net_task\n", i);
    }
}


/**************************************************************************/
/* Notes                                                                  */
/**************************************************************************/
/* My simple test was hardwiring PTC14/15 at J199 pins 3-4 for a hardware echo test. */
