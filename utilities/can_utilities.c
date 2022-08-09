#include <data_logger.h>

void create_egress_BCM(can_msg *msg, canid_t can_id, __u8 dlc)
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

void create_sniffer_filters(struct can_filter* filters)
{
    filters[0].can_id   = 0x124;
	filters[0].can_mask = CAN_SFF_MASK;
	filters[1].can_id   = 0x200;
	filters[1].can_mask = CAN_SFF_MASK;
}