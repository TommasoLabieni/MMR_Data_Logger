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

void create_sniffer_filters(struct can_filter* filters, const cJSON_msg_50x_t* msg)
{
    int index = 0;

    for (int i=0; i < NUM_CAN_FRAMES_TO_EGRESS; i++)
    {
        for (int j=0; j < msg[i].msg_info.length; j++)
        {
            // printf("i: %d\tj: %d\tindex: %d\n", i, j, index);
            filters[index].can_id   = msg[i].msg_info.msg_info_ptr[j].msg_MMR_ID->valueint;
            printf("CAN frame with id: %.3x added to filter list\n", msg[i].msg_info.msg_info_ptr[j].msg_MMR_ID->valueint);
            filters[index].can_mask = CAN_SFF_MASK;
            index++;
        }
    }
}

char* get_MMR_CAN_frame_descripion(cJSON_msg_50x_t* msg_50x, unsigned int msg_id)
{
    for (int i=0; i < NUM_CAN_FRAMES_TO_EGRESS; i++)
    {
        for (int j=0; j < msg_50x[i].msg_info.length; j++)
        {
            // printf("i: %d\tj: %d\tindex: %d\n", i, j, index);
            if (msg_id == msg_50x[i].msg_info.msg_info_ptr[j].msg_MMR_ID->valueint)
            {
                return msg_50x[i].msg_info.msg_info_desc[j];
            }
        }
    }

    return NULL;
}