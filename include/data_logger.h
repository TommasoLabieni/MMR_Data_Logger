#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>

#define DEVICE_INTERFACE "vcan0"
#define MSG_500_DLC 8
#define MSG_501_DLC 6
#define MSG_502_DLC 5

typedef __s32 err_t;
typedef __s8 error_msg[255];

/* BCM Type definition */
typedef struct
{
		struct bcm_msg_head msg_head;
		struct can_frame frame;
} can_msg;

void create_frame(can_msg* msg, canid_t can_id, __u8 dlc);