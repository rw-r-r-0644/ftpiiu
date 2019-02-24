#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <coreinit/memfrmheap.h>
#include <coreinit/memheap.h>
#include <coreinit/screen.h>
#include <coreinit/thread.h>
#include <coreinit/cache.h>
#include <coreinit/time.h>
#include <vpad/input.h>
#include <whb/proc.h>
#include <whb/log_console.h>
#include <whb/log.h>
#include "ftp.h"
#include "virtualpath.h"
#include "net.h"

#define PORT                    21

void console_printf(const char *format, ...)
{
	char * tmp = NULL;

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

/* Entry point */
int main()
{
	WHBProcInit();
	WHBLogConsoleInit();

	initialise_network();
	MountVirtualDevices();

	console_printf(VERSION_STRING);

	uint32_t ip = network_gethostip();
    console_printf("Listening on %u.%u.%u.%u:%i", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, PORT);

    int serverSocket = create_server(PORT);
	int network_down = 0;

    while(WHBProcIsRunning() && serverSocket >= 0 && !network_down)
    {
        network_down = process_ftp_events(serverSocket);
        if(network_down)
            break;
		OSSleepTicks(OSMicrosecondsToTicks(1000));
    }

	cleanup_ftp();
	if(serverSocket >= 0)
        network_close(serverSocket);

	UnmountVirtualPaths();
	finalize_network();

	WHBLogConsoleFree();
	WHBProcShutdown();

    return 0;
}

