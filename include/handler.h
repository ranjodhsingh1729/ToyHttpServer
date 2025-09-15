#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include "conf.h"
#include "logger.h"
#include "parser.h"
#include "router.h"
#include "status.h"

/* worker for main listner */
void *connection_handler(void *arg);
void http_return_error(int error_code, int client_sock_fd);

#endif // HANDLER_H