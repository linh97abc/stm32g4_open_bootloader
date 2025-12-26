/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "openbl_mem.h"
#include "app_openbootloader.h"
#include "common_interface.h"
#include "flash_interface.h"
#include "openbl_core.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
FLASH_ProcessTypeDef FlashProcess = {.Lock = HAL_UNLOCKED, \
                                     .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                     .ProcedureOnGoing = 0U, \
                                     .Address = 0U, \
                                     .Bank = FLASH_BANK_1, \
                                     .Page = 0U, \
                                     .NbPagesToErase = 0U
                                    };

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *ListOfPages, uint32_t Length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);


/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_START_ADDRESS,
  FLASH_END_ADDRESS,
  FLASH_BL_SIZE,
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  OPENBL_FLASH_SetReadOutProtectionLevel,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Unlock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Unlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Lock(void)
{
  HAL_FLASH_Lock();
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval None.
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  HAL_FLASH_Unlock();

  HAL_FLASH_OB_Unlock();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_FLASH_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in FLASH memory.
  * @param  Address The address where that data will be written.
  * @param  Data The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength)
{
  uint32_t index;
  uint32_t length = DataLength;
  uint32_t remainder;
  uint64_t remainder_data = 0;

  /* Check the remaining of quad-word */
  remainder = length & 0x7U;

  if (remainder)
  {
    length = (length & 0xFFFFFFF8U);

    uint8_t *premainder_data = (uint8_t*) &remainder_data;
    /* copy the remaining bytes */
    for (index = 0U; index < remainder; index++)
    {
    	premainder_data[index] = *(Data + length + index);
    }

    /* fill the upper bytes with 0xFF */
    for (index = remainder; index < 8U; index++)
    {
    	premainder_data[index] = 0xFF;
    }
  }

  /* Unlock the flash memory for write operation */
  OPENBL_FLASH_Unlock();

  for (index = 0U; index < length; (index += 8U))
  {
	  OPENBL_FLASH_ProgramDoubleWord((Address + index), *(uint64_t*)((Data + index)));
  }

  if (remainder)
  {
	  OPENBL_FLASH_ProgramDoubleWord((Address + length), remainder_data);
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();
}

//typedef void (*pFunction)(void);
//
//void JumpToApplication(uint32_t Address)
//{
//    pFunction JumpToApp;
//
//    /* Disable all interrupts */
//    __disable_irq();
//
//    /* Stop SysTick */
//    SysTick->CTRL = 0;
//    SysTick->LOAD = 0;
//    SysTick->VAL  = 0;
//
//    /* Deinit bootloader HW */
////    OpenBootloader_DeInit();
////    HAL_RCC_DeInit();
////    HAL_DeInit();
//
//    /* Disable all NVIC interrupts */
//    for (uint32_t i = 0; i < 8; i++)
//    {
//        NVIC->ICER[i] = 0xFFFFFFFF;
//        NVIC->ICPR[i] = 0xFFFFFFFF;
//    }
//
//    /* Set Vector Table to Application */
//    SCB->VTOR = Address;
//
//    /* Set MSP to application's stack pointer */
//    __set_MSP(*(__IO uint32_t *) Address);
//
//    /* Get application's Reset_Handler */
//    JumpToApp = (pFunction)(*(__IO uint32_t *)(Address + 4U));
//
//    /* Enable IRQ (optional, app usually enables itself) */
//    __enable_irq();
//
//    /* Jump */
//    JumpToApp();
//}

/**
  * @brief  This function is used to jump to a given address.
  * @param  Address The address where the function will jump.
  * @retval None.
  */
void OPENBL_FLASH_JumpToAddress(uint32_t Address)
{


	  Function_Pointer jump_to_address;

	  /* Deinitialize all HW resources used by the Bootloader to their reset values */

	    /* Disable all interrupts */
	    __disable_irq();

	    /* Stop SysTick */
	    SysTick->CTRL = 0;
	    SysTick->LOAD = 0;
	    SysTick->VAL  = 0;


		/* Disable all NVIC interrupts */
		for (uint32_t i = 0; i < 8; i++)
		{
			NVIC->ICER[i] = 0xFFFFFFFF;
			NVIC->ICPR[i] = 0xFFFFFFFF;
		}


	    /* Set Vector Table to Application */
	    SCB->VTOR = Address;




	  /* Initialize user application's stack pointer */
	    __set_MSP(*(__IO uint32_t *) Address);
	  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(Address + 4U));

	  __enable_irq();
	  jump_to_address();



}

