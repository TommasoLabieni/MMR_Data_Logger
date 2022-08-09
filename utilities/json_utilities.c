#include <json_utilities.h>

void parse_MMR_CAN_msg_id(const char * const monitor)
{
    cJSON_msg_500_t msg_500;
    cJSON *msg_parser, *msg_parser_it, *msg_info, *msg_info_it;
    
    cJSON *monitor_json = cJSON_Parse(monitor);

    if (monitor_json == NULL)
    {   
        perror( "Error creating monitor");
        exit(1);
    }

    msg_parser = cJSON_GetObjectItemCaseSensitive(monitor_json, "msg_500");
    create_msg_500(&msg_500, msg_parser);
}

int create_msg_500(cJSON_msg_500_t* msg, cJSON* parser)
{
    cJSON *ECU_payload, *ECU_payload_it, *parser_it, *msg_info, *msg_info_it;

    err_t ret;
    
    /* Matrix containing all MSG 500 fields */
    char *fields[MSG_500_DLC] = {"SPEED_ACTUAL", "SPEED_TARGET","STEER_ACTUAL", "STEER_TARGET","BRAKE_ACTUAL", "BRAKE_TARGET","MOTOR_M_ACTUAL", "MOTOR_M_TARGET"}; 

    /* Allocate memory for msg info */
    msg->msg_info.msg_info_ptr = malloc(MSG_500_DLC * sizeof(cJSON_msg_info_t));

    if (msg == NULL)
    {
        perror("Error allocating memory for can_msg");
        return -1;
    }

    /* Assign msg_info_t description from fields matrix */
    for (unsigned int i=0; i < MSG_500_DLC; i++)
    {
        strcpy(msg->msg_info.msg_info_desc[i], fields[i]);
    } 

    for (unsigned int i=0; i < MSG_500_DLC; i++)
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

int check_msg_info_correctness(unsigned int msg_id, char* msg_desc, cJSON_msg_info_t* msg)
{
    printf("CHECKING MESSAGE %x. FIELD: %s\n", msg_id, msg_desc);

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
        perror("SCALE Is NaN");
        return -1;
    }

    /* Check MMR_ID */
    if (!cJSON_IsNumber(msg->msg_MMR_ID))
    {
        perror("MMR_ID Is NaN");
        return -1;
    }

    return 0;
}

void print_common_msgs_info(cJSON_msg_info_t* msg)
{
    char info[1000];

    sprintf(info, "Payload bit Start: %d\nPayload bit End: %d\nFORMAT: %s\nIS_PERC: %s\n", 
        msg->msg_PBS->valueint,
        msg->msg_PBE->valueint,
        msg->msg_FORMAT->valuestring,
        (msg->msg_IS_PERCENTAGE->valueint  ? "true" : "false")
    );

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

    printf("%s\n", info);
}
