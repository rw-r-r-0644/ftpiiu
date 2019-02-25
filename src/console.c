#include <whb/log_console.h>
#include <whb/log.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

void console_printf(const char *format, ...)
{
	char *tmp = NULL;

	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va) >= 0) && tmp)
	{
		WHBLogPrint(tmp);
		WHBLogConsoleDraw();
	}
	va_end(va);
	
	free(tmp);
}

void initialize_console()
{
	WHBLogConsoleInit();
}

void finalize_console()
{
	WHBLogConsoleFree();
}
