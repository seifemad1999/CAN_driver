#ifndef __CAN_H__
#define __CAN_H__


/*****************************************************************************
/
/ This data type is used to identify the which CAN Module used. This is
/ used when calling the CANInit() function.
/
/*****************************************************************************/

//
//typedef enum{
//  CAN0,
//  CAN1 = 0x1000
//  
//}CAN_module;

#define CAN0 0
#define CAN1 0x1000


typedef struct
{
	unsigned int Phase1_Seg;
	unsigned int Phase2_Seg;
	unsigned int SJW;
	unsigned int QuantumPrescaler;
}
CANBitParmters;


void CAN_init (unsigned long base_address);


























#endif