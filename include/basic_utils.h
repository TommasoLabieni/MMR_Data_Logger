#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef int err_t;
typedef char error_msg[255];

/* ********** CONSTANT VALUES DECLARATION ********** */

#define MSG_500_ID 0x500
#define MSG_501_ID 0x501
#define MSG_502_ID 0x502

#define MSG_500_DLC 8
#define MSG_501_DLC 6
#define MSG_502_DLC 5

/* ************************************************* */
