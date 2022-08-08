#include <data_logger.h>

void create_frame(can_msg *msg, canid_t can_id, __u8 dlc)
{
    msg->msg_head.opcode  = TX_SETUP;
    msg->msg_head.can_id  = can_id;
    msg->msg_head.flags   = SETTIMER|STARTTIMER|TX_CP_CAN_ID;
    msg->msg_head.nframes = 1;
    msg->msg_head.count = 0;
    msg->msg_head.ival1.tv_sec = 0;
    msg->msg_head.ival1.tv_usec = 0;
    msg->msg_head.ival2.tv_sec = 0;
    msg->msg_head.ival2.tv_usec = 100000;
    msg->frame.can_dlc=dlc;
}