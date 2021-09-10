#include "can.h"
#include "Common_Macros.h"
#include "tm4c123gh6pm.h"



void CAN_init (unsigned long base_address)
{
  int i;
  SET_BIT(CAN0_CTL_R + base_address , CAN_CTL_INIT)
  
  while(BIT_IS_SET(CAN0_IF1CRQ_R + base_address , CAN_IF1CRQ_BUSY))
  {
  }

  BIT_IS_SET(CAN0_IF1CMSK_R + base_address, CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_ARB | CAN_IF1CMSK_CONTROL);
  CAN0_IF1ARB2_R + base_address = 0;
  for (i = 1; i <= 32; i++)
  {
	  while (BIT_IS_SET(CAN0_IF1CRQ_R + base_address, CAN_IF1CRQ_BUSY))
	  {
	  }
	  SET_BIT(CAN0_IF1CRQ_R , i)
  }



}

void CAN_BitTimming_Set(unsigned long base_address, CANBitParmters *Parms)
{
	unsigned int BitReg;
	unsigned int Old_Init;

	Old_Init = CAN0_CTL_R + base_address;
	SET_BIT(CAN0_CTL_R + base_address, CAN_CTL_INIT);
	SET_BIT(CAN0_CTL_R + base_address, CAN_CTL_CCE);

	
	BitReg  = ((Parms->Phase2_Seg - 1) << 12) & CAN_BIT_TSEG2_M;
	BitReg |= ((Parms->Phase1_Seg - 1) << 8) & CAN_BIT_TSEG1_M;
	BitReg |= ((Parms->SJW - 1) << 6) & CAN_BIT_SJW_M;
	BitReg |= (Parms->QuantumPrescaler - 1) & CAN_BIT_BRP_M;
	SET_BIT(CAN0_BIT_R + base_address, BitReg);

	CLEAR_BIT(CAN0_CTL_R + base_address, CAN_CTL_CCE);
	CLEAR_BIT(CAN0_CTL_R + base_address, CAN_CTL_INIT);
	SET_BIT(CAN0_CTL_R + base_address, Old_Init);
}























