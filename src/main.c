#include "console.h"
#include "proc.h"
#include "ftp.h"
#include "virtualpath.h"
#include "net.h"

#define FTP_PORT	21

/* Entry point */
int main()
{
	initialize_proc();
	initialize_console();
	initialise_network();
	MountVirtualDevices();

	console_printf(VERSION_STRING);

	uint32_t ip = network_gethostip();
    console_printf("Listening on %u.%u.%u.%u:%i", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, FTP_PORT);

    int serverSocket = create_server(FTP_PORT);
	int network_down = 0;

    while(proc_is_running() && serverSocket >= 0 && !network_down)
    {
        network_down = process_ftp_events(serverSocket);
        if(network_down)
            break;
		proc_sleep(1000);
    }

	cleanup_ftp();
	if(serverSocket >= 0)
        network_close(serverSocket);

	UnmountVirtualPaths();
	finalize_network();
	finalize_console();
	finalize_proc();
    return 0;
}

