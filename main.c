#include <data_logger.h>
#include <json_utilities.h>

int main(int argc, char **argv)
{
	/* ********** VARS DECLARATION ********** */

	__s32 sock_send, sock_recv;
	struct sockaddr_can addr;
	struct ifreq ifr;
	err_t ret;
	error_msg error;
	__u32 nbytes;
	can_msg msg_500, msg_501, msg_502;
	can_msg sniffer;
	struct can_filter rfilter[N_MMR_MSGS_FILTERED];
	cJSON_msg_50x_t* msg_50x_sniffer;

	/* ************************************** */


	/* ********** CONNECTION TO CAN NETWORK ********** */

	/* Socket for Egress Frames Creation */
	sock_send = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);

	/* Socket for Received Frames Creation */
	sock_recv = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	printf("Connecting to: %s interface\n", DEVICE_INTERFACE);

	/* Connection to CAN with interface = DEVICE_INTERFACE */
	strcpy(ifr.ifr_name, DEVICE_INTERFACE);
	ret = (ioctl(sock_send, SIOCGIFINDEX, &ifr) || ioctl(sock_recv, SIOCGIFINDEX, &ifr));
	
	if (ret)
	{
		sprintf(error, "Error during connection to CAN Bus interface: %s. Error code is: %d\n", DEVICE_INTERFACE, ret);
		perror(error);
		exit(1);
	}

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	/* Open Socket Connection */
	ret = (connect(sock_send, (struct sockaddr *)&addr, sizeof(addr)) || bind(sock_recv, (struct sockaddr *)&addr, sizeof(addr)));

	if (ret)
	{
		sprintf(error, "Error during socket connection. Error code is %d\n", ret);
		perror(error);
		exit(2);
	}

	printf("Connected to: %s\n", DEVICE_INTERFACE);

	printf("Socket connection created!\n");

	/* *********************************************** */

	/* ********** CAN FRAMES AND FILTERS CREATION ********** */

	/* CAN Frames Creation */
	create_egress_BCM(&msg_500, MSG_500_ID, MSG_500_DLC);
	create_egress_BCM(&msg_501, MSG_501_ID, MSG_501_DLC);
	create_egress_BCM(&msg_502, MSG_502_ID, MSG_502_DLC);

	/* Fixed frames creation (of course to be removed; this is just an example on how to write to the can frame) */
    memcpy(msg_500.frame.data,(__u8[]){0x00,0x28,0xFF,0x00,0x00,0x01,0xFF,0x00},MSG_500_DLC);
    memcpy(msg_501.frame.data,(__u8[]){0x00,0x32,0xFF,0x00,0x00,0x01},MSG_501_DLC);
    memcpy(msg_502.frame.data,(__u8[]){0x00,0x3C,0xFF,0x00,0x00},MSG_502_DLC);

	msg_50x_sniffer = parse_MMR_CAN_msg_id();

	create_sniffer_filters(rfilter, msg_50x_sniffer);
	setsockopt(sock_recv, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	/* ***************************************************** */

	/* Write CAN messages to kernel space. From now on, the kernel will transmit all 3 CAN frames to the CAN network every 100ms without the need of the syscall write() */
	write(sock_send, &msg_500, sizeof(can_msg));
	write(sock_send, &msg_501, sizeof(can_msg));
	write(sock_send, &msg_502, sizeof(can_msg));

	struct can_frame recv_frame;
	while(1)
	{
		/* Read Frames */
		nbytes = read(sock_recv, &recv_frame, sizeof(struct can_frame));

		if (nbytes < 0) {
        	perror("CAN RAW Socket read error.");
        	exit(3);
		}

		if (nbytes < sizeof(struct can_frame)) {
			perror("Read of an incomplete CAN frame\n");
			exit(4);
		} else if (nbytes == sizeof(struct can_frame))
		{
			char* msg_desc = get_MMR_CAN_frame_descripion(msg_50x_sniffer, recv_frame.can_id & CAN_SFF_MASK);
			if (msg_desc != NULL)
			{
				printf("ID: %.3x\t --> %s.\n\tDATA:\t", recv_frame.can_id & CAN_SFF_MASK, msg_desc);
			}
			for (__u8 i=0; i < recv_frame.can_dlc; i++)
			{
				printf("%d:%x ", i, recv_frame.data[i]);
			}
			printf("\n");
		}
	}

	/* Session terminated -> Close socket connection. */
	ret = (close(sock_send) || close(sock_recv));
	if (ret)
	{
		sprintf(error, "Error during socket close. Error code is: %d\n", ret);
		perror(error);
		exit(3);
	}

	printf("Socket connection closed.\n\nTerminating\n");

	return 0;
}
