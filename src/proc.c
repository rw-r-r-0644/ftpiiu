/*
 * proc.c
 * Process status control
 *
 * Most of the code is from wut's whb, however
 * home button is now denied even when not in hbl,
 * to prevent issues with memory and screen access
 * while in the home menu
 */

#include <wut.h>
#include <coreinit/time.h>
#include <coreinit/thread.h>
#include <coreinit/core.h>
#include <coreinit/exit.h>
#include <coreinit/foreground.h>
#include <coreinit/messagequeue.h>
#include <coreinit/systeminfo.h>
#include <coreinit/title.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>

#define HBL_TITLE_ID (0x0005000013374842)
#define MII_MAKER_JPN_TITLE_ID (0x000500101004A000)
#define MII_MAKER_USA_TITLE_ID (0x000500101004A100)
#define MII_MAKER_EUR_TITLE_ID (0x000500101004A200)

static volatile BOOL proc_running = FALSE;

static uint32_t proc_main_core;
static BOOL proc_from_hbl = FALSE;

static uint32_t proc_save_callback(void *context)
{
	OSSavesDone_ReadyToRelease();
	return 0;
}

static uint32_t proc_home_button_denied(void *context)
{
	proc_running = FALSE;
	return 0;
}

BOOL proc_is_running()
{
	ProcUIStatus status;

	if (proc_main_core != OSGetCoreId()) {
		ProcUISubProcessMessages(TRUE);
		return proc_running;
	}

	status = ProcUIProcessMessages(TRUE);
	if (status == PROCUI_STATUS_EXITING) {
		proc_running = FALSE;
	} else if (status == PROCUI_STATUS_RELEASE_FOREGROUND) {
		ProcUIDrawDoneRelease();
	}

	if (!proc_running) {
		ProcUIShutdown();
	}

	return proc_running;
}

void proc_sleep(uint32_t us)
{
	OSSleepTicks(OSMicrosecondsToTicks(1000));
}

void initialize_proc()
{
	uint64_t titleID = OSGetTitleID();
	if (titleID == HBL_TITLE_ID ||
		 titleID == MII_MAKER_JPN_TITLE_ID ||
		 titleID == MII_MAKER_USA_TITLE_ID ||
		 titleID == MII_MAKER_EUR_TITLE_ID) {
		proc_from_hbl = TRUE;
	}

	OSEnableHomeButtonMenu(FALSE);
	proc_main_core = OSGetCoreId();
	proc_running = TRUE;
	ProcUIInitEx(&proc_save_callback, NULL);
	ProcUIRegisterCallback(PROCUI_CALLBACK_HOME_BUTTON_DENIED,
		&proc_home_button_denied, NULL, 100);
}

void finalize_proc()
{
	proc_running = FALSE;
	if (proc_from_hbl) {
		SYSRelaunchTitle(0, NULL);
	}
}

