#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include "conf.h"

typedef enum {
  LOG_LVL_DEBUG,
  LOG_LVL_INFO,
  LOG_LVL_WARN,
  LOG_LVL_ERROR,
  LOG_LVL_CRITICAL,
} LogLevel;

extern FILE *g_cur_log_stream;
extern LogLevel g_cur_log_level;

/* If file_path is NULL, stderr is used */
void init_log(LogLevel log_level, char *file_path);

void close_log();

int log_msg(LogLevel log_level, char *fmtstr, ...);
#define log_debug(fmtstr, ...) log_msg(LOG_LVL_DEBUG, fmtstr, ##__VA_ARGS__)
#define log_info(fmtstr, ...) log_msg(LOG_LVL_INFO, fmtstr, ##__VA_ARGS__)
#define log_warn(fmtstr, ...) log_msg(LOG_LVL_WARN, fmtstr, ##__VA_ARGS__)
#define log_error(fmtstr, ...) log_msg(LOG_LVL_ERROR, fmtstr, ##__VA_ARGS__)
#define log_critical(fmtstr, ...)                                              \
  log_msg(LOG_LVL_CRITICAL, fmtstr, ##__VA_ARGS__)

#endif // LOGGER_H