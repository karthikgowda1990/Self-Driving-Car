/*
 * boardIds.h
 *
 *  Created on: Oct 6, 2014
 *      Author: Pradyumna Upadhya
 */

#ifndef BOARDIDS_H_
#define BOARDIDS_H_
#include <stdint.h>

const uint32_t MASTER_CAN29_EXGRP1START=0x05000000U;
const uint32_t MASTER_CAN29_EXGRP1END=0x050FFFFFU;

const uint32_t GPS_CAN29_EXGRP1START=0x05100000U;
const uint32_t GPS_CAN29_EXGRP1END=0x051FFFFFU;

const uint32_t MOTOR_CAN29_EXGRP1START=0x05200000U;
const uint32_t MOTOR_CAN29_EXGRP1END=0x052FFFFFU;

const uint32_t SENSOR_CAN29_EXGRP1START=0x05300000U;
const uint32_t SENSOR_CAN29_EXGRP1END=0x053FFFFFU;

const uint32_t IO_CAN29_EXGRP1START=0x05400000U;
const uint32_t IO_CAN29_EXGRP1END=0x054FFFFFU;

const uint32_t APP_CAN29_EXGRP1START=0x05500000U;
const uint32_t APP_CAN29_EXGRP1END=0x055FFFFFU;

/**
 * Have an enumeration of controller IDs
 */
typedef enum:uint8_t {
    cid_master_controller = 0x50,
    cid_geographical_controller = 0x51,
    cid_motor_controller = 0x52,
    cid_sensor_controller = 0x53,
    cid_inputOutput_controller = 0x54,
    cid_andriod_controller = 0x55
} cid_t;


/*************************** Message IDs of boards************************************************/

/**
 * NOTE: & TODO: MessageIDs are not complete. Define all the messages properly.
 */

/* NOTE: follow this range for messages
 *
Reserved     0x000 - 0x0FF
Common commands     0x100 - 0x1FF
Common responses    0x200 - 0x2FF
Controller specific commands    0x300 - 0x3FF
Add subscription message(s)     0x400 - 0x4FF
Subscribed messages     0x500 - 0x5FF

EXAMPLE:

FORMAT is mid_BOARDNAME_MESSAGETYPE_MESSAGENAME
where BOARDNAME is like master,geographical,motor,sensor,inputOutput,andriod
where MESSAGETYPE is like reserved,common,cntrlr,subsp,subsc
where MESSAGE NAME is like motorspeed

EXAMPLE: below see the relation between mid_motor_subsp_motorspeed and mid_motor_subsc_motorspeed
if Master wants motors speed then it sends 0x43C to subscribe it from motor.
 Motor in turn sends can message with 0x53C and motot speed as can data.

 */

//NOTE: message Id size is 12 bits. Therefore last 4msbs are not used here in below ids.
typedef enum:uint16_t{

    //common commands
    mid_commoncmd_boardStatus = 0x110,
    mid_commonresponse_boardStatus = 0x210,
    mid_commoncmd_getTime = 0x111,
    mid_commonresponse_presentTime = 0x211,
    mid_commonresponse_boardsNewData = 0x213, //response to 0x313

    //controller specific commands
    mid_master_controllercmd_reset = 0x310,
    mid_master_controllercmd_setTime = 0x311,
    mid_master_controllercmd_start = 0x312,
    mid_master_controllercmd_stop = 0x313,
    mid_master_controllercmd_GetBoardData = 0x314,

    //subscription messages for motor and corresponding publish response
    mid_master_subscribe_setNewMotorData = 0x42A,
    mid_master_publish_setNewMotorData = 0x52A,
    mid_master_subscribe_setNewMotorSpeed = 0x42B,
    mid_master_publish_setNewMotorSpeed = 0x52B,
    mid_master_subscribe_setNewMotorDirection= 0x42C,
    mid_master_publish_setNewMotorDirection = 0x52C,
    //subscription messages for andriod and corresponding publish response
    mid_master_subscribe_getCurrentSensorData = 0x45C,
    mid_master_publish_getCurrentSensorData = 0x55C,
    mid_master_subscribe_getCurrentLocData = 0x45D,
    mid_master_publish_getCurrentLocData = 0x55D,
    //susbscription messages for display and corresponding publish response
    mid_inputOutput_subscribe_getDisplayData = 0x44A,
    mid_inputOutput_publish_getDisplayData = 0x54A

}mid_master_t;

typedef enum:uint16_t{
    //subscription messages for master and corresponding publish response
    mid_compass_subscribe_data = 0x40C,
    mid_compass_publish_data = 0x50C,
    mid_gps_subscribe_data = 0x40B,
    mid_gps_publish_data = 0x50B,
    mid_geo_subscribe_data = 0x40A,
    mid_geo_publish_data = 0x50A,
    mid_geo_subscribe_data_getdisplayData = 0x44A,
    mid_geo_publish_data_getdisplayData = 0x54A
    //
}mid_geographical_t;

typedef enum:uint16_t{

    //subscription messages for master and corresponding subscribed response
    mid_motor_subscribe_motorData = 0x40A,
    mid_motor_publish_motorData = 0x50A,
    mid_motor_subscribe_motorSpeed = 0x40B,
    mid_motor_publish_motorSpeed = 0x50B,
    mid_motor_subscribe_motorDirection = 0x40C,
    mid_motor_publish_motorDirection = 0x50C
    //

}mid_motor_t;

