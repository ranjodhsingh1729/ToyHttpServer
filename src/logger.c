#include "logger.h"

FILE *g_cur_log_stream = NULL;
LogLevel g_cur_log_level = LOG_LVL_INFO; // Default to INFO level

void init_log(LogLevel log_level, char *file_path) {
  g_cur_log_level = log_level;
  if (file_path != NULL) {
    g_cur_log_stream = fopen(file_path, "a");
  }
  if (g_cur_log_stream == NULL) {
    g_cur_log_stream = stderr;
    fprintf(stderr, "[CRIT] Failed to open log file: %s\n", strerror(errno));
  }

  // Use line buffering for logs
  setvbuf(g_cur_log_stream, NULL, _IOLBF, 0);
  log_info("Server logging initialized");
}

void close_log() {
  if (g_cur_log_stream != stderr) {
    fclose(g_cur_log_stream);
  }
}

int log_msg(LogLevel lg_lvl, char *fmtstr, ...) {
  if (lg_lvl < g_cur_log_level) {
    return 0;
  }

  time_t curr_time = time(NULL);
  struct tm *tm_info = localtime(&curr_time);

  int bytes_written =
      fprintf(g_cur_log_stream, "[%04d-%02d-%02d %02d:%02d:%02d] ",
              tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
              tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

  switch (lg_lvl) {
  case LOG_LVL_DEBUG:
    bytes_written += fprintf(g_cur_log_stream, "[DEBUG] ");
    break;
  case LOG_LVL_INFO:
    bytes_written += fprintf(g_cur_log_stream, "[INFO] ");
    break;
  case LOG_LVL_WARN:
    bytes_written += fprintf(g_cur_log_stream, "[WARN] ");
    break;
  case LOG_LVL_ERROR:
    bytes_written += fprintf(g_cur_log_stream, "[ERROR] ");
    break;
  case LOG_LVL_CRITICAL:
    bytes_written += fprintf(g_cur_log_stream, "[CRIT] ");
    break;
  default:
    bytes_written += fprintf(g_cur_log_stream, "[???] ");
    break;
  }

  va_list args;
  va_start(args, fmtstr);
  bytes_written += vfprintf(g_cur_log_stream, fmtstr, args);
  va_end(args);

  fprintf(g_cur_log_stream, "\n");
  return bytes_written;
}