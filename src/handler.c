#include "handler.h"

void http_return_error(int err_code, int client_sock_fd) {
    char res_buf[MAX_RES_LEN];
    int nbytes = snprintf(res_buf, MAX_RES_LEN, ERR_RES_FMT_STR, err_code, http_status_message(err_code));
    send(client_sock_fd, res_buf, nbytes, 0);
}

void *connection_handler(void *arg) {
    int client_sock_fd = *((int *)arg);

    int err_code;
    ssize_t nbytes;
    struct request req;
    char req_buf[MAX_REQ_LEN];
    char res_buf[MAX_RES_LEN];
    char res_body_buf[MAX_RES_BODY_LEN];
    ssize_t res_body_size;

    while (1) {
        nbytes = recv(client_sock_fd, req_buf, sizeof(req_buf), 0);
        if (nbytes == -1) {
            log_error("recv %s at %d: %s", __FILE__, __LINE__, strerror(errno));
            break;
        }
        if (nbytes == 0) {
            log_info("recv %s at %d: %s", __FILE__, __LINE__, "Connection Gracefully Closed By Peer");
            break;
        }

        req_buf[nbytes] = '\0';
        fprintf(stdout, "REQ:-\n%s\n", req_buf);
        fprintf(stdout, "%s\n", "--------------------------------");

        err_code = parse_request(&req, req_buf, nbytes);
        if (err_code != 0) {
            log_info("parse_request %s at %d:  %s", __FILE__, __LINE__, strerror(errno));
            http_return_error(err_code, client_sock_fd);
            continue;
        }

        fprintf(stdout, "PARSED REQ:-\n");
        fprintf(stdout, "METHOD: %ld\n", req.start_line.method);
        fprintf(stdout, "URI: %s\n", req.start_line.uri);
        fprintf(stdout, "VERSION: %ld\n", req.start_line.version);
        fprintf(stdout, "HOST: %s\n", req.host);
        fprintf(stdout, "Connection: %d\n", req.connection);
        fprintf(stdout, "%s\n", "--------------------------------");

        res_body_size = MAX_RES_BODY_LEN;
        err_code = get_resource(req.start_line.uri, res_body_buf, &res_body_size);
        if (err_code > 1) {
            log_info("get_resource: %s at %d", __FILE__, __LINE__);
            http_return_error(err_code, client_sock_fd);
            continue;
        }
        nbytes = snprintf(res_buf, MAX_RES_LEN, (err_code ? DIR_RES_FMT_STR : FILE_RES_FMT_STR), res_body_size, res_body_buf);

        res_buf[nbytes] = '\0';
        fprintf(stdout, "RES:-\n%s\n", res_buf);
        fprintf(stdout, "%s\n", "--------------------------------");

        nbytes = send(client_sock_fd, res_buf, strlen(res_buf), 0);
        if (nbytes == -1) {
            log_error("send %s at %d: %s", __FILE__, __LINE__, strerror(errno));
            break;
        }
    }

    close(client_sock_fd);

    return NULL;
}