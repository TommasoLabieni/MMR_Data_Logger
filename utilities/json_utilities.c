#include <json_utilities.h>

char* fileToCString(char* filename);

cJSON_msg_50x_t* parse_MMR_CAN_msg_id()
{
    cJSON_msg_50x_t* msg_50x = malloc(3*sizeof(cJSON_msg_50x_t));
    cJSON *msg_parser, *msg_parser_it, *msg_info, *msg_info_it;
    
    const char * const monitor = fileToCString(JSON_FILENAME);
    
    cJSON *monitor_json = cJSON_Parse(monitor);

    if (monitor_json == NULL)
    {   
        perror( "Error creating monitor");
        exit(1);
    }


    /* ********** CREATION OF MSG 500  ********** */
    
    msg_parser = cJSON_GetObjectItemCaseSensitive(monitor_json, "msg_500");

    /* Matrix containing all MSG 500 fields */
    char *msg_500_fields[MSG_500_N_MMR_MSGS] = {"SPEED_ACTUAL", "SPEED_TARGET","STEER_ACTUAL", "STEER_TARGET","BRAKE_ACTUAL", "BRAKE_TARGET","MOTOR_M_ACTUAL", "MOTOR_M_TARGET"}; 
    
    create_msg_50x(&msg_50x[0], msg_parser, 0x500, MSG_500_DLC, MSG_500_N_MMR_MSGS, msg_500_fields);
    
    /* ****************************************** */

    /* ********** CREATION OF MSG 501  ********** */

    msg_parser = cJSON_GetObjectItemCaseSensitive(monitor_json, "msg_501");

    /* Matrix containing all MSG 501 fields */
    char *msg_501_fields[MSG_501_N_MMR_MSGS] = {"ACC_LONGITUDINAL", "ACC_LATERAL", "YAW_RATE"}; 
    
    create_msg_50x(&msg_50x[1], msg_parser, 0x501, MSG_501_DLC, MSG_501_N_MMR_MSGS, msg_501_fields);
    
    /* ****************************************** */

    /* ********** CREATION OF MSG 502  ********** */

    msg_parser = cJSON_GetObjectItemCaseSensitive(monitor_json, "msg_502");

    /* Matrix containing all MSG 502 fields */
    char *msg_502_fields[MSG_502_N_MMR_MSGS] = {"AS_STATE", "EBS_STATE","AMI_STATE", "STEERING_STATE","SERVICE_BRAKE_STATE", "LAP_COUNTER","CONES_COUNT_ACTUAL", "CONES_COUNT_TOTAL"}; 

    create_msg_50x(&msg_50x[2], msg_parser, 0x502,  MSG_502_DLC, MSG_502_N_MMR_MSGS, msg_502_fields);

    /* ****************************************** */

    return msg_50x;
}

