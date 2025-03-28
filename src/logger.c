#include "logger.h"


FILE *g_cur_log_stream = NULL;
LogLevel g_cur_log_level = LOG_LVL_DEBUG;


void init_log(LogLevel log_level, char *file_path) {
    g_cur_log_level = log_level;
    if (file_path != NULL) {
        g_cur_log_stream = fopen(file_path, "a");
    } else {
        g_cur_log_stream = stderr;
    }
    if (g_cur_log_stream == NULL) {
        g_cur_log_stream = stderr;
        log_critical("fopen %s at %d: %s", __FILE__, __LINE__, strerror(errno));
    }
    // THIS INCREASES DISK READ WRITES!!!
    setvbuf(g_cur_log_stream, NULL, _IONBF, (size_t)0);
    log_info("------LOG SESSION STARTS------");
}

void close_log() {
    log_info("-------LOG SESSION OVER--------");
    fclose(g_cur_log_stream);
}

int log_msg(LogLevel lg_lvl, char *fmtstr, ...) {
    int bytes_writen = -1;
    if (lg_lvl < g_cur_log_level) {
        return bytes_writen;
    }

    time_t curr_time = 0;
    curr_time = time(NULL);
    bytes_writen += fprintf(g_cur_log_stream, "[%ld] ", curr_time);

    switch (lg_lvl)
    {
    case LOG_LVL_DEBUG:
        bytes_writen += fprintf(g_cur_log_stream, "[DEBUG] ");
        break;
    case LOG_LVL_INFO:
        bytes_writen += fprintf(g_cur_log_stream, "[INFO] ");
        break;
    case LOG_LVL_WARN:
        bytes_writen += fprintf(g_cur_log_stream, "[WARN] ");
        break;
    case LOG_LVL_ERROR:
        bytes_writen += fprintf(g_cur_log_stream, "[ERROR] ");
        break;
    case LOG_LVL_CRITICAL:
        bytes_writen += fprintf(g_cur_log_stream, "[CRITICAL] ");
        break;
    default:
        bytes_writen += fprintf(g_cur_log_stream, "[UNKNOWN] ");
        break;
    }

    va_list args;
    va_start(args, fmtstr);
    vfprintf(g_cur_log_stream, fmtstr, args);
    va_end(args);

    fprintf(g_cur_log_stream, "\n");

    return bytes_writen;
}