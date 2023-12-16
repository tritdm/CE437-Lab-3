#include "can.h"

volatile uint8_t CANDataRcvFlag = 0;
uint8_t CANRxBuffer[CAN_DATA_LENGTH];
CAN_RxHeaderTypeDef CANRxHeader;

uint8_t CANTxBuffer[] = {0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t CANTxMailboxes = CAN_TX_MAILBOX0;
CAN_TxHeaderTypeDef CANTxHeader;

int SAE_J1850_Calc(uint8_t data[], int len)
{
	int crc, temp1, temp2;
	crc 	= 0;
	temp1 	= 0;
	temp2 	= 0;
	for (int _idx = 0; _idx < len; ++ _idx)
	{
		if (0 == _idx)
		{
			temp1 = 0;
		}
		else
		{
			temp1 = data[len - _idx];
		}
		crc = crc ^ temp1;
		for (int _idy = 8; _idy > 0; -- _idy)
		{
			temp2 = crc;
			crc = crc << 1;
			if (0 != (temp2 & 128))
			{
				crc = crc ^ 0x1d;
			}
		}
	}
	return crc;
}

void genMessage()
{
	CANTxHeader.StdId 	= CAN_TX_STD_ID;
	CANTxHeader.IDE 	= CAN_ID_STD;
	CANTxHeader.RTR 	= CAN_RTR_DATA;
	CANTxHeader.DLC 	= CAN_DATA_LENGTH;

	CANTxBuffer[0] 		= rand() % 16;
	CANTxBuffer[1] 		= rand() % 16;
	CANTxBuffer[6] 		= (CANTxBuffer[6] + 1)%15;
}

void CAN_Transmit(CAN_HandleTypeDef *hcan)
{
	if (HAL_CAN_AddTxMessage(hcan, &CANTxHeader, CANTxBuffer, &CANTxMailboxes) == HAL_OK)
	{
		HAL_GPIO_TogglePin(GPIO_Port, LEDR_Pin);
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &CANRxHeader, CANRxBuffer) != HAL_OK)
	{
		Error_Handler();
	}

	if (CANRxHeader.StdId == CAN_RX_STD_ID)
	{
		CANDataRcvFlag = 1;
		HAL_GPIO_TogglePin(GPIO_Port, LEDG_Pin);
	}
}