/**
  * @brief  Return the FLASH Read Protection level.
  * @retval The return value can be one of the following values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  */
uint32_t OPENBL_FLASH_GetReadOutProtectionLevel(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.RDPLevel;
}

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
static void OPENBL_OB_Launch(void)
{
  /* Set the option start bit */
  HAL_FLASH_OB_Launch();

  /* Set the option lock bit and Lock the flash */
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @param  Level Can be one of these values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  * @retval None.
  */
void OPENBL_FLASH_SetReadOutProtectionLevel(uint32_t Level)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  if (Level != OB_RDP_LEVEL2)
  {
    flash_ob.OptionType = OPTIONBYTE_RDP;
    flash_ob.RDPLevel   = Level;

    /* Unlock the FLASH registers & Option Bytes registers access */
    OPENBL_FLASH_OB_Unlock();

    /* Change the RDP level */
    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* Register system reset callback */
  Common_SetPostProcessingCallback(OPENBL_OB_Launch);
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  State Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  ListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *ListOfPages, uint32_t Length)
{
  ErrorStatus status = SUCCESS;

  if (State == ENABLE)
  {
    OPENBL_FLASH_EnableWriteProtection(ListOfPages, Length);

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
  else if (State == DISABLE)
  {
    OPENBL_FLASH_DisableWriteProtection();

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
  else
  {
    status = ERROR;
  }

  return status;
}


/**
  * @brief  This function is used to erase the specified FLASH pages.
  * @param  *p_Data Pointer to the buffer that contains erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or the value of one parameter is not OK
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *p_Data, uint32_t DataLength)
{
  uint32_t counter;
  uint32_t pages_number;
  uint32_t page_error;
  uint32_t errors = 0U;
  ErrorStatus status = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  pages_number  = (uint32_t)(*(uint16_t *)(p_Data));
  p_Data       += 2;

  erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_init_struct.NbPages   = 1U;

  for (counter = 0U; ((counter < pages_number) && (counter < (DataLength / 2U))) ; counter++)
  {
    erase_init_struct.Page = ((uint32_t)(*(uint16_t *)(p_Data)));

    if (erase_init_struct.Page <= 127)
    {
      erase_init_struct.Banks = FLASH_BANK_1;
    }
#if defined (FLASH_OPTR_DBANK)
    else if (erase_init_struct.Page <= 255)
    {
      erase_init_struct.Banks = FLASH_BANK_2;
    }
#endif
    else
    {
      status = ERROR;
    }

    if (status != ERROR)
    {
      if (HAL_FLASHEx_Erase(&erase_init_struct, &page_error) != HAL_OK)
      {
        errors++;
      }
    }
    else
    {
      /* Reset the status for next erase operation */
      status = SUCCESS;
    }

    p_Data += 2;
  }


  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  return status;
}

/**
  * @brief  This function is used to Set Flash busy state variable to activate busy state sending
  *         during flash operations
  * @retval None.
  */
void OPENBL_Enable_BusyState_Flag(void)
{
  /* Enable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_ENABLED;
}

/**
  * @brief  This function is used to disable the send of busy state in I2C non stretch mode.
  * @retval None.
  */
void OPENBL_Disable_BusyState_Flag(void)
{
  /* Disable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  Address specifies the address to be programmed.
  * @param  Data specifies the data to be programmed.
  * @retval None.
  */
static void OPENBL_FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, Data);
}

