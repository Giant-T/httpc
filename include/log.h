#ifndef LOG_H
#define LOG_H

__attribute__((format(printf, 1, 2)))
void log_err(const char* format, ...);

__attribute__((format(printf, 1, 2)))
void log_info(const char* format, ...);

#endif // LOG_H once