int create_msg_50x(cJSON_msg_50x_t* msg, cJSON* parser, canid_t msg_id, unsigned int msg_dlc, unsigned int n_MMR_msgs, char** fields)
{
    printf("Creating Message 0x%.3x\n", msg_id & 0x7FF);
    cJSON *ECU_payload, *ECU_payload_it, *parser_it, *msg_info, *msg_info_it;

    err_t ret;
    
    /* Assign msg id */
    msg->msg_id = msg_id;

    /* Assign msg DLC */
    msg->msg_dlc = msg_dlc;

    /* Allocate memory for msg info */
    msg->msg_info.msg_info_ptr = malloc(n_MMR_msgs * sizeof(cJSON_msg_info_t));

    if (msg == NULL)
    {
        perror("Error allocating memory for can_msg");
        return -1;
    }

    /* Assign msg_info_t description from fields matrix */
    for (unsigned int i=0; i < n_MMR_msgs; i++)
    {
        strcpy(msg->msg_info.msg_info_desc[i], fields[i]);
    }

    /* Assign the length of the frame */
    msg->msg_info.length =  n_MMR_msgs;

    for (unsigned int i=0; i < n_MMR_msgs; i++)
    {
        cJSON_ArrayForEach(parser_it, parser)
        {
            msg_info = cJSON_GetObjectItemCaseSensitive(parser_it, fields[i]);
            cJSON_ArrayForEach(msg_info_it, msg_info)
            {
                /* Parse all values (even if a msg does not contain that value. The correctness will be check later) */
                msg->msg_info.msg_info_ptr[i].msg_PBS = cJSON_GetObjectItemCaseSensitive(msg_info_it, "PAYLOAD_BIT_START");
                msg->msg_info.msg_info_ptr[i].msg_PBE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "PAYLOAD_BIT_END");
                msg->msg_info.msg_info_ptr[i].msg_FORMAT = cJSON_GetObjectItemCaseSensitive(msg_info_it, "FORMAT");
                msg->msg_info.msg_info_ptr[i].msg_IS_PERCENTAGE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "IS_PERCENTAGE");
                msg->msg_info.msg_info_ptr[i].msg_MAX_PERC_VALUE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "MAX_PERC_VALUE");
                msg->msg_info.msg_info_ptr[i].msg_IS_ECU_MESSAGE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "IS_ECU_MSG");
                /* Read payload msg */
                ECU_payload = cJSON_GetObjectItemCaseSensitive(msg_info_it, "ECU_PAYLOAD");
                cJSON_ArrayForEach(ECU_payload_it, ECU_payload)
                {
                    msg->msg_info.msg_info_ptr[i].msg_ECU_PAYLOAD = malloc(sizeof(cJSON_ecu_payload_t));
                    msg->msg_info.msg_info_ptr[i].msg_ECU_PAYLOAD->ecu_payload_BYTE_LENGTH = cJSON_GetObjectItemCaseSensitive(ECU_payload_it, "BYTE_LENGTH");
                    msg->msg_info.msg_info_ptr[i].msg_ECU_PAYLOAD->ecu_payload_BYTE_START = cJSON_GetObjectItemCaseSensitive(ECU_payload_it, "BYTE_START");
                    msg->msg_info.msg_info_ptr[i].msg_ECU_PAYLOAD->ecu_payload_DIVIDE_BY = cJSON_GetObjectItemCaseSensitive(ECU_payload_it, "DIVIDE_BY");
                }
                msg->msg_info.msg_info_ptr[i].msg_SCALE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "SCALE");
                msg->msg_info.msg_info_ptr[i].msg_MMR_ID = cJSON_GetObjectItemCaseSensitive(msg_info_it, "MMR_ID");
                msg->msg_info.msg_info_ptr[i].msg_MAX_ENUM_VALUE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "MAX_ENUM_VALUE");
                msg->msg_info.msg_info_ptr[i].msg_DATA_TYPE = cJSON_GetObjectItemCaseSensitive(msg_info_it, "DATA_TYPE");

                /* Check that these values are correct */
                
                ret = check_msg_info_correctness(MSG_500_ID, msg->msg_info.msg_info_desc[i], &(msg->msg_info.msg_info_ptr[i]));

                if (ret)
                {
                    return ret;
                }

                print_common_msgs_info(&(msg->msg_info.msg_info_ptr[i]));
            }
        }
    }
    
    return 0;
}

int check_msg_info_correctness(canid_t msg_id, char* msg_desc, cJSON_msg_info_t* msg)
{
    // printf("CHECKING MESSAGE %x. FIELD: %s\n", msg_id, msg_desc);

    /* Check PBS */
    if (!cJSON_IsNumber(msg->msg_PBS))
    {
        perror("PBS Is NaN");
        return -1;
    }
    /* Check PBE */
    if (!cJSON_IsNumber(msg->msg_PBE))
    {
        perror("PBE Is NaN");
        return -1;
    }
    /* Check FORMAT */
    if (!cJSON_IsNumber(msg->msg_FORMAT) && !cJSON_IsString(msg->msg_FORMAT))
    {
        perror("FORMAT Is NaN nor a String");
        return -1;
    }
    /* Check IS_PERCENTAGE */
    if (!cJSON_IsBool(msg->msg_IS_PERCENTAGE))
    {
        perror("IS_PERCENTAGE Is not a Bool");
        return -1;
    }

    /* Check MAX_PERC_VALUE if and only if IS_PERCENTAGE = true */
    if (msg->msg_IS_PERCENTAGE->valueint)
    {
        /* Check MAX_PERCENTAGE */
        if (!cJSON_IsNumber(msg->msg_MAX_PERC_VALUE))
        {
            perror("MAX_PERC_VALUE Is NaN");
            return -1;
        }
    }

    /* Check IS_ECU_MESSAGE */
    if (!cJSON_IsBool(msg->msg_IS_ECU_MESSAGE))
    {
        perror("IS_ECU_MESSAGE Is not a Bool");
        return -1;
    }

    /* Check ECU_PAYLOAD if and only if IS_ECU_MESSAGE = true */
    if (msg->msg_IS_ECU_MESSAGE->valueint)
    {
        /* Check BYTE_LENGTH */
        if (!cJSON_IsNumber(msg->msg_ECU_PAYLOAD->ecu_payload_BYTE_LENGTH))
        {
            perror("ECU_BYTE_LENGTH Is NaN");
            return -1;
        }
        /* Check ECU_BYTE_START */
        if (!cJSON_IsNumber(msg->msg_ECU_PAYLOAD->ecu_payload_BYTE_START))
        {
            perror("ECU_BYTE_START Is NaN");
            return -1;
        }
        /* Check DIVIDE_BY */
        if (!cJSON_IsNumber(msg->msg_ECU_PAYLOAD->ecu_payload_DIVIDE_BY))
        {
            perror("DIVIDE_BY Is NaN");
            return -1;
        }
    }

    /* Check SCALE */
    if (!cJSON_IsNumber(msg->msg_SCALE))
    {
        perror("SCALE Is not a String");
        return -1;
    }

    /* Check MMR_ID */
    if (!cJSON_IsNumber(msg->msg_MMR_ID))
    {
        perror("MMR_ID Is NaN");
        return -1;
    }

    /* Check DATA_TYPE */
    if (!cJSON_IsString(msg->msg_DATA_TYPE))
    {
        perror("MMR_ID Is NaN");
        return -1;
    }

    return 0;
}

