#ifndef HANDLER_H
#define HANDLER_H

#include "conf.h"
#include "logger.h"
#include "parser.h"
#include "status.h"
#include "router.h"
#include "common.h"


/* worker for main listner */
void *connection_handler(void *arg);
void http_return_error(int error_code, int client_sock_fd);


#endif // HANDLER_H