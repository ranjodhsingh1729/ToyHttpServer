/*
**MAIN LISTENER**
*/

#include "server.h"

static volatile int stop_server = 0;

void interrupt_handler(int signum) {
    stop_server = 1;
    log_info("Received Signal: %d", signum);
}


int main() {
    signal(SIGINT, interrupt_handler);

    init_log(DEFAULT_LOG_LVL, "logs/server.log");

    log_info("INITIALISING SERVER");

    struct sockaddr_in server_sockaddr_in;
    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_port = htons(PORT);
    inet_pton(AF_INET, IPV4_ADDR, &server_sockaddr_in.sin_addr);

    struct sockaddr_in6 server_sockaddr_in6;
    server_sockaddr_in6.sin6_family = AF_INET6;
    server_sockaddr_in6.sin6_port = htons(PORT);
    inet_pton(AF_INET6, IPV6_ADDR, &server_sockaddr_in6.sin6_addr);

    int server_sock_in_fd, server_sock_in6_fd;
    if ((server_sock_in_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        log_error("socket %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        exit(1);
    }
    if ((server_sock_in6_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        log_error("socket %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        exit(1);
    }

    int opt = 1;
    setsockopt(server_sock_in_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(server_sock_in6_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_sock_in_fd, (struct sockaddr *)(&server_sockaddr_in), sizeof(server_sockaddr_in)) == -1) {
        log_error("bind %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        exit(1);
    }
    if (bind(server_sock_in6_fd, (struct sockaddr *)(&server_sockaddr_in6), sizeof(server_sockaddr_in6)) == -1) {
        log_error("bind %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        exit(1);
    }

    if (listen(server_sock_in_fd, MAX_BACKLOG) == -1) {
        log_error("listen %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        exit(1);
    }
    if (listen(server_sock_in6_fd, MAX_BACKLOG) == -1) {
        log_error("listen %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        exit(1);
    }

    log_info("SERVER IS NOW LISTNING...");

    struct pollfd pollfds[2];
    pollfds[0].fd = server_sock_in_fd;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = server_sock_in6_fd;
    pollfds[1].events = POLLIN;

    int client_sock_in_fd;
    int client_sock_in6_fd;
    struct sockaddr_in client_sockaddr_in;
    struct sockaddr_in6 client_sockaddr_in6;
    int client_sockaddr_in_size = sizeof(client_sockaddr_in);
    int client_sockaddr_in6_size = sizeof(client_sockaddr_in6);

    while (!stop_server) {
        if (poll(pollfds, (nfds_t)2, -1) == -1) {
            if (errno == INTERRUPTED_SYSTEM_CALL) {
                log_info("poll %s at %d: %s", __FILE__, __LINE__, strerror(errno));
                continue;
            }

            log_error("poll %s at %d: %s", __FILE__, __LINE__, strerror(errno));
            exit(1);
        }

        log_info("RECEIVED A NEW CONNECTION");

        if (pollfds[0].revents & POLLIN) {
            client_sock_in_fd = accept(server_sock_in_fd, (struct sockaddr *)(&client_sockaddr_in), (socklen_t *)(&client_sockaddr_in_size));
            if (client_sock_in_fd == -1) {
                log_error("accept %s at %d: %s", __FILE__, __LINE__, strerror(errno));
            } else {
                pthread_t thread;
                pthread_create(&thread, NULL, connection_handler, &client_sock_in_fd);
                pthread_detach(thread);
            }
        }

        if (pollfds[1].revents & POLLIN) {
            client_sock_in6_fd = accept(server_sock_in6_fd, (struct sockaddr *)(&client_sockaddr_in6), (socklen_t *)(&client_sockaddr_in6_size));
            if (client_sock_in6_fd == -1) {
                log_error("accept %s at %d: %s", __FILE__, __LINE__, strerror(errno));
            } else {
                pthread_t thread;
                pthread_create(&thread, NULL, connection_handler, &client_sock_in6_fd);
                pthread_detach(thread);
            }
        }

        if (client_sock_in_fd != -1) {
            log_info("HANDLER THREAD CREATED AND DETACHED");
        } else {
            log_warn("UNABLE TO ACCEPT CLIENT CONNECTION");
        }

    }

    log_info("STOPING SERVER");

    close(server_sock_in_fd);
    close(server_sock_in6_fd);

    close_log();

    return 0;
}