void print_common_msgs_info(cJSON_msg_info_t* msg)
{
    char info[1000];

    sprintf(info, "Payload bit Start: %d\nPayload bit End: %d\n", 
        msg->msg_PBS->valueint,
        msg->msg_PBE->valueint
    );

    /* Checking FORMAT Data Type */
    if (cJSON_IsString(msg->msg_FORMAT))
    {
        /* String data type */
        sprintf(info, "%sFORMAT: %s\n", info, msg->msg_FORMAT->valuestring);
    } else
    {
        /* Integer data type */
        sprintf(info, "%sFORMAT: %d\n", info, msg->msg_FORMAT->valueint);
    }

    sprintf(info, "%sIS_PERCENTAGE: %s\n", info, (msg->msg_IS_PERCENTAGE->valueint  ? "true" : "false"));

    if (msg->msg_IS_PERCENTAGE->valueint)
    {
        sprintf(info, "%sMAX_PERC_VALUE: %d\n", info, msg->msg_MAX_PERC_VALUE->valueint);
    }

    sprintf(info, "%sIS_ECU_MESSAGE: %s\n", info, (msg->msg_IS_ECU_MESSAGE->valueint ? "true" : "false"));
    
    if (msg->msg_IS_ECU_MESSAGE->valueint)
    {
        sprintf(info, "%s\tECU_BYTE_LENGTH: %d\n\tECU_BYTE_START: %d\n\tECU_DIVIDE_BY: %d\n", info, 
            msg->msg_ECU_PAYLOAD->ecu_payload_BYTE_LENGTH->valueint,
            msg->msg_ECU_PAYLOAD->ecu_payload_BYTE_START->valueint,
            msg->msg_ECU_PAYLOAD->ecu_payload_DIVIDE_BY->valueint
        );
    }

    sprintf(info, "%sSCALE: %d\nMMR_ID: %.3x\nDATA_TYPE: %s", info, msg->msg_SCALE->valueint, msg->msg_MMR_ID->valueint, msg->msg_DATA_TYPE->valuestring);

    printf("%s\n", info);
}

char* fileToCString(char* filename)
{
    long len = 0;
    int temp;
    char *JSON_content;
	

    FILE* fp = fopen(filename, "rb+");
    if(!fp)
    {
		perror("err open");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    if(0 == len)
    {
		perror("len");
        return NULL;
    }

    fseek(fp, 0, SEEK_SET);
    JSON_content = (char*) malloc(sizeof(char) * len);
    temp = fread(JSON_content, 1, len, fp);

    fclose(fp);

    return JSON_content;
}

bool is_enum(char* data_type)
{
    return (!strcmp(data_type,"enum"));
}

bool is_float(char* data_type)
{
    return (!strcmp(data_type,"float"));
}

bool is_bool(char* data_type)
{
    return (!strcmp(data_type,"bool"));
}


bool is_short(char* data_type)
{
    return (!strcmp(data_type,"short"));
}

bool is_byte(char* data_type)
{
    return (!strcmp(data_type,"byte"));
}

int test_func(char *filename)
{
    char* JSON_content = fileToCString(filename);
    parse_MMR_CAN_msg_id(JSON_content);

	return 0;
}