/*
 * subscriptiontask.hpp
 *
 *  Created on: Oct 5, 2014
 *      Author: Pradyumna Upadhya
 */

#ifndef SUBSCRIPTIONTASK_HPP_
#define SUBSCRIPTIONTASK_HPP_

#include <stdint.h>
#include "can.h"
#include "vector.hpp"
#include "scheduler_task.hpp"
#include <stdio.h>
#include "boardIds.hpp"
#include "file_logger.h"
#include <string.h>
#include "semphr.h"

#define DEBUG_SUBSCRIPTION_TASK 1

/**
 * Encoded enumeration of message rate and the index.
 * We need the message rate in ms to set the task's run() method call frequency, but
 * we also need an index to be able to add subscription messages to the correct list.
 */
typedef enum {
    msgRate1Hz  = (1000 << 4) | 0,
    msgRate5Hz  = ( 200 << 4) | 1,
    msgRate10Hz = ( 100 << 4) | 2,
    msgRate20Hz = (  50 << 4) | 3,
    msgRate50Hz = (  20 << 4) | 4,

    /// Marks the last entry; do not use!
    msgRateLast = (0 << 4) | 5,
} msgRate_t;

/** @{ Converts msgRate_t to frequency in milliseconds and its index */
static inline uint8_t msgRateToIndex(msgRate_t mr)   { return (mr & 0x0F); }
static inline uint16_t msgRateToFreqMs(msgRate_t mr) { return (mr >> 4);   }
/** @} */

static inline msgRate_t convertHzTomsdRateEnum(uint8_t nHz)
{
    msgRate_t rate=msgRate5Hz;
    switch (nHz)
    {
        case 1 ... 4:
        rate=msgRate1Hz;
        break;

        case 5 ... 9:
        rate=msgRate5Hz;
        break;

        case 10 ... 19:
        rate=msgRate10Hz;
        break;

        case 20 ... 49:
        rate=msgRate20Hz;
        break;

        case 50:

        rate=msgRate50Hz;
        break;

        default:
        rate=msgRate1Hz;
        break;
    }

    return rate;
}

/**
 * Structure of a subscribed message
 */
typedef struct {
    uint8_t dstAddr;        ///< The destination address of a node
    uint16_t msgNum;        ///< The message number for the node
    can_msg_t *canMsgPtr;   ///< Pointer to the actual CAN message pointer
} subscribedMsg_t;



/**
 * The task that sends out the subscription messages of a list at the defined frequency.
 */
static const uint8_t SubListSize=5;

class canSubscribedMsgTask : public scheduler_task
{
    public:
        canSubscribedMsgTask(msgRate_t rate,                ///< The message rate of this task
                             VECTOR<subscribedMsg_t> *ptr,  ///< The subscription message list to send
                             uint8_t priority               ///< The priority of this task
                             );

        /// FreeRTOS task method
        bool run(void *p);

        bool init(void);

        /// Add a subscription message to be sent at the given rate
        static bool addSubscribedMsg(msgRate_t rate,        ///< The message rate
                                     uint8_t dst,           ///< Destination address
                                     uint16_t msgNum,       ///< Message number
                                     can_msg_t*&pCanMsg     ///< Actual CAN message pointer
                                     );

        static bool PublishSubscribedMessageData(const uint16_t msgNum,const uint8_t*const msgData,const uint8_t msgDataSize);

    private:
        canSubscribedMsgTask();                 ///< Private default constructor, do not use.
        const msgRate_t mMsgRate;               ///< This tasks' message rate
        VECTOR<subscribedMsg_t> *mpSubsMsgList; ///< The pointer to the subscription list

        /// The list of subscription message list
        static VECTOR<subscribedMsg_t> *mpSubsList[SubListSize];
};


void addSubscriptionTasks(uint8_t priority);


#endif /* SUBSCRIPTIONTASK_HPP_ */
