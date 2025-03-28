#ifndef PARSER_H
#define PARSER_H


#include "conf.h"
#include "logger.h"
#include "status.h"
#include "common.h"


struct request
{
    // required
    struct {
        ssize_t method;
        char uri[MAX_URI_LEN];
        ssize_t version;
    } start_line;
    char host[MAX_HOST_LEN];

    // optional
    // 0 is for keep-alive and 1 is for close
    // if version is HTTP/1.0 close is the default
    // if version is HTTP/1.1 keep-alive is the default
    int connection;

    // request body
    char body[MAX_REQ_BODY_LEN];
};

void get_token(char delimiter, size_t *p, char *tbuf, size_t tbuf_size, char *buf, size_t buf_size);
int parse_request(struct request *req, char *buf, size_t buf_size);


#endif // PARSER_H