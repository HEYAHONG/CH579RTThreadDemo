#ifndef FLASHAREA_H
#define FLASHAREA_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdlib.h"
uint32_t app_get_flasharea_addr(void);
uint32_t app_get_flasharea_size(void);
int app_flashdev_init(void);
#define APP_FLASHAREA_DEVNAME "uflash"

#ifdef __cplusplus
}
#endif

#endif
