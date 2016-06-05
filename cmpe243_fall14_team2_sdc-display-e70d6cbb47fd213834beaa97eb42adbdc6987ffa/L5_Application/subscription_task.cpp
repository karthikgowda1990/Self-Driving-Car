/*
 * subscriptiontask.cpp
 *
 *  Created on: Oct 5, 2014
 *      Author: Pradyumna Upadhya
 */



#include "subscription_task.hpp"


/// Shared list of vector pointers between all of the instances of canSubsribedMsgTask (s)
VECTOR<subscribedMsg_t> *canSubscribedMsgTask::mpSubsList[SubListSize] = { 0 };

void makeCANMessamgeInvalid(can_msg_t& msg)
{
    //by default setting this...do samething in addsubsrciption function.
    memset(msg.data.bytes, 0, sizeof(msg.data.bytes));
    msg.data.bytes[0]  = 0xFF;
    msg.frame_fields.data_len = 0;
}

bool IsCANMessageInvalid(can_msg_t& msg)
{
    return (msg.data.bytes[0] == 0xFF && msg.frame_fields.data_len == 0);
}

bool canSubscribedMsgTask::PublishSubscribedMessageData(const uint16_t msgNum,const uint8_t *const msgData,const uint8_t msgDataSize)
{
    subscribedMsg_t subsMsg;

    if(NULL == msgData)
    {
        return false;
    }

    for(uint8_t listVectorIndex=0;listVectorIndex<SubListSize;++listVectorIndex)
    {
        VECTOR<subscribedMsg_t> *pSubsMsgList = canSubscribedMsgTask::mpSubsList[listVectorIndex];

        if( NULL == pSubsMsgList)continue;

    for (unsigned int i = 0; i < pSubsMsgList->size(); i++)
    {
        subsMsg = (*pSubsMsgList) [i];

        can_msg_t&  msg = *(subsMsg.canMsgPtr);

        if(subsMsg.msgNum != msgNum)continue;

        vPortEnterCritical();

        makeCANMessamgeInvalid(msg);

        for(uint8_t i=0;i<msgDataSize;++i)
        {
            msg.data.bytes[i]=msgData[i];
        }
        msg.frame_fields.data_len=msgDataSize;

        vPortExitCritical();

#ifdef DEBUG_SUBSCRIPTION_TASK
        //printf("MASTER: New subscribed message published with message id : %x to %x for message : %x. Message size:%x \n",msg.msg_id,subsMsg.dstAddr,subsMsg.msgNum,msg.frame_fields.data_len );
#endif
        }
    }

    return true;

}


canSubscribedMsgTask::
canSubscribedMsgTask(msgRate_t rate,                ///< The message rate of this task
                     VECTOR<subscribedMsg_t> *ptr,  ///< The subscription message list to send
                     uint8_t priority               ///< The priority of this task
                     ) :
    scheduler_task("sendMsg", 3 * 512, priority),   ///< Default constructor calls
    mMsgRate(rate),                           ///< Store our message rate
    mpSubsMsgList(ptr)                              ///< Store our subscription list pointer
{
    /* Set the task rate to call the run() method */
    setRunDuration(msgRateToFreqMs(mMsgRate));

    /* Add our subscribed message list pointer to the list such that
     * addSubscribedMsg() method can add subscribed messages to our list.
     */

    mpSubsList[msgRateToIndex(mMsgRate)] = mpSubsMsgList;
}


bool canSubscribedMsgTask::init(void)
{
}

bool canSubscribedMsgTask::run(void *p)
{
    subscribedMsg_t subsMsg;
    const can_t canbusNum = can1;  /* CAN Bus to use */
    const uint32_t timeoutMs = 50; /* Some reasonable time */
    can_msg_t msg;

#ifdef DEBUG_SUBSCRIPTION_TASK
    //puts("MOTOR: canSubscribedMsgTask::run\n");
#endif

    for (unsigned int i = 0; i < mpSubsMsgList->size(); i++)
    {

        subsMsg = (*mpSubsMsgList) [i];

        vPortEnterCritical();
        msg = *(subsMsg.canMsgPtr);
        //removing reference and doing assignment as if its reference it may get partially new data
        //before CAN_tx() call after exiting critical region
        vPortExitCritical();

#ifdef DEBUG_SUBSCRIPTION_TASK
        //printf("MASTER:subsb msg sent with message id : %x to %x for message : %x. Message size:%x \n",msg.msg_id,subsMsg.dstAddr,(subsMsg.msgNum),msg.frame_fields.data_len );
#endif
        if( true == !IsCANMessageInvalid(msg) )
        {


        if (!CAN_tx(canbusNum, &msg, timeoutMs)) {
            LOG_ERROR("Error sending message from %uHz task within %u ms",
                      msgRateToFreqMs(mMsgRate), timeoutMs);
        }

        }

    }

    return true;
}

