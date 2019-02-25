#ifndef _PROC_H_
#define _PROC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <wut.h>
#include <stdint.h>

BOOL proc_is_running();
void proc_sleep(uint32_t us);

void initialize_proc();
void finalize_proc();

#ifdef __cplusplus
}
#endif

#endif /* _PROC_H_ */
