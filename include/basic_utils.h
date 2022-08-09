#ifndef BASIC_UTILS_H
#define BASIC_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>

typedef int err_t;
typedef char error_msg[255];

/* ********** CONSTANT VALUES DECLARATION ********** */

#define ERR_MSG_ID 0x1000

#define NUM_CAN_FRAMES_TO_EGRESS 3

/* These constants define the IDs of the CAN frames to be egressed */
#define MSG_500_ID 0x500
#define MSG_501_ID 0x501
#define MSG_502_ID 0x502

/* Theses constants define the length of the payload in Bytes */
#define MSG_500_DLC 8
#define MSG_501_DLC 6
#define MSG_502_DLC 5

/* Theses constants define the number of MMR_msgs that each CAN 50x frame contains */
#define MSG_500_N_MMR_MSGS 8
#define MSG_501_N_MMR_MSGS 3
#define MSG_502_N_MMR_MSGS 8

/* N_MMR_MSGS_FILTERED should contains the TOTAL number of MMR CAN frames to filter */
#define N_MMR_MSGS_FILTERED MSG_500_N_MMR_MSGS + MSG_501_N_MMR_MSGS + MSG_502_N_MMR_MSGS

#define JSON_FILENAME "MMR_CAN_msg_id.json"

/* ************************************************* */

#endif //BASIC_UTILS_H