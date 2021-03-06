/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 * @note  printf of %f may be turned off to save memory, this can be configured at sys_config.h
 */
#include "FreeRTOS.h"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "uart2.hpp"
#include "uart_dev.hpp"
#include "string.h"
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include "utilities.h"
#include <can.h>
#include "display.hpp"
#include "printf_lib.h"
/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at cpp_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */

int main(void)
{
    InitializeCan();

    CAN_bypass_filter_ack_all_msgs();

    //SetCANFilter(IO_CAN29_EXGRP1START,IO_CAN29_EXGRP1END);


    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    //VECTOR<subscribedMsg_t> *ptr= new VECTOR<subscribedMsg_t>(10);
    addSubscriptionTasks(PRIORITY_MEDIUM);
    //LD.setNumber(CAN_count);
    //scheduler_add_task(new canSubscribedMsgTask  (msgRate1Hz,ptr,PRIORITY_MEDIUM));
    scheduler_add_task(new canMsgRecieveTask  (10,PRIORITY_MEDIUM,"CanMsgRecieveTask",4*512));
    scheduler_add_task(new canDataSendTask  (20,PRIORITY_MEDIUM,"canMsgSendTask",3*512));
    scheduler_add_task(new LCDTask  (500,PRIORITY_MEDIUM,"LCDTask",3*512));//display_on_lcd();
    scheduler_start();

    while(1)
    {

    }

    return -1;
}



    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing "learn" command.
     */
    // scheduler_add_task(new terminalTask(PRIORITY_MEDIUM));
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    //scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you
     * want the terminal task to always be responsive so you can poke around in
     * case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
#if 0
    scheduler_add_task(new example_io_demo());
#endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
#if 0
    scheduler_add_task(new example_task());
    scheduler_add_task(new example_alarm());
    scheduler_add_task(new example_logger_qset());
    scheduler_add_task(new example_nv_vars());
#endif

    /**
     * Try the rx / tx tasks together to see how they queue data to each other.
     */
#if 0
    scheduler_add_task(new queue_tx());
    scheduler_add_task(new queue_rx());
#endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
#if 0
    scheduler_add_task(new producer());
    scheduler_add_task(new consumer());
#endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
#if 0
    Uart3 &u3 = Uart3::getInstance();
    u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
    scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
#endif

    //  scheduler_start(); ///< This shouldn't return
    // return -1;
//}
