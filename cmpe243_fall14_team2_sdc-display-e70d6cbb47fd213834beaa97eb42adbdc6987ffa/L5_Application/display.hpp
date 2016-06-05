/*
 * master.hpp
 *
 *  Created on: Oct 20, 2014
 *      Author: Roady
 */

#ifndef MASTER_HPP_
#define MASTER_HPP_

#include "printf_lib.h"
#include <stdio.h>
#include<iostream>
#include <stdint.h>
#include "subscription_task.hpp"
#include "scheduler_task.hpp"
#include "io.hpp"
#include <string.h>
#include <string>
#include "utilities.h"
#include "soft_timer.hpp"
#include "command_handler.hpp"
#include "wireless.h"
#include "char_dev.hpp"
#include "FreeRTOS.h"
#include "semphr.h"
#include "uart2.hpp"
#include "uart_dev.hpp"


typedef enum {
 SNR =0,
 GPS=1,
 MTR=2,
 ADROID=3
}
cnt_board;

   class canDataSendTask : public scheduler_task
    {
        public:
           canDataSendTask(uint16_t periodMs, uint8_t priority,const char* taskName,uint32_t stackSize);

            bool run(void *p);

        private:
            const uint16_t mTaskPeriodMs;
            uint8_t mCanData[8];
            uint8_t mCanDatasize;
            can_msg_t mCanMsg;
            void logCANMessageTransmit();

    };


//void display_on_lcd();

static void  subscription_add(can_msg_t* msg);


class canMsgRecieveTask : public scheduler_task
   {
       public:
           canMsgRecieveTask(uint16_t periodMs,uint8_t priority,const char* taskName,uint32_t stackSize);

           bool run(void *p);

       private:
           const uint16_t mTaskPeriodMs;
           can_msg_t mmsg;
           void logCANMessageRecieve();
   };

class LCDTask : public scheduler_task
   {
       public:
           LCDTask(uint16_t periodMs,uint8_t priority,const char* taskName,uint32_t stackSize);

           bool run(void *p);

       private:
           const uint16_t mTaskPeriodMs;

           void display_on_lcd();
   };
#endif /* MASTER_HPP_ */
