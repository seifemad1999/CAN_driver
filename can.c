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
	  SET_BIT(CAN0_IF1CRQ_R + base_address, i)
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

void CANMessageSet(unsigned long base_address, unsigned long ObjID,
	CANMsgObject *pMsgObject, MsgObjType MsgType)
{
	unsigned short CmdMaskReg;
	unsigned short MaskReg1 = 0, MaskReg2 = 0;
	unsigned short ArbReg1 = 0, ArbReg2 = 0;
	unsigned short MsgCtrl = 0;
	char TransferData = 0;
	char useExtendedID;

	while (BIT_IS_SET(CAN0_IF1CRQ_R + base_address, CAN_IF1CRQ_BUSY))
	{
	}

	if ((pMsgObject->MsgID > 0x7FF) || (pMsgObject->MsgID & MSG_OBJ_EXTENDED_ID))
	{
		useExtendedID = 1;
	}
	else
	{
		useExtendedID = 0;
	}

	usCmdMaskReg = CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB | CAN_IF1CMSK_CONTROL;

	switch (MsgType)
	{
        // Transmit message object.
        case MSG_OBJ_TYPE_TX:
        {
            // Set the TXRQST bit and reset the rest of the register.
            MsgCtrl |= CAN_IF1MCTL_TXRQST;
            ArbReg2 = CAN_IF1ARB2_DIR;
            TransferData = 1;
            break;
        }

        // Transmit remote request message object
        case MSG_OBJ_TYPE_TX_REMOTE:
        {
            // Set the TXRQST bit and reset the rest of the register.
            MsgCtrl |= CAN_IF1MCTL_TXRQST;
            ArbReg2 = 0;
            break;
        }

        // Receive message object.
        case MSG_OBJ_TYPE_RX:
        {
            // This clears the DIR bit along with everything else The TXRQST
            // bit was cleared by defaulting MsgCtrl to 0.
            ArbReg2 = 0;
            break;
        }

        // Receive remote request message object.
        case MSG_OBJ_TYPE_RX_REMOTE:
        {
            ArbReg2 = CAN_IF1ARB2_DIR;
			// Enable Acceptance Mask
			MsgCtrl = CAN_IF1MCTL_UMASK;
            // Make the full Identifier by default
            MaskReg1 = 0xffff;
            MaskReg2 = 0x1fff;
            // Send  mask to the message object.
            CmdMaskReg |= CAN_IF1CMSK_MASK;
            break;
        }

        default:
        {
            return;
        }
	}

	// Enable transmit interrupts if they should be enabled
	if (pMsgObject->Flags & MSG_OBJ_TX_INT_ENABLE)
	{
		MsgCtrl |= CAN_IF1MCTL_TXIE;
	}

	// Enable receive interrupts if they should be enabled
	if (pMsgObject->Flags & MSG_OBJ_RX_INT_ENABLE)
	{
		MsgCtrl |= CAN_IF1MCTL_RXIE;
	}

	// Configure the Mask Registers
	if (pMsgObject->Flags & MSG_OBJ_USE_ID_FILTER)
	{
		if (UseExtendedID)
		{
			// Set the 29 bits
			MaskReg1 = pMsgObject->MsgIDMask & CAN_IF1MSK1_IDMSK_M;
			MaskReg2 = ((pMsgObject->MsgIDMask >> 16) & CAN_IF1MSK2_IDMSK_M);
			MsgCtrl |= CAN_IF1MCTL_UMASK;
			CmdMaskReg |= CAN_IF1CMSK_MASK;
			CmdMaskReg |= CAN_IF1CMSK_ARB;
		}
		else
		{
			// Set the 29 bits
			usMaskReg1 = 0;
			usMaskReg2 = ((pMsgObject->MsgIDMask << 2) & CAN_IF1MSK2_IDMSK_M);
		}
	}

	// Set extended ID bit then in MASK reg 2
	if ((pMsgObject->Flags & MSG_OBJ_USE_EXT_FILTER) == MSG_OBJ_USE_EXT_FILTER)
	{
		MaskReg1 |= CAN_IF1MSK2_MXTD;
		MsgCtrl |= CAN_IF1MCTL_UMASK;
		CmdMaskReg |= CAN_IF1CMSK_MASK;
		CmdMaskReg |= CAN_IF1CMSK_ARB;
	}

	// filter on the message direction field
	if ((pMsgObject->ulFlags & MSG_OBJ_USE_DIR_FILTER) == MSG_OBJ_USE_DIR_FILTER)
	{
		MaskReg1 |= CAN_IF1MSK2_MDIR;
		MsgCtrl |= CAN_IF1MCTL_UMASK;
		CmdMaskReg |= CAN_IF1CMSK_MASK;
		CmdMaskReg |= CAN_IF1CMSK_ARB;
	}

	// Set the Arb bit 
	CmdMaskReg |= CAN_IF1CMSK_ARB;

	// Configure the Arbitration registers
	if (UseExtendedID)
	{
		// Set the 29 bit version of the Identifier for this message object
		ArbReg1 |= pMsgObject->MsgID & CAN_IF1ARB1_ID_M;
		ArbReg2 |= (pMsgObject->MsgID >> 16) & CAN_IF1ARB2_ID_M;
		// Mark the message as valid and set the extended ID bit.
		ArbReg2 |= CAN_IF1ARB2_MSGVAL | CAN_IF1ARB2_XTD;
	}
	else
	{
		// Set the 11 bit version of the Identifier for this message object
		// The lower 18 bits are set to zero
		ArbReg2 |= (pMsgObject->MsgID << 2) & CAN_IF1ARB2_ID_M;
		// Mark the message as valid
		ArbReg2 |= CAN_IF1ARB2_MSGVAL;
	}

	// Set the data length 
	MsgCtrl |= (pMsgObject->MsgLen & CAN_IF1MCTL_DLC_M);

	// Write the registers
	SET_BIT(CAN0_IF1CMSK_R + base_address, CmdMaskReg);
	SET_BIT(CAN0_IF1MSK1_R + base_address, MaskReg0);
	SET_BIT(CAN0_IF1MSK2_R + base_address, MaskReg1);
	SET_BIT(CAN0_IF1ARB1_R + base_address, ArbReg0);
	SET_BIT(CAN0_IF1ARB2_R + base_address, ArbReg1);
	SET_BIT(CAN0_IF1MCTL_R + base_address, MsgCtrl);

	// Transfer the message object to the message object specifiec by ulObjID
	SET_BIT(CAN0_IF1CRQ_R + base_address, ObjID & CAN_IF1CRQ_MNUM_M);

	return;
}

