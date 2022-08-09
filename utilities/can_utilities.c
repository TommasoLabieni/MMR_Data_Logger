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

char* get_MMR_CAN_frame_description(cJSON_msg_50x_t* msg_50x, canid_t msg_id)
{
    for (unsigned int i=0; i < NUM_CAN_FRAMES_TO_EGRESS; i++)
    {
        for (unsigned int j=0; j < msg_50x[i].msg_info.length; j++)
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

unsigned int get_MMR_CAN_frame_info_index(cJSON_msg_50x_t* msg_50x, canid_t msg_id)
{
    for (unsigned int i=0; i < msg_50x->msg_info.length; i++)
    {
        if (msg_id == msg_50x->msg_info.msg_info_ptr[i].msg_MMR_ID->valueint)
        {
            return i;
        }
    }

    return __INT32_MAX__;
}

canid_t MMR_CAN_frame_to_EGRESS_frame(cJSON_msg_50x_t* msg_50x, canid_t msg_id)
{
    for (int i=0; i < NUM_CAN_FRAMES_TO_EGRESS; i++)
    {
        for (int j=0; j < msg_50x[i].msg_info.length; j++)
        {
            // printf("i: %d\tj: %d\tindex: %d\n", i, j, index);
            if (msg_id == msg_50x[i].msg_info.msg_info_ptr[j].msg_MMR_ID->valueint)
            {
                return msg_50x[i].msg_id;
            }
        }
    }

    return ERR_MSG_ID;
}

/* Function that set k-th CAN frame payload bit to 0 */ 
void  unset_CAN_k_th_bit(__u8 v[],  __u8 k )
{
    v[k/8] &= ~(1 << (k%8));
}
/* Function that set k-th CAN frame payload bit to 1 */ 
void  set_CAN_k_th_bit( __u8 v[],  __u8 k )
{
    v[k/8] |= 1 << (k%8); 
}

int insert_MMR_CAN_frame_into_EGRESS_frame(struct can_frame recv_frame, cJSON_msg_50x_t* msg_50x)
{
    __u8* mask;
    mask = malloc(msg_50x->msg_dlc * sizeof(char));
    // printf("DLC IS: %u\n", msg_50x->msg_dlc);

    const char default_mask[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    switch (msg_50x->msg_dlc)
    {
    case 8:
        mask = (char *) memcpy(mask, default_mask, 8);
        break;
    case 6:
        mask = (char *) memcpy(mask, default_mask, 6);
        break;
    case 5:
        mask = (char *) memcpy(mask, default_mask, 5);
        break;
    default:
        perror("Wrong msg DLC");
        return -1;
    }

    printf("MASK IS: "); 
    for (int i = 0; i < msg_50x->msg_dlc; i++)
    {
        printf("%x", (unsigned int)mask[i]);
    }
    printf("\n");

    /* Get msg_info_index */
    unsigned int msg_info_index = get_MMR_CAN_frame_info_index(msg_50x, recv_frame.can_id & CAN_SFF_MASK);
    if (msg_info_index == __INT32_MAX__)
    {
        perror("EXTERNAL MSG INFO");
        return -1;
    }

    /* Unset mask bits correspondig to msg reserved bits */
    for (size_t i = msg_50x->msg_info.msg_info_ptr[msg_info_index].msg_PBS->valueint; i <= msg_50x->msg_info.msg_info_ptr[msg_info_index].msg_PBE->valueint; i++)
    {
        // printf("unsetting bit %lu-th\n", i);
        unset_CAN_k_th_bit(mask, i);
    }

    printf("NEW MASK IS: "); 
    for (int i = 0; i < msg_50x->msg_dlc; i++)
    {
        printf("%.2x | ", (unsigned int)mask[i]);
    }
    printf("\n");

    /* TODO: ASSIGN DATA */

    return 0;
}