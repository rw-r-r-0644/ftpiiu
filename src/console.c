/*
 * console.c
 * Console output
 */

#include <coreinit/memfrmheap.h>
#include <coreinit/memheap.h>
#include <coreinit/screen.h>
#include <coreinit/thread.h>
#include <coreinit/cache.h>
#include <coreinit/time.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CONSOLE_LINES_TV	(27)
#define MAX_CONSOLE_LINES_DRC	(18)
#define FRAME_HEAP_TAG			(0x46545055)

static MEMHeapHandle MEM1Heap;

static void *sBufferTV, *sBufferDRC;
static uint32_t sBufferSizeTV, sBufferSizeDRC;

static char * consoleArrayTv[MAX_CONSOLE_LINES_TV];
static char * consoleArrayDrc[MAX_CONSOLE_LINES_DRC];

void console_printf(const char *format, ...)
{
	char * tmp = NULL;

	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va) >= 0) && tmp)
	{
		if(consoleArrayTv[0])
			free(consoleArrayTv[0]);
		if(consoleArrayDrc[0])
			free(consoleArrayDrc[0]);

		for(int i = 1; i < MAX_CONSOLE_LINES_TV; i++)
			consoleArrayTv[i-1] = consoleArrayTv[i];

		for(int i = 1; i < MAX_CONSOLE_LINES_DRC; i++)
			consoleArrayDrc[i-1] = consoleArrayDrc[i];

		if(strlen(tmp) > 79)
			tmp[79] = 0;

		consoleArrayTv[MAX_CONSOLE_LINES_TV-1] = strdup(tmp);
		consoleArrayDrc[MAX_CONSOLE_LINES_DRC-1] = (tmp);
	}
	va_end(va);

	OSScreenClearBufferEx(SCREEN_TV, 0);
	OSScreenClearBufferEx(SCREEN_DRC, 0);

	for(int i = 0; i < MAX_CONSOLE_LINES_TV; i++)
	{
		if(consoleArrayTv[i])
			OSScreenPutFontEx(SCREEN_TV, 0, i, consoleArrayTv[i]);
	}

	for(int i = 0; i < MAX_CONSOLE_LINES_DRC; i++)
	{
		if(consoleArrayDrc[i])
			OSScreenPutFontEx(SCREEN_DRC, 0, i, consoleArrayDrc[i]);
	}

	DCFlushRange(sBufferTV, sBufferSizeTV);
	DCFlushRange(sBufferDRC, sBufferSizeDRC);
	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
}

void initialize_console()
{
	for(int i = 0; i < MAX_CONSOLE_LINES_TV; i++)
		consoleArrayTv[i] = NULL;

	for(int i = 0; i < MAX_CONSOLE_LINES_DRC; i++)
		consoleArrayDrc[i] = NULL;

	MEM1Heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
	MEMRecordStateForFrmHeap(MEM1Heap, FRAME_HEAP_TAG);

	OSScreenInit();

	sBufferSizeTV = OSScreenGetBufferSizeEx(SCREEN_TV);
	sBufferSizeDRC = OSScreenGetBufferSizeEx(SCREEN_DRC);

	sBufferTV = MEMAllocFromFrmHeapEx(MEM1Heap, sBufferSizeTV, 4);
	sBufferDRC = MEMAllocFromFrmHeapEx(MEM1Heap, sBufferSizeDRC, 4);

	OSScreenSetBufferEx(SCREEN_TV, sBufferTV);
	OSScreenSetBufferEx(SCREEN_DRC, sBufferDRC);

	OSScreenEnableEx(SCREEN_TV, 1);
	OSScreenEnableEx(SCREEN_DRC, 1);
}

void finalize_console()
{
	for(int i = 0; i < MAX_CONSOLE_LINES_TV; i++)
		if(consoleArrayTv[i])
			free(consoleArrayTv[i]);

	for(int i = 0; i < MAX_CONSOLE_LINES_DRC; i++)
		if(consoleArrayDrc[i])
			free(consoleArrayDrc[i]);

	OSScreenShutdown();
	MEMFreeByStateToFrmHeap(MEM1Heap, FRAME_HEAP_TAG);
}

