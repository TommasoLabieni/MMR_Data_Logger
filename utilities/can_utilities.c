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

void printf_CAN_frame(__u8* payload, __u8 dlc)
{
    for (int i = 0; i < dlc; i++)
    {
        printf("%d:%.2x | ", i, payload[i]);
    }
    printf("\n");
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

cJSON_msg_50x_t* MMR_CAN_frame_to_EGRESS_frame(cJSON_msg_50x_t* msg_50x, canid_t msg_id)
{
    for (int i=0; i < NUM_CAN_FRAMES_TO_EGRESS; i++)
    {
        for (int j=0; j < msg_50x[i].msg_info.length; j++)
        {
            // printf("i: %d\tj: %d\tindex: %d\n", i, j, index);
            if (msg_id == msg_50x[i].msg_info.msg_info_ptr[j].msg_MMR_ID->valueint)
            {
                return &(msg_50x[i]);
            }
        }
    }

    return NULL;
}

cJSON_msg_info_t* MMR_CAN_frame_to_EGRESS_frame_info(cJSON_msg_50x_t* msg_50x, canid_t msg_id)
{
    for (int i=0; i < msg_50x->msg_info.length; i++)
    {
        // printf("ID: %d\ti: %d\t", msg_id, i);
        // printf("CHECKING: %s\n", msg_50x->msg_info.msg_info_desc[i]);
        if (msg_id == msg_50x->msg_info.msg_info_ptr[i].msg_MMR_ID->valueint)
        {
            return &(msg_50x->msg_info.msg_info_ptr[i]);
        }
    }

    return NULL;
}

__u8 get_msg_bits_n(cJSON_msg_info_t* msg)
{
    return (__u8) ((msg->msg_PBE -  msg->msg_PBS) + 1);
}

int insert_MMR_CAN_frame_into_EGRESS_frame(struct can_frame recv_frame, can_msg* msg_50x, __u8 msg_dlc, cJSON_msg_info_t* msg_50x_info)
{
    __u8* mask, *output_msg;
    mask = malloc(msg_dlc * sizeof(char));
    output_msg = calloc(msg_dlc, msg_dlc * sizeof(char));
    printf("DLC IS: %u\n", msg_dlc);

    const char default_mask[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    switch (msg_dlc)
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

    // printf("MASK IS:\t\t");
    // printf_CAN_frame(mask, msg_dlc);

    if (msg_50x_info == NULL)
    {
        perror("ERROR");
        return -1;
    }

    /* Unset mask bits corresponding to msg reserved bits */
    for (size_t i = msg_50x_info->msg_PBS->valueint; i <= msg_50x_info->msg_PBE->valueint; i++)
    {
        // printf("unsetting bit %lu-th\n", i);
        unset_CAN_k_th_bit(mask, i);
    }

    // printf("NEW MASK IS:\t\t");
    // printf_CAN_frame(mask, msg_dlc);

    /* Get length of the msg to egress in bytes */
    __u8 n_bytes = bit_to_byte(msg_50x_info->msg_PBE->valueint - msg_50x_info->msg_PBS->valueint + 1);

    short format_data;

    if (msg_50x_info->msg_IS_ECU_MESSAGE->valueint)
    {
        printf("ECU MESSAGE\n");
        __u8 byte_start = msg_50x_info->msg_ECU_PAYLOAD->ecu_payload_BYTE_START->valueint;
        __u8 len = msg_50x_info->msg_ECU_PAYLOAD->ecu_payload_BYTE_LENGTH->valueint;
        for (int i=0; i < len; i++)
        {
            // printf("Byte Index: %u\n READ %x\n", byte_start+i, recv_frame.data[byte_start+len-i-1]);
            format_data <<= 8;
            format_data |= recv_frame.data[byte_start+len-i-1];
        }

        format_data /= (short) msg_50x_info->msg_ECU_PAYLOAD->ecu_payload_DIVIDE_BY->valueint;

    }

    /* Convert received frame to correct data format */
    if (is_float(msg_50x_info->msg_DATA_TYPE->valuestring))
    {
        float f_value = byte_array_to_float(recv_frame.data);
        printf("CONVERSION FROM %f TO %u\n", f_value, n_bytes);
        if (n_bytes == 2)
            format_data = float_to_short(f_value);

        else
            format_data = float_to_byte(f_value);
    }
    printf("FORMAT: %d\n", format_data);

    /* Percentage msg handler */
    if (msg_50x_info->msg_IS_PERCENTAGE->valueint)
    {
        format_data *= 100 / msg_50x_info->msg_MAX_PERC_VALUE->valueint;
    }

    /* Scale MSG */
    format_data *= msg_50x_info->msg_SCALE->valueint;

    printf("SCALED FORMAT: %d\n", format_data);

    /* Get first byte index in which data have to be stored */
    __u8 byte_index = bit_to_byte(msg_50x_info->msg_PBS->valueint);

    // printf("INDEX: %u\n", byte_index);

    int i=0;
    do
    {
        /* Copy CAN frame data */
        output_msg[byte_index+i] |= format_data; //todo: add shift
        ++i;
    } while (i < n_bytes);

    /* Shift output msg of PBS bits IF AND ONLY IF the msg is not a FULL BYTE LONG !!! */
    if (bit_to_byte(msg_50x_info->msg_PBE->valueint - msg_50x_info->msg_PBS->valueint + 1) == 0)
    {
        // printf("Shifting Message\n");
        shift_array_left(output_msg, byte_index, (msg_50x_info->msg_PBS->valueint % 8));
    }

    // printf_CAN_frame(output_msg, msg_dlc);

    /* Message created, now we have to replace old msg data with new one */

    /* First thing first, we have to make a bit-to-bit end between old_msg and MASK */
    bit_to_bit_AND_rray(msg_50x->frame.data, mask, msg_dlc);

    /* Then, just make an bit-to-bit or between masked msg and new data */
    bit_to_bit_OR_rray(msg_50x->frame.data, output_msg, msg_dlc);

    printf_CAN_frame(msg_50x->frame.data, msg_dlc);

    return 0;
}