typedef enum:uint16_t{
    //subscription message for master and corresponding publish response
    mid_sensor_subscribe_data = 0x40A,
   mid_sensor_publish_data = 0x50A,
   mid_sensor_subscribe_data_display = 0x44A,
   mid_sensor_publish_data_display = 0x54A

}mid_sensor_t;

typedef enum:uint16_t{
    //subscription message for master and corresponding publish response
    mid_inputOutput_subscribe_data = 0x40A,
    mid_inputOutput_publish_data = 0x50A
}mid_inputOutput_t;

typedef enum:uint16_t{
    //subscription message for master and corresponding publish response
    mid_android_controllercmd_gpsDestination_latitude = 0x353,
    mid_android_controllercmd_gpsDestination_longitude = 0x354,
    mid_android_controllercmd_gpsDestination_eom = 0x355,
    mid_andriod_subscribe_startStop = 0x40A,
    mid_andriod_publish_startStop = 0x50A,
    mid_andriod_subscribe_getNewGPSData = 0x40D,
    mid_andriod_publish_getNewGPSData = 0x50D,
    mid_andriod_subscribe_getNewGPSlongitude = 0x40B,
    mid_andriod_publish_getNewGPSlongitude = 0x50B,
    mid_andriod_subscribe_getNewGPSlatitude = 0x40C,
    mid_andriod_publish_getNewGPSlatitude = 0x50C
}mid_andriod_t;
/**********************************************END OF MESSAGEIDs*****************************************************/

/**
 * Each controller shall then set its own ID
 * NOTE: CHANGE THIS FOR EVERY BOARD...DON'T FORGET!!!!!
 * TODO:  " " same " "
 */
const cid_t our_controller_id = cid_inputOutput_controller;

/**
 * Create a "union" whose struct overlaps with the uint32_t
 */
const int8_t CAN_SOURCEIDBITS=8;
const int8_t CAN_DSTIDBITS=8;
const int8_t CAN_MESSAGEBITS=12;


typedef union {
    struct {
        uint32_t msg_num : CAN_MESSAGEBITS; ///< Message number
        uint32_t src : CAN_SOURCEIDBITS;      ///< Source ID
        uint32_t dst : CAN_DSTIDBITS;      ///< Destination ID
        uint32_t : 1;          ///< Unused (reserved 29th bit)
    };
    /// This "raw" overlaps with <DST> <SRC> <ID>
    uint32_t raw;

} __attribute__((packed)) controller_id_t;


/**
 * Creates a message ID based on the message ID protocol
 * @param [in] dst  The destination controller ID
 * @param [in] msg_num  The message number to send to the dst controller
 *
 * @returns  The 32-bit message ID created by the input parameters
 */
static inline uint32_t make_id(uint8_t dst, uint16_t msg_num)
{
    controller_id_t cid;
    cid.raw = 0;
    cid.msg_num  = msg_num;
    cid.src = our_controller_id;
    cid.dst = dst;
    return cid.raw;
}


////////////////////////MOTOR ENUMS/////////////////////////////////////
typedef enum:uint8_t
{
    motor_front=0,
    motor_back=1
} motor_type_t;


typedef enum:uint8_t {
    motor_dir_l90D = 11,
    motor_dir_l70D = 12,
    motor_dir_l55D = 13,
    motor_dir_l25D = 14,
    motor_dir_0D = 15,
    motor_dir_r25D = 16,
    motor_dir_r55D = 17,
    motor_dir_r70D = 18,
    motor_dir_r90D = 19,
} motor_direction_t;

typedef enum:uint8_t {
    motor_speed_b100 = 11,
    motor_speed_b80 = 12,
    motor_speed_b60 = 13,
    motor_speed_b30 = 14,
    motor_speed_0 = 15,
    motor_speed_f30 = 16,
    motor_speed_f60 = 17,
    motor_speed_f80 = 18,
    motor_speed_f100 = 19,
} motor_speed_t;

typedef enum:uint8_t {
   motor_state_continue = 0,
   motor_state_update = 1,
   motor_state_deaccelerate = 2,
} motor_state_t;

typedef enum{
    motor_dataIndex_state =0,
    motor_dataIndex_direction=1,
    motor_dataIndex_speed=2,
    motor_dataIndex_distance=3,
}motor_dataIndex_t;

///////////////////////MOTOR ENUMS END////////////////////////////////////


//////////////////////SENOR ENUMS/////////////////////////////////////////
typedef enum{
    sensor_dataIndex_upperLeft =0,
    sensor_dataIndex_upperMiddle=1,
    sensor_dataIndex_upperRight=2,
    sensor_dataIndex_middleRight=3,
    sensor_dataIndex_bottomRight=4,
    sensor_dataIndex_bottomMiddle=5,
    sensor_dataIndex_bottomLeft=6,
    sensor_dataIndex_middleLeft=7
}sensor_dataIndex_t;
/////////////////////SENSOR ENUMS END/////////////////////////////////////


#endif /* BOARDIDS_H_ */
