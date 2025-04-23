#include "parser.h"

const char *Methods[] = {
    "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH",
};

const char *Versions[] = {
    "HTTP/1.1", "HTTP/1.0",
};

void get_token(char delimiter, size_t *p, char *tbuf, size_t tbuf_size, char *buf, size_t buf_size) {
    size_t i = 0;
    while (i < tbuf_size-1 && *p < buf_size) {
        if (buf[*p] == delimiter) {
            break;
        }
        tbuf[i++] = buf[(*p)++];
    }
    tbuf[i] = '\0';
}

int parse_request(struct request *req, char *buf, size_t buf_size) {
    size_t p = 0;
    char delimiter;
    char tbuf[MAX_REQ_LEN];

    // Parse method
    delimiter = ' ';
    get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);
    req->start_line.method = -1;
    for (int i = 0; i < HTTP_NUM_METHODS; i++) {
        if (strcmp(Methods[i], tbuf) == 0) {
            req->start_line.method = (ssize_t)i;
            break;
        }
    }
    if (req->start_line.method == -1) {
        log_debug("Invalid HTTP method: %s", tbuf);
        return HTTP_METHOD_NOT_ALLOWED;
    }

    p++;

    // Parse URI
    delimiter = ' ';
    req->start_line.uri[0] = '\0';
    get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);
    strcpy(req->start_line.uri, tbuf);

    p++;

    // Parse HTTP version
    delimiter = '\r';
    get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);
    req->start_line.version = -1;
    for (int i = 0; i < HTTP_NUM_VERSIONS; i++) {
        if (strcmp(Versions[i], tbuf) == 0) {
            req->start_line.version = (ssize_t)i;
            break;
        }
    }
    if (req->start_line.version == -1) {
        log_debug("Invalid HTTP version: %s", tbuf);
        return HTTP_HTTP_VERSION_NOT_SUPPORTED;
    }

    // Set default connection value based on HTTP version
    req->connection = req->start_line.version ? 1 : 0;

    p+=2;

    // Parse headers
    req->host[0] = '\0';
    while(1) {
        delimiter = ':';
        get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);

        p++;
        if (buf[p] == ' ') {
            p++;
        }

        delimiter = '\r';
        if (strcasecmp("Host", tbuf) == 0) {   
            get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);
            strcpy(req->host, tbuf);
        } else if (strcasecmp("Connection", tbuf) == 0) {
            get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);
            if (strcasecmp("close", tbuf) == 0) {
                req->connection = 1;
            } else if (strcasecmp("keep-alive", tbuf)) {
                req->connection = 0;
            }
        } else {
            get_token(delimiter, &p, tbuf, sizeof(tbuf), buf, buf_size);
        }

        p+=2;
        if (buf[p] == '\r') {
            p+=2;
            break;
        }
    }

    if (req->host[0] == '\0') {
        log_debug("Missing required Host header");
        return HTTP_BAD_REQUEST;
    }

    // TODO:- memcpy(req.body, &buf[p], sizeof(req.body));

    return 0;
}