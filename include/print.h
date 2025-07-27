#ifndef PRINT_H_
#define PRINT_H_

__attribute__((format(printf, 1, 2)))
void print_err(const char* format, ...);

#endif  // PRINT_H_
