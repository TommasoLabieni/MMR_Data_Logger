#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <basic_utils.h>
#include <json_utilities.h>
#include <byte_conversion.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define DEVICE_INTERFACE "vcan0"

/* BCM Type definition */
typedef struct
{
        struct bcm_msg_head msg_head;
        struct can_frame frame;
} can_msg;

typedef struct {
        canid_t can_id;
        canid_t can_mask;
} can_filter;

void create_egress_BCM(can_msg* msg, canid_t can_id, __u8 dlc);
void create_sniffer_filters(struct can_filter* filters, const cJSON_msg_50x_t* msg);
char* get_MMR_CAN_frame_description(cJSON_msg_50x_t* msg_50x, canid_t msg_id);
unsigned int get_MMR_CAN_frame_info_index(cJSON_msg_50x_t* msg_50x, canid_t msg_id);
canid_t MMR_CAN_frame_to_EGRESS_frame(cJSON_msg_50x_t* msg_50x, canid_t msg_id);
int insert_MMR_CAN_frame_into_EGRESS_frame(struct can_frame recv_frame, cJSON_msg_50x_t* msg_50x);
#endif //DATA_LOGGER_H