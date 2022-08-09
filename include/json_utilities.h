#include <basic_utils.h>
#include <cjson/cJSON.h>

typedef struct
{
    cJSON *ecu_payload_BYTE_LENGTH;
    cJSON *ecu_payload_BYTE_START;
    cJSON *ecu_payload_DIVIDE_BY;
} cJSON_ecu_payload_t;

typedef struct
{
    cJSON *msg_PBS;                          /* Payload Byte Start */
    cJSON *msg_PBE;                          /* Payload Byte End */
    cJSON *msg_FORMAT;
    cJSON *msg_IS_PERCENTAGE;
    cJSON *msg_MAX_PERC_VALUE;
    cJSON *msg_IS_ECU_MESSAGE;
    cJSON_ecu_payload_t *msg_ECU_PAYLOAD;
    cJSON *msg_SCALE;   
    cJSON *msg_MMR_ID;

} cJSON_msg_info_t;

typedef struct
{
    /* Both fields have the same length which is MSG_50x_DLC */
    cJSON_msg_info_t* msg_info_ptr;
    char msg_info_desc[8][255];
    size_t length;
} cJSON_50x_msg_info_t;

typedef struct
{
    cJSON_50x_msg_info_t msg_info;
} cJSON_msg_500_t;

void parse_MMR_CAN_msg_id(const char * const monitor);
/*
    This function creates msg with ID 500
    @param msg: reference to the msg that has to be created
    @param parser: JSON File parser

    @return: 0 if success otherwise -1
*/
int create_msg_500(cJSON_msg_500_t* msg, cJSON* parser);
/*
    This function checks wether a msg has correctly formatted params or not

    @param msg_id: id of the msg to check
    @param msg_desc: brief description of the msg to check
    @param msg: msg to check

    @return: 0 if everything is correct otherwise -1
*/
int check_msg_info_correctness(unsigned int msg_id, char* msg_desc, cJSON_msg_info_t* msg);
void print_common_msgs_info(cJSON_msg_info_t* msg);