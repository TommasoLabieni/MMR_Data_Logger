#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <basic_utils.h>
#include <json_utilities.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>

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
char* get_MMR_CAN_frame_description(cJSON_msg_50x_t* msg_50x, unsigned int msg_id);
#endif //DATA_LOGGER_H