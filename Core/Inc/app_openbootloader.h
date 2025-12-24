#ifndef _APP_OPEN_BL_H__
#define _APP_OPEN_BL_H__

#include <stdint.h>

#define SPECIAL_CMD_MAX_NUMBER 0
#define EXTENDED_SPECIAL_CMD_MAX_NUMBER 0

#define FLASH_BL_SIZE (24*1024)

extern uint16_t SpecialCmdList[SPECIAL_CMD_MAX_NUMBER? SPECIAL_CMD_MAX_NUMBER: 1];
extern uint16_t ExtendedSpecialCmdList[EXTENDED_SPECIAL_CMD_MAX_NUMBER? EXTENDED_SPECIAL_CMD_MAX_NUMBER: 1];

int OpenBootloader_DeInit();

#endif
