#include "router.h"

/* unscapes uri % escapes */
void unescape_uri(char *uri, size_t uri_len) {
    char x;
    size_t i = 0, j = 0;
    while (i < uri_len) {
        if (i < uri_len-2 && uri[i] == '%' && isxdigit(uri[i+1]) && isxdigit(uri[i+1])) {
            x = (char)((uri[i+1]-'0')*16+(uri[i+2]-'0'));
            uri[j++] = x;
            i+=3;
        } else {
            uri[j++] = uri[i++];
        }
    }
    uri[j] = '\0';
}

/* ignores ./ and <entry>/../ */
void convert_to_absolute_path(char *path, size_t path_len) {
    int prv_sep = 0;
    size_t i = 0, j = 0;
    while (path[i] || i < path_len) {
        if (path[i] == '/') {
            prv_sep = i;
        }

        if (i < path_len-1 && path[i] == '.' && path[i+1] == '/') {
            i+=2;
            continue;
        }
        if (i < path_len-2 && path[i] == '.' && path[i+1] == '.' && path[i+2] == '/') {
            i+=3;
            j=prv_sep;
            continue;
        }

        path[j++] = path[i++];
    }

    path[j] = '\0';
}

/* At most <file_size_max> bytes of file at <path> is read into <file_buf> */
ssize_t get_file_resource(char *path, char *file_buf, size_t file_size_max) {
    FILE *fd = fopen(path, "rb");
    if (fd == NULL) {
        log_info("fopen %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        return -1;
    }

    fseek(fd, 0, SEEK_END);
    size_t filesize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    if (filesize > file_size_max) {
        log_error("FILE TOO LARGE");
        return -1;
    }

    ssize_t nchunks = fread(file_buf, filesize, 1, fd);
    if (nchunks == -1) {
        log_error("read %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        return -1;
    }

    fclose(fd);
    return filesize;
}

/* A directory at <path> listed in form of html and atmost <dir_size_max> bytes are written into <dir_buf> */
ssize_t get_dir_resource(char *path, char *dir_buf, size_t dir_size_max) {
    char lst_buf[MAX_RES_BODY_LEN];
    size_t lst_size_max = MAX_RES_BODY_LEN;

    size_t p = 0, t;

    struct dirent *entry;
    DIR *dir = opendir(path);
    if (!dir) {
        log_error("opendir(path): %s", strerror(errno));
        return -1;
    }

    char entry_name[MAX_ENTRY_NAME_LEN];
    while (p < lst_size_max && (entry = readdir(dir)) != NULL) {
        if (strcmp(".", entry->d_name) == 0) {
            continue;
        }
        if (strcmp("..", entry->d_name) == 0) {
            continue;
        }


        t = stricpy(entry_name, entry->d_name);
        if (entry->d_type == DT_DIR) {
            t += stricpy(&entry_name[t], "/");
        }
        entry_name[t] = '\0';


        p += snprintf(
            &lst_buf[p],
            lst_size_max-p,
            DIR_LIST_ITEM_FMT_STR,
            entry_name,
            entry_name
        );

    }
    lst_buf[p] = '\0';

    p = snprintf(
        dir_buf,
        dir_size_max,
        DIR_RES_BODY_FMT_STR,
        path,
        lst_buf
    );

    closedir(dir);
    return p;
}

/*
 *Decodes uri and appends it to server root
 *removes all ./ and ../ from the combined path
 *makes sure that the path is inside server root
 *if the path is to a file a octet stream response is given
 *if the path is to a directory a html directory listing is given
 *return value 0 => file and 1 => folder
 */
int get_resource(char *uri, char *buf, ssize_t *buf_len) {
    char curr_path[MAX_PATH_LEN];
    char test_path[MAX_PATH_LEN];

    if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
        log_info("getcwd in %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        return HTTP_FORBIDDEN;
    }

    strcpy(test_path, curr_path);
    unescape_uri(uri, strlen(uri));
    strcpy(&test_path[strlen(curr_path)], uri);
    convert_to_absolute_path(test_path, strlen(test_path));

    if (strlen(test_path) < strlen(curr_path)) {
        log_info("PATH SMALLER THAN SERVER ROOT: %s at %d", __FILE__, __LINE__);
        return HTTP_FORBIDDEN;
    }
    if (strncmp(test_path, curr_path, strlen(curr_path)) != 0) {
        log_info("PATH OUTSIDE OF SERVER ROOT: %s at %d", __FILE__, __LINE__);
        return HTTP_FORBIDDEN;
    }
    printf("%s\n", test_path);
    fflush(stdout);
    if (access(test_path, F_OK) != 0) {
        log_info("access %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        return HTTP_NOT_FOUND;
    }
    if (access(test_path, R_OK) != 0) {
        log_info("access %s at %d: %s", __FILE__, __LINE__, strerror(errno));
        return HTTP_FORBIDDEN;
    }

    int f = 0;
    for (size_t i = 0; uri[i]; i++) {
        f = (uri[i] == '/');
    }

    ssize_t nbytes;
    if (f) {
        nbytes = get_dir_resource(test_path, buf, *buf_len);
    } else {
        nbytes = get_file_resource(test_path, buf, *buf_len);
    }

    if (nbytes < -1) {
        log_info("REQUEST FAILED DUE TO PREVIOUS ISSUE: %s at %d", __FILE__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (nbytes >= MAX_RES_BODY_LEN-1) {
        log_info("RESPONSE TRUNCATED: %s at %d", __FILE__, __LINE__);
    }

    *buf_len = nbytes;
    return f;
}