/**
  * @brief  This function is used to enable write protection of the specified FLASH areas.
  * @param  ListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *ListOfPages, uint32_t Length)
{
  uint8_t wrp_start_offset = 0x7F;
  uint8_t wrp_end_offset   = 0x00;
  ErrorStatus status       = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;
  FLASH_OBProgramInitTypeDef config_old;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  HAL_FLASHEx_OBGetConfig( &config_old );

  flash_ob.OptionType = OPTIONBYTE_WRP;
    flash_ob.USERConfig = config_old.USERConfig;

  /* Write protection of bank 1 area WRPA 1 area */
  if (Length >= 2)
  {
    wrp_start_offset = *(ListOfPages);
    wrp_end_offset   = *(ListOfPages + 1);

    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAA;
    flash_ob.WRPStartOffset = wrp_start_offset;
    flash_ob.WRPEndOffset   = wrp_end_offset;
//    flash_ob.WRPLock        = DISABLE;

    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* Write protection of bank 1 area WRPA 2 area */
  if (Length >= 4)
  {
    wrp_start_offset = *(ListOfPages + 2);
    wrp_end_offset   = *(ListOfPages + 3);

    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
    flash_ob.WRPStartOffset = wrp_start_offset;
    flash_ob.WRPEndOffset   = wrp_end_offset;
//    flash_ob.WRPLock        = DISABLE;

    HAL_FLASHEx_OBProgram(&flash_ob);
  }

//  /* Write protection of bank 2 area WRPB 1 area */
//  if (Length >= 6)
//  {
//    wrp_start_offset = *(ListOfPages + 4);
//    wrp_end_offset   = *(ListOfPages + 5);
//
//    flash_ob.WRPArea        = OB_WRPAREA_BANK2_AREAA;
//    flash_ob.WRPStartOffset = wrp_start_offset;
//    flash_ob.WRPEndOffset   = wrp_end_offset;
//    flash_ob.WRPLock        = DISABLE;
//
//    HAL_FLASHEx_OBProgram(&flash_ob);
//  }
//
//  /* Write protection of bank 2 area WRPB 2 area */
//  if (Length >= 8)
//  {
//    wrp_start_offset = *(ListOfPages + 6);
//    wrp_end_offset   = *(ListOfPages + 7);
//
//    flash_ob.WRPArea        = OB_WRPAREA_BANK2_AREAB;
//    flash_ob.WRPStartOffset = wrp_start_offset;
//    flash_ob.WRPEndOffset   = wrp_end_offset;
//    flash_ob.WRPLock        = DISABLE;
//
//    HAL_FLASHEx_OBProgram(&flash_ob);
//  }

  return status;
}

/**
  * @brief  This function is used to disable write protection.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  uint8_t wrp_start_offset = 0x7F;
  uint8_t wrp_end_offset   = 0x00;
  ErrorStatus status       = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  FLASH_OBProgramInitTypeDef config_old;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();


  HAL_FLASHEx_OBGetConfig( &config_old );

  flash_ob.OptionType = OPTIONBYTE_WRP;

  flash_ob.USERConfig = config_old.USERConfig;

  /* Disable write protection of bank 1 area WRPA A area */
  flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAA;
  flash_ob.WRPStartOffset = wrp_start_offset;
  flash_ob.WRPEndOffset   = wrp_end_offset;
//  flash_ob.WRPLock        = DISABLE;

  HAL_FLASHEx_OBProgram(&flash_ob);

  /* Disable write protection of bank 1 area WRPA B area */
  flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
  flash_ob.WRPStartOffset = wrp_start_offset;
  flash_ob.WRPEndOffset   = wrp_end_offset;
//  flash_ob.WRPLock        = DISABLE;

  HAL_FLASHEx_OBProgram(&flash_ob);

//  /* Disable write protection of bank 2 area WRPB A area */
//  flash_ob.WRPArea        = OB_WRPAREA_BANK2_AREAA;
//  flash_ob.WRPStartOffset = wrp_start_offset;
//  flash_ob.WRPEndOffset   = wrp_end_offset;
//  flash_ob.WRPLock        = DISABLE;
//
//  HAL_FLASHEx_OBProgram(&flash_ob);
//
//  /* Disable write protection of bank 2 area WRPB B area */
//  flash_ob.WRPArea        = OB_WRPAREA_BANK2_AREAB;
//  flash_ob.WRPStartOffset = wrp_start_offset;
//  flash_ob.WRPEndOffset   = wrp_end_offset;
//  flash_ob.WRPLock        = DISABLE;

//  HAL_FLASHEx_OBProgram(&flash_ob);

  return status;
}
