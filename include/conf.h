#ifndef CONF_H
#define CONF_H

// CTRL-C IN POLL
#define INTERRUPTED_SYSTEM_CALL 4

// UNUSED
#define SERVER_ROOT .
#define SERVER_ROOT_LEN 8192

// PARSER: METHOD AND VERSION ARRAYS
#define HTTP_NUM_METHODS 9
#define HTTP_NUM_VERSIONS 2

// MAIN LISTNER: SOCKET SETUP
#define HOST_NAME "127.0.0.1"
#define IPV4_ADDR "127.0.0.1"
#define IPV6_ADDR "::1"
#define PORT 8000
#define MAX_BACKLOG 20
#define DEFAULT_LOG_LVL LOG_LVL_DEBUG

// PARSER, ROUTER AND HANDLER
#define MAX_HOST_LEN 256
#define MAX_URI_LEN 8192
#define MAX_REQ_BODY_LEN 8192
#define MAX_REQ_LEN 2*MAX_REQ_BODY_LEN
#define MAX_RES_BODY_LEN 8192
#define MAX_RES_LEN 2*MAX_RES_BODY_LEN
#define MAX_PATH_LEN SERVER_ROOT_LEN+MAX_URI_LEN
#define MAX_ENTRY_NAME_LEN 256

// HANDLER AND ROUTER RESPONSE ENCODING
#define DIR_RES_FMT_STR "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: text/html; charset=utf-8\r\n\r\n%s"
#define FILE_RES_FMT_STR "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: application/octet-stream;\r\n\r\n%s"
#define ERR_RES_FMT_STR "HTTP/1.1 %d %s\r\nContent-Length: 0\r\n\r\n"
#define DIR_RES_BODY_FMT_STR "<!DOCTYPE HTML><html lang='en'><head><meta charset='UTF-8'><title>Directory Listing</title></head><body><h1>Directory Listing For %s</h1><hr><ul>%s</ul><hr></body></html>"
#define DIR_LIST_ITEM_FMT_STR "<li><a href='%s'>%s</a></li>"


#endif // CONF_H