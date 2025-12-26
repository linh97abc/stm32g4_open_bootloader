#include <app_openbootloader.h>
#include <openbl_core.h>
#include <openbl_usart_cmd.h>
#include <openbl_mem.h>
#include <usart_interface.h>

int OpenBootloader_DeInit()
{

	return 0;
}

#define APP_ADDRESS ( 0x8000000 + 0x3800)
// #define RETAIN_MEM 0x20007fc0

int main_task()
{

	OPENBL_HandleTypeDef interface;

	extern OPENBL_MemoryTypeDef FLASH_Descriptor;
	extern OPENBL_MemoryTypeDef ICP_Descriptor;
	OPENBL_MEM_RegisterMemory(&FLASH_Descriptor);
	OPENBL_MEM_RegisterMemory(&ICP_Descriptor);

//	OPENBL_USART_SendByte(0x55);

//	if(*(uint32_t*)RETAIN_MEM != 1)
//	{
//		OPENBL_MEM_JumpToAddress(APP_ADDRESS);
//
//	}

	OPENBL_OpsTypeDef ops;
	ops.Init = OPENBL_USART_Configuration;
	ops.DeInit = OPENBL_USART_DeInit;
	ops.Detection = OPENBL_USART_ProtocolDetection;
	ops.GetCommandOpcode = OPENBL_USART_GetCommandOpcode;
	ops.SendByte = OPENBL_USART_SendByte;

	interface.p_Ops = &ops;
	interface.p_Cmd = OPENBL_USART_GetCommandsList();

	OPENBL_RegisterInterface(&interface);
	OPENBL_Init();

	OPENBL_InterfaceDetection();

	while(1)
	{
		OPENBL_CommandProcess();
	}
}