unsigned long CANStatusGet(unsigned long base_address, CANStsReg StatusReg)
{
	unsigned long Status;
	switch (StatusReg)
	{
		// Just return the global CAN status register since that is what was requested
	case CAN_STS_CONTROL:
	{
		Status = *(CAN0_STS_R + base_address);
		SET_BIT(CAN0_STS_R + base_address, ~(CAN_STS_RXOK | CAN_STS_TXOK | CAN_STS_LEC_M));
		break;
	}

	// Combine the New Data status bits into one 32bit value.
	case CAN_STS_NEWDAT:
	{
		Status = *(CAN0_NWDA1_R + base_address);
		Status |= *(CAN0_NWDA2_R + base_address) << 16;
		break;
	}

	// Combine the Message valid status bits into one 32bit value.
	case CAN_STS_MSGVAL:
	{
		Status  = *(CAN0_MSG1VAL_R + base_address);
		Status |= *(CAN0_MSG2VAL_R + base_address) << 16;
		break;
	}

	// Unknown CAN status requested so return 0
	default:
	{
		Status = 0;
		break;
	}
	}
	return(Status);
}

void CANEnable(unsigned long base_address)
{	
	// Clear the init bit in the control register.
	CLEAR_BIT(CAN0_CTL_R + base_address, CAN_CTL_INIT);
}

void CANDisable(unsigned long base_address)
{
	// Set the init bit in the control register
	SET_BIT(CAN0_CTL_R + base_address, CAN_CTL_INIT);
}















