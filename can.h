#ifndef __CAN_H__
#define __CAN_H__



//
//typedef enum{
//  CAN0,
//  CAN1 = 0x1000
//  
//}CAN_module;

#define CAN0 0
#define CAN1 0x1000



//*****************************************************************************
// These are the flags used by the CANMsgObject.Flags value when calling the
//*****************************************************************************

// This indicates that a message object has no flags set.
#define MSG_OBJ_NO_FLAGS        0x00000000

// This indicates that transmit interrupts should be enabled, or are enabled.
#define MSG_OBJ_TX_INT_ENABLE   0x00000001

// This indicates that receive interrupts should be enabled, or are enabled.
#define MSG_OBJ_RX_INT_ENABLE   0x00000002

// This indicates that a message object will use or is using an extended identifier
#define MSG_OBJ_EXTENDED_ID     0x00000004

// This indicates that a message object will use or is using filtering
// based on the object's message identifier.
#define MSG_OBJ_USE_ID_FILTER   0x00000008

// This indicates that new data was available in the message object.
#define MSG_OBJ_NEW_DATA        0x00000080

// This indicates that data was lost since this message object was last read
#define MSG_OBJ_DATA_LOST       0x00000100

// This indicates that a message object will use or is using filtering
// based on the direction of the transfer.  If the direction filtering is
// used, then ID filtering must also be enabled.
#define MSG_OBJ_USE_DIR_FILTER  (0x00000010 | MSG_OBJ_USE_ID_FILTER)

// This indicates that a message object will use or is using message
// identifier filtering based on the extended identifier.  If the extended
// identifier filtering is used, then ID filtering must also be enabled.
#define MSG_OBJ_USE_EXT_FILTER  (0x00000020 | MSG_OBJ_USE_ID_FILTER)

// This indicates that a message object is a remote frame.
#define MSG_OBJ_REMOTE_FRAME    0x00000040





//*****************************************************************************
//
// This structure is used for encapsulating the values associated with setting
// up the bit timing for a CAN controller.  The structure is used when calling
// CAN_BitTimming_Set function
//
//*****************************************************************************
typedef struct
{
	unsigned int Phase1_Seg;
	unsigned int Phase2_Seg;
	unsigned int SJW;
	unsigned int QuantumPrescaler;
}
CANBitParmters;


//*****************************************************************************
// The structure used for encapsulating all the items associated with a CAN
// message object in the CAN controller.
//*****************************************************************************
typedef struct
{
	
	// The CAN message identifier used for 11 or 29 bit identifiers.
	
	unsigned long MsgID;

	
	// The message identifier mask used when identifier filtering is enabled.
	
	unsigned long MsgIDMask;

	
	// This value holds various status flags and settings specified by tCANObjFlags.
	
	unsigned long Flags;

	
	// This value is the number of bytes of data in the message object.
	
	unsigned long MsgLength;

	
	// This is a pointer to the message object's data.
	
	unsigned char *ptr_MsgData;
}
CANMsgObject;

//*****************************************************************************
// This definition is used to determine the type of message object that will
// be set up via a call to the CANMessageSet()
//*****************************************************************************
typedef enum
{
	
	// Transmit message object.
	
	MSG_OBJ_TYPE_TX,

	
	// Transmit remote request message object
	
	MSG_OBJ_TYPE_TX_REMOTE,

	
	// Receive message object.
	
	MSG_OBJ_TYPE_RX,

	
	// Receive remote request message object.
	
	MSG_OBJ_TYPE_RX_REMOTE,

	
	// Remote frame receive remote, with auto-transmit message object.
	
	MSG_OBJ_TYPE_RXTX_REMOTE
}
MsgObjType;


// enum for can status
typedef enum
{
	// Read the full CAN controller status.
	CAN_STS_CONTROL,

	// Read the full 32-bit mask of message objects with a transmit request
	CAN_STS_TXREQUEST,

	// Read the full 32-bit mask of message objects with new data available.
	CAN_STS_NEWDAT,

	// Read the full 32-bit mask of message objects that are enabled.
	CAN_STS_MSGVAL
}
CANStsReg;


//*****************************************************************************
//
//  Functions prototypes
//
//*****************************************************************************

void CAN_init (unsigned long base_address);
void CAN_BitTimming_Set(unsigned long base_address, CANBitParmters *Parms);
void CANMessageSet(unsigned long base_address, unsigned long ObjID, CANMsgObject *pMsgObject, MsgObjType MsgType);
unsigned long CANStatusGet(unsigned long base_address, CANStsReg StatusReg);
void CANEnable(unsigned long base_address);
void CANDisable(unsigned long base_address);






















#endif