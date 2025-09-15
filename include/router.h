#ifndef ROUTER_H
#define ROUTER_H

#include "common.h"
#include "conf.h"
#include "logger.h"
#include "status.h"
#include "strutils.h"

void unescape_uri(char *uri, size_t uri_len);
int get_resource(char *uri, char *buf, ssize_t *buf_size);
ssize_t get_dir_resource(char *path, char *lst_buf, size_t lst_size);

#endif // ROUTER_H