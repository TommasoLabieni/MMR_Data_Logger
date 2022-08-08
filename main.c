#include <data_logger.h>

int main(int argc, char **argv)
{
	/* ********** VARS DECLARATION ********** */

	__s32 s;
	struct sockaddr_can addr;
	struct ifreq ifr;
	err_t ret;
	error_msg error;
	__u32 nbytes;
	can_msg msg_500, msg_501, msg_502;

	/* ************************************** */


	/* ********** CONNECTION TO CAN NETWORK ********** */

	/* Socket Creation */
	s = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);

	/* Connection to CAN with interface = DEVICE_INTERFACE */
	strcpy(ifr.ifr_name, DEVICE_INTERFACE);
	ret = ioctl(s, SIOCGIFINDEX, &ifr);
	
	if (ret)
	{
		sprintf(error, "Error during connection to CAN Bus interface: %s. Error code is: %d\n", DEVICE_INTERFACE, ret);
		perror(error);
		exit(1);
	}

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	/* Open Socket Connection */
	ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	printf("Connected to: %s\n", DEVICE_INTERFACE);

	if (ret)
	{
		sprintf(error, "Error during socket connection. Error code is %d\n", ret);
		perror(error);
		exit(2);
	}

	printf("Socket connection created!\n");

	/* *********************************************** */

	/* ********** CAN FRAMES AND FILTERS CREATION ********** */

	/* CAN Frames Creation */
	create_frame(&msg_500, 0x500, MSG_500_DLC);
	create_frame(&msg_501, 0x501, MSG_501_DLC);
	create_frame(&msg_502, 0x502, MSG_502_DLC);

	/* Fixed frames creation (of course to be removed; this is just an example on how to write to the can frame) */
    memcpy(msg_500.frame.data,(__u8[]){0x00,0x28,0xFF,0x00,0x00,0x01,0xFF,0x00},MSG_500_DLC);
    memcpy(msg_501.frame.data,(__u8[]){0x00,0x32,0xFF,0x00,0x00,0x01},MSG_501_DLC);
    memcpy(msg_502.frame.data,(__u8[]){0x00,0x3C,0xFF,0x00,0x00},MSG_502_DLC);
	
	//TODO: Add Filters

	/* ***************************************************** */

	/* Write CAN messages to kernel space. From now on, the kernel will transmit all 3 CAN frames to the CAN network every 100ms without the need of the syscall write() */
	write(s, &msg_500, sizeof(can_msg));
	write(s, &msg_501, sizeof(can_msg));
	write(s, &msg_502, sizeof(can_msg));

	while(1) {;}

	/* Session terminated -> Close socket connection. */
	ret = close(s);
	if (ret)
	{
		sprintf(error, "Error during socket close. Error code is: %d\n", ret);
		perror(error);
		exit(3);
	}

	printf("Socket connection closed.\n\nTerminating\n");

	return 0;
}
