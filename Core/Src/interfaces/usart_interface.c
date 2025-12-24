/**
  ******************************************************************************
  * @file    usart_interface.c
  * @author  MCD Application Team
  * @brief   Contains USART HW configuration
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
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
#include "interfaces_conf.h"
#include "openbl_core.h"
#include "openbl_usart_cmd.h"
#include "usart_interface.h"
//#include "iwdg_interface.h"

extern UART_HandleTypeDef huart2;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEFAULT_USART_BAUDRATE   115200U

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//static uint8_t UsartDetected = 0U;

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//static void OPENBL_USART_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function is used to initialize the used USART instance.
  * @retval None.
  */
//static void OPENBL_USART_Init(void)
//{
//
//}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to configure USART pins and then initialize the used USART instance.
  * @retval None.
  */
void OPENBL_USART_Configuration(void)
{

}

/**
  * @brief  This function is used to De-initialize the USART pins and instance.
  * @retval None.
  */
void OPENBL_USART_DeInit(void)
{

}

/**
  * @brief  This function is used to detect if there is any activity on USART protocol.
  * @retval Returns 1 if interface is detected else 0.
  */
uint8_t OPENBL_USART_ProtocolDetection(void)
{
  /* Check if the USARTx is addressed */
//  if (((USARTx->ISR & LL_USART_ISR_ABRF) != 0) && ((USARTx->ISR & LL_USART_ISR_ABRE) == 0))
//  {
//    /* Read byte in order to flush the 0x7F synchronization byte */
//    OPENBL_USART_ReadByte();
//
//    /* Acknowledge the host */
//    OPENBL_USART_SendByte(ACK_BYTE);
//
//    UsartDetected = 1U;
//  }
//  else
//  {
//    UsartDetected = 0U;
//  }
//
//  return UsartDetected;

	OPENBL_USART_ReadByte();
	 OPENBL_USART_SendByte(ACK_BYTE);

  return 1;
}

/**
  * @brief  This function is used to get the command opcode from the host.
  * @retval Returns the command.
  */
uint8_t OPENBL_USART_GetCommandOpcode(void)
{
  uint8_t command_opc = 0x0;

  /* Get the command opcode */
  command_opc = OPENBL_USART_ReadByte();

  /* Check the data integrity */
  if ((command_opc ^ OPENBL_USART_ReadByte()) != 0xFF)
  {
    command_opc = ERROR_COMMAND;
  }

  return command_opc;
}

/**
  * @brief  This function is used to read one byte from USART pipe.
  * @retval Returns the read byte.
  */
uint8_t OPENBL_USART_ReadByte(void)
{
//  while (!LL_USART_IsActiveFlag_RXNE_RXFNE(USARTx))
//  {
////    OPENBL_IWDG_Refresh();
//  }
//
//  return LL_USART_ReceiveData8(USARTx);

  uint8_t c;
  while(HAL_UART_Receive(&huart2, &c, 1, HAL_MAX_DELAY) != HAL_OK) {}
  return c;
}

/**
  * @brief  This function is used to send one byte through USART pipe.
  * @param  Byte The byte to be sent.
  * @retval None.
  */
void OPENBL_USART_SendByte(uint8_t Byte)
{
//  LL_USART_TransmitData8(USARTx, (Byte & 0xFF));
//
//  while (!LL_USART_IsActiveFlag_TC(USARTx))
//  {
//  }

  HAL_UART_Transmit(&huart2, &Byte, 1, HAL_MAX_DELAY);
}

/**
  * @brief  This function is used to process and execute the special commands.
  *         The user must define the special commands routine here.
  * @param  SpecialCmd Pointer to the OPENBL_SpecialCmdTypeDef structure.
  * @retval Returns NACK status in case of error else returns ACK status.
  */
void OPENBL_USART_SpecialCommandProcess(OPENBL_SpecialCmdTypeDef *SpecialCmd)
{
  switch (SpecialCmd->OpCode)
  {
    /* Unknown command opcode */
    default:
      if (SpecialCmd->CmdType == OPENBL_SPECIAL_CMD)
      {
        /* Send NULL data size */
        OPENBL_USART_SendByte(0x00U);
        OPENBL_USART_SendByte(0x00U);

        /* Send NULL status size */
        OPENBL_USART_SendByte(0x00U);
        OPENBL_USART_SendByte(0x00U);
      }
      else if (SpecialCmd->CmdType == OPENBL_EXTENDED_SPECIAL_CMD)
      {
        /* Send NULL status size */
        OPENBL_USART_SendByte(0x00U);
        OPENBL_USART_SendByte(0x00U);
      }
      break;
  }
}
