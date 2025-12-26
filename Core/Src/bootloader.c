#include <app_openbootloader.h>
#include <openbl_core.h>
#include <openbl_usart_cmd.h>
#include <openbl_mem.h>
#include <usart_interface.h>
#include <interfaces_conf.h>

int OpenBootloader_DeInit()
{

	return 0;
}

#define APP_ADDRESS ( 0x8000000 + 0x3000)
// #define RETAIN_MEM 0x20007fc0



static void CheckStopBoot()
{
	const char* stop_boot_char = "StopBoot\n";
	char buff[9] = {0};

	uint32_t start_time = HAL_GetTick();

	while(1)
	{
		  while (!LL_USART_IsActiveFlag_RXNE_RXFNE(USARTx))
		  {
			  if ((HAL_GetTick() - start_time) >= 1000)
			  {
				  OPENBL_MEM_JumpToAddress(APP_ADDRESS);

			  }
		  }

		  for(uint8_t i = 0; i < (sizeof(buff) - 1); i++)
		  {
			  buff[i] = buff[i+1];
		  }
		  buff[sizeof(buff) - 1] =  LL_USART_ReceiveData8(USARTx);

		  int is_match = 1;
		  for(uint8_t i = 0; i < sizeof(buff); i++)
		  {
			  if(buff[i] != stop_boot_char[i])
			  {
				  is_match = 0;
				  break;
			  }
		  }

		  if(is_match)
		  {
			  OPENBL_USART_SendByte('O');
			  OPENBL_USART_SendByte('K');
			  OPENBL_USART_SendByte('\n');
			  return;
		  }
	}
}

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

	CheckStopBoot();

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
