#include "handler.h"
#include "logger.h"
#include "parser.h"

void http_return_error(int err_code, int client_sock_fd) {
  char res_buf[MAX_RES_LEN];
  int nbytes = snprintf(res_buf, MAX_RES_LEN, ERR_RES_FMT_STR, err_code,
                        http_status_message(err_code));
  if (send(client_sock_fd, res_buf, nbytes, 0) < 0) {
    log_error("Failed to send error response: %s", strerror(errno));
  }
  // Only log client errors at debug level, server errors at error level
  if (err_code >= 500) {
    log_error("Server error response: %d %s", err_code,
              http_status_message(err_code));
  } else {
    log_debug("Client error response: %d %s", err_code,
              http_status_message(err_code));
  }
  close(client_sock_fd);
  pthread_exit(NULL); // Exit the thread since we're done with this connection
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
      log_error("Connection read error: %s", strerror(errno));
      break;
    }
    if (nbytes == 0) {
      // Client closing connection is normal, only log at debug level
      log_debug("Client closed connection");
      break;
    }

    req_buf[nbytes] = '\0';
    err_code = parse_request(&req, req_buf, nbytes);
    if (err_code != 0) {
      http_return_error(err_code, client_sock_fd);
      continue;
    }

    // Only log request details at debug level
    log_debug("Processing request: %s %s", Methods[req.start_line.method],
              req.start_line.uri);

    res_body_size = MAX_RES_BODY_LEN;
    err_code = get_resource(req.start_line.uri, res_body_buf, &res_body_size);

    if (res_body_size < 0) {
      // Handle negative size as HTTP error code
      http_return_error(-res_body_size, client_sock_fd);
      continue;
    }

    if (err_code > 1) {
      http_return_error(err_code, client_sock_fd);
      continue;
    }

    nbytes = snprintf(res_buf, MAX_RES_LEN,
                      (err_code ? DIR_RES_FMT_STR : FILE_RES_FMT_STR),
                      res_body_size, res_body_buf);

    if (nbytes >= MAX_RES_LEN) {
      log_warn("Response truncated due to buffer limits");
      http_return_error(HTTP_INTERNAL_SERVER_ERROR, client_sock_fd);
      continue;
    }

    nbytes = send(client_sock_fd, res_buf, strlen(res_buf), 0);
    if (nbytes == -1) {
      log_error("Failed to send response: %s", strerror(errno));
      break;
    }

    // Only log successful responses at debug level
    log_debug("Response sent: 200 OK (%zd bytes)", nbytes);
  }

  close(client_sock_fd);
  return NULL;
}