bool CheckForDuplicates(uint16_t& messageNumber,const VECTOR<subscribedMsg_t> *const element)
{
   uint8_t size=element->size();
   for(uint8_t i=0;i<size;++i)
   {
       if(messageNumber == (*element)[i].msgNum)
           return true;
   }

   return false;
}

bool canSubscribedMsgTask::addSubscribedMsg(msgRate_t rate, uint8_t dst, uint16_t msgNum, can_msg_t*& pCanMsg)
{
    bool ok = false;

    /* Populate the fields of the subscribed message */
    subscribedMsg_t subsMsg;
    subsMsg.dstAddr = dst;
    subsMsg.msgNum = msgNum;
    subsMsg.canMsgPtr = pCanMsg;
    pCanMsg->msg_id = make_id(subsMsg.dstAddr, subsMsg.msgNum);

    /* Look up the vector for this msgRate_t(rate), and check if it has capacity */
    VECTOR<subscribedMsg_t> *vectorPtr =  mpSubsList[msgRateToIndex(rate)];

    if (! (ok = (NULL != vectorPtr))) {
        //LOG_ERROR("Vector pointer for %uHz task was NULL, was the task created?",
                   //msgRateToFreqMs(rate));
#if DEBUG_SUBSCRIPTION_TASK
        printf("Vector pointer for %uHz task was NULL, was the task created?",
                msgRateToFreqMs(rate));
#endif

    }
    else {
        /* Add the subscription message to our list or if no capacity, log an error*/
        if ((ok = (vectorPtr->size() < vectorPtr->capacity()))) {

            //NOTE:check  if this message entry is already there. If its there dnt add again

            if(!CheckForDuplicates(msgNum,vectorPtr))
            {
                makeCANMessamgeInvalid(*pCanMsg);
                vPortEnterCritical();
               (*vectorPtr).push_back(subsMsg) ;
                vPortExitCritical();
            }
#if DEBUG_SUBSCRIPTION_TASK
            else
            {
                //puts("Duplicate subscription message got\n");
                delete pCanMsg;
                pCanMsg=NULL;
            }
#endif
        }
        else {
            //LOG_ERROR("List capacity for %uHz task has exceeded maximum subscriptions",
                     // msgRateToFreqMs(rate));
        }
    }

    return ok;
}

void addSubscriptionTasks(uint8_t priority)
{
    const int maxMsgsPerList = 50;

    VECTOR<subscribedMsg_t> *list1Hz  = new VECTOR<subscribedMsg_t> (maxMsgsPerList);
    VECTOR<subscribedMsg_t> *list5Hz  = new VECTOR<subscribedMsg_t> (maxMsgsPerList);
    VECTOR<subscribedMsg_t> *list10Hz = new VECTOR<subscribedMsg_t> (maxMsgsPerList);
    VECTOR<subscribedMsg_t> *list20Hz = new VECTOR<subscribedMsg_t> (maxMsgsPerList);
    VECTOR<subscribedMsg_t> *list50Hz = new VECTOR<subscribedMsg_t> (maxMsgsPerList);

    scheduler_add_task(new canSubscribedMsgTask(msgRate1Hz,  list1Hz,  priority));
    scheduler_add_task(new canSubscribedMsgTask(msgRate5Hz,  list5Hz,  priority));
    scheduler_add_task(new canSubscribedMsgTask(msgRate10Hz, list10Hz, priority));
    scheduler_add_task(new canSubscribedMsgTask(msgRate20Hz,  list20Hz,  priority));
    scheduler_add_task(new canSubscribedMsgTask(msgRate50Hz,  list50Hz,  priority));
}


