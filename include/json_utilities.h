#ifndef JSON_UTILITIES_H
#define JSON_UTILITIES_H

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
    cJSON *msg_DATA_TYPE;

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
    canid_t msg_id;
    unsigned int msg_dlc;
} cJSON_msg_50x_t;


/*
    This function is used to parse a JSON-MMR-formatted file and to return the reference to the 3 CAN frames to egress.

    @return: reference to the 3 CAN frames to egress

*/ 
cJSON_msg_50x_t* parse_MMR_CAN_msg_id();
/*
    This function creates msg with ID 50x
    @param msg: reference to the msg that has to be created
    @param parser: JSON File parser
    @param msg_id: Id of the message
    @param msg_dlc: DLC of the message
    @param n_MMR_msgs: number of messages to parse 
    @param fields: JSON-MMR-formatted Field to parse

    @return: 0 if success otherwise -1
*/
int create_msg_50x(cJSON_msg_50x_t* msg, cJSON* parser, canid_t msg_id, unsigned int msg_dlc, unsigned int n_MMR_msgs, char** fields);
/*
    This function checks wether a msg has correctly formatted params or not

    @param msg_id: id of the msg to check
    @param msg_desc: brief description of the msg to check
    @param msg: msg to check

    @return: 0 if everything is correct otherwise -1
*/
int check_msg_info_correctness(canid_t msg_id, char* msg_desc, cJSON_msg_info_t* msg);
/*
    This function simply prints all the information by a generic msg

    @param msg: Msg to be printed

    @return: nothing
*/
void print_common_msgs_info(cJSON_msg_info_t* msg);
/* 
    Thif function reads a JSON file (with MMR Format) and test the software

    @param filename: Name of the JSON file

    @return: 0 if ok otherwise -1
*/
int test_func(char* filename);
#endif //JSON_UTILITIES_H