#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdarg.h>

void console_printf(const char *format, ...);

void initialize_console();
void finalize_console();

#ifdef __cplusplus
}
#endif

#endif /* _CONSOLE_H_ */
