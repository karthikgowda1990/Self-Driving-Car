/*
 * master.c
pp
 *
 *  Created on: Oct 20, 2014
 *      Author: Roady
 */

#include "display.hpp"
static volatile bool start_command =true;
static volatile bool stop_command =false;
uint8_t sensor_data[8]={0};
uint16_t compass_data[4]={0};
uint64_t android_data[3]={0};
double d_AndroidLongitudeEntry = 0.00;
double d_AndroidLatitudeEntry = 0.00;
uint8_t motor_data[8]={0};
uint8_t master_data[8]={0};
uint16_t CAN_count = 0;
canDataSendTask::canDataSendTask(uint16_t periodMs, uint8_t priority,const char* taskName,uint32_t stackSize) :
                scheduler_task(taskName,stackSize, priority),
                mTaskPeriodMs(periodMs),mCanDatasize(0)
            {
                setRunDuration(periodMs);
            }


LCDTask::LCDTask(uint16_t periodMs, uint8_t priority,const char* taskName,uint32_t stackSize) :
                scheduler_task(taskName,stackSize, priority),
                mTaskPeriodMs(periodMs)
            {
                setRunDuration(periodMs);

            }

void canMsgRecieveTask::logCANMessageRecieve()
{
    static bool oldLEDValue = false;

    if(oldLEDValue == false)
           LE.on(2);
    else
           LE.off(2);

           oldLEDValue = !oldLEDValue;
    uint16_t canmsgDropCount = CAN_get_rx_dropped_count(can1);

     if(canmsgDropCount)
        LE.on(4);
     else
        LE.off(4);
}


void canDataSendTask::logCANMessageTransmit()
{
   static uint32_t canPreviousMsgCount=0;
   static bool oldLEDValue = false;
   uint32_t msgCount = GetCanMsgCount();

   if(canPreviousMsgCount != msgCount)
   {
       if(oldLEDValue == false)
       LE.on(3);
       else
       LE.off(3);

       oldLEDValue = !oldLEDValue;

       canPreviousMsgCount=msgCount;
   }
}

uint8_t subscribeSendCount=0;
const uint8_t MAXSUBSCRIBESENDCOUNT=10;
bool canDataSendTask::run(void *p)
{

    if(start_command==true && subscribeSendCount<MAXSUBSCRIBESENDCOUNT)
    //if(SW.getSwitch(1))
    {
        ++subscribeSendCount;

    puts("Sending CAN subscription message to GPS\n");

    uint8_t subscribems = 1;

     memset(&mCanMsg, 0, sizeof(mCanMsg));

     memset(&mCanData, 0, sizeof(mCanData));
     mCanData[0]=subscribems;
     // 0the member of data array has subscription message frequency
     mCanDatasize=1;

     mCanMsg=CreateCANMessage(0,0x0515444A,1,1,(const char*)mCanData);
     CAN_tx(can1, &mCanMsg, 0);


    //else if(SW.getSwitch(2)){

        puts("Sending CAN subscription message to sensor\n");


         memset(&mCanMsg, 0, sizeof(mCanMsg));

         memset(&mCanData, 0, sizeof(mCanData));
         mCanData[0]=subscribems;
         // 0the member of data array has subscription message frequency
         mCanDatasize=1;

         mCanMsg=CreateCANMessage(0,0x0535444A,1,1,(const char*)mCanData);
         CAN_tx(can1, &mCanMsg, 0);

         start_command=false;
         stop_command=false;

         logCANMessageTransmit();

         }
    else if(stop_command==true)
    {
        start_command=false;
        stop_command=false;
    }
     return true;
    }

char arr[17]={0};
char arr1[28]={0};
//char arr2[28]={0};
int j;
void uint8_to_char(uint8_t data)
{

    uint8_t num;

    char t[5];
    num = data;
    int i=0;
    do
      {

       uint8_t temp;
       temp=num%10;
       num=num/10;
       t[i]=temp+'0';
       i++;
      }
    while(num!=0);
      t[i]='\0';
      i--;

    while(i>=0)
      {
         arr[j]=t[i];
         i--;
         j++;
      }
     arr[j]=' ';
     j++;
     arr[j]='\0';
     //return arr;
}

 void uint16_to_char(uint16_t data)
{

    uint16_t num=0;
    char t[7]={0};
    num = data;
    int i=0;
    uint16_t temp=0;
    do
      {
       temp=num%10;
       num=num/10;
       t[i]=temp+'0';
       i++;
      }
    while(num!=0);
      t[i]='\0';
      i--;
    while(i>=0)
      {
         arr1[j]=t[i];
         i--;
         j++;
      }

     printf("In uint_func arr1 %s\n",arr1);

}
/*
 void uint64_to_char(uint64_t data)
{

    uint64_t num=0;
    char t[7]={0};
    num = data;
    int i=0;
    uint64_t temp=0;
    do
      {
       temp=num%10;
       num=num/10;
       t[i]=temp+'0';
       i++;
      }
    while(num!=0);
      t[i]='\0';
      i--;
    while(i>=0)
      {
         arr2[j]=t[i];
         i--;
         j++;
      }

     printf("In uint_func arr1 %s\n",arr2);

}
*/



static void  subscription_add(can_msg_t* msg)
{
    if(msg==NULL)return;

    uint8_t srcId = ((*msg).msg_id>>CAN_MESSAGEBITS) & (0xffU);
    uint16_t msgId = (*msg).msg_id & 0xfffU | 0x00000100;//changing subscription message to subscribed message

#ifdef DEBUG_SUBSCRIPTION_TASK
    printf("message id is: %x,src id is: %x\n ",(*msg).msg_id, srcId);
#endif

    msgRate_t msgRate=msgRate1Hz;//convertHzTomsdRateEnum((*msg).data.bytes[0]);
    canSubscribedMsgTask::addSubscribedMsg(msgRate,srcId,msgId, msg);

}


canMsgRecieveTask::canMsgRecieveTask(uint16_t periodMs,uint8_t priority,const char* taskName,const uint32_t stackSize) :
            scheduler_task(taskName,stackSize, priority),
            mTaskPeriodMs(periodMs)
    {
            setRunDuration(periodMs);
    }

bool Publish_data_display(can_msg_t& mmsg)
{

    uint8_t srcId = ((mmsg).msg_id>>CAN_MESSAGEBITS) & (0xffU);
    uint16_t msgId = (mmsg).msg_id & 0xfffU;


    if(msgId == mid_sensor_publish_data_display && srcId == cid_sensor_controller )
       {
        #ifdef DEBUG_SUBSCRIPTION_TASK
            for(int i=0;i<8;i++)
                sensor_data[i]=mmsg.data.bytes[i];
        #endif
        }

    else if(msgId == mid_geo_publish_data_getdisplayData && srcId == cid_geographical_controller )
        {
         #ifdef DEBUG_SUBSCRIPTION_TASK
        for(int i=0;i<4;i++)
           compass_data[i]=mmsg.data.words[i];

         #endif
         }
    else if(msgId == mid_android_controllercmd_gpsDestination_eom && srcId == cid_andriod_controller )
         {
          #ifdef DEBUG_SUBSCRIPTION_TASK
          #endif
         }
    else if(msgId == mid_android_controllercmd_gpsDestination_longitude && srcId == cid_andriod_controller )
         {
           #ifdef DEBUG_SUBSCRIPTION_TASK
            memcpy((char *) &(d_AndroidLongitudeEntry), (char*) &(mmsg.data.qword), 8);
           #endif
         }
    else if(msgId == mid_android_controllercmd_gpsDestination_latitude && srcId == cid_andriod_controller )
         {
           #ifdef DEBUG_SUBSCRIPTION_TASK
            memcpy((char *) &(d_AndroidLatitudeEntry), (char*) &(mmsg.data.qword), 8);
           #endif
         }
    else if(msgId == mid_master_controllercmd_start && srcId == cid_master_controller )
         {
            #ifdef DEBUG_SUBSCRIPTION_TASK
                start_command=true;
                LE.on(1);
            #endif
         }
    else if(msgId == mid_master_controllercmd_stop && srcId == cid_master_controller )
         {
            #ifdef DEBUG_SUBSCRIPTION_TASK
               stop_command=true;
               LE.off(1);
            #endif
         }
    return true;

}

uint8_t boardindex=SNR;
bool sensor_flag=false;
bool gps_flag=false;
bool android_flag=false;
bool LCDTask::run(void *p)
{
    LD.setNumber(CAN_count);

    Uart2 &u2=Uart2::getInstance();
    uint16_t before_dec=0;
    uint16_t after_dec=0;
    u2.init(38400);
    u2.putChar(0xF0);
    //u2.putline("$CLR_SCR");

    //switch(boardindex)
    //{
    //case 0:
    if((LPC_GPIO1->FIOPIN & (1 << 9)))
    {
        sensor_flag=true;
        gps_flag=false;
        android_flag=false;
    }
    else if((LPC_GPIO1->FIOPIN & (1 << 10)))
    {
        gps_flag=true;
        sensor_flag=false;
        android_flag=false;
    }
    else if((LPC_GPIO1->FIOPIN & (1 << 14)))
    {
        android_flag=true;
        gps_flag=false;
        sensor_flag=false;
    }
    else if((LPC_GPIO1->FIOPIN & (1 << 15)))
    {
        android_flag=false;
        gps_flag=false;
        sensor_flag=false;
    }
    if(sensor_flag==true)
    {
        u2.putline("$CLR_SCR");
        printf("Case 1\n");
        j=0;
        u2.printf("$L:0:Sensor data:\n");
        for(int k=0;k<3;k++)
            uint8_to_char(sensor_data[k]);
        u2.printf("$L:1:%s  cms\n",arr);

        j=0;
       /* for(int k=3;k<6;k++)
            uint8_to_char(sensor_data[k]);
        u2.printf("$L:2:%s  cms\n",arr);
        */
        int k;
        k=7;
        uint8_to_char(sensor_data[k]);
        k=5;
        uint8_to_char(sensor_data[k]);
        k=3;
        uint8_to_char(sensor_data[k]);
        u2.printf("$L:2:%s  cms\n",arr);
        j=0;
        uint16_to_char(CAN_count);

        u2.printf("$L:3:Can msg count: %s\n",arr1);
        CAN_count=0;
    }
       // break;
    //case 1:

    else if(gps_flag==true)
    {
        u2.putline("$CLR_SCR");
        printf("Case 2\n");
        u2.printf("$L:0:GPS Data:\n");
        j=0;
        for(int k=0;k<2;k++)
        {
            before_dec=compass_data[k]/10;
            after_dec=compass_data[k]%10;

            uint16_to_char(before_dec);
            arr1[j]='.';
            j++;
            uint16_to_char(after_dec);
            arr1[j]=' ';
            j++;
            printf("compass_data - %d",compass_data[k]);
            before_dec=0;
            after_dec=0;
        }
        arr1[j]='\0';
        printf("arr2=%s\n",arr1);
        u2.printf("$L:1:%s degrees\n",arr1);
        j=0;
        uint16_to_char(CAN_count);
        arr1[j]=' ';
        j++;
        arr1[j]='\0';
        u2.printf("$L:3:Can msg count: %s\n",arr1);
        CAN_count=0;
    }


    else if(android_flag==true)
    {
       u2.putline("$CLR_SCR");
       u2.printf("$L:0:Android Data:\n");
       char latitude[20];
       char longitude[20];
       snprintf(latitude,20,"%3.6f",d_AndroidLatitudeEntry);
       snprintf(longitude,20,"%3.6f",d_AndroidLongitudeEntry);
       u2.printf("$L:1:Latitude:%s\n",latitude);
       u2.printf("$L:2:Longitude:%s\n",longitude);
       j=0;
       uint16_to_char(CAN_count);
       arr1[j]=' ';
       j++;
       arr1[j]='\0';
       u2.printf("$L:3:Can msg count: %s\n",arr1);
       CAN_count=0;
    }
    else
    {

        u2.putline("$CLR_SCR");

        switch(boardindex)
        {
            case 0:
                j=0;
                u2.printf("$L:0:Sensor data:\n");
                for(int k=0;k<3;k++)
                    uint8_to_char(sensor_data[k]);
                u2.printf("$L:1:%s  cms\n",arr);

                j=0;
                int k;
                k=7;
                uint8_to_char(sensor_data[k]);
                k=5;
                uint8_to_char(sensor_data[k]);
                k=3;
                uint8_to_char(sensor_data[k]);
                u2.printf("$L:2:%s  cms\n",arr);
                j=0;
                uint16_to_char(CAN_count);

                u2.printf("$L:3:Can msg count: %s\n",arr1);
                CAN_count=0;
                break;


        case 1:
            u2.putline("$CLR_SCR");
            printf("Case 2\n");
            u2.printf("$L:0:GPS Data:\n");
            j=0;
            for(int k=0;k<2;k++)
            {
                before_dec=compass_data[k]/10;
                after_dec=compass_data[k]%10;

                uint16_to_char(before_dec);
                arr1[j]='.';
                j++;
                uint16_to_char(after_dec);
                arr1[j]=' ';
                j++;
                printf("compass_data - %d",compass_data[k]);
                before_dec=0;
                after_dec=0;
            }
            arr1[j]='\0';
            printf("arr2=%s\n",arr1);
            u2.printf("$L:1:%s degrees\n",arr1);
            j=0;
            uint16_to_char(CAN_count);
            arr1[j]=' ';
            j++;
            arr1[j]='\0';
            u2.printf("$L:3:Can msg count: %s\n",arr1);
            CAN_count=0;
            break;
        case 2:
            u2.putline("$CLR_SCR");
            u2.printf("$L:0:Android Data:\n");
            char latitude[20];
            char longitude[20];
            snprintf(latitude,20,"%3.6f",d_AndroidLatitudeEntry);
            snprintf(longitude,20,"%3.6f",d_AndroidLongitudeEntry);
            u2.printf("$L:1:Latitude:%s\n",latitude);
            u2.printf("$L:2:Longitude:%s\n",longitude);
            j=0;
            uint16_to_char(CAN_count);
            arr1[j]=' ';
            j++;
            arr1[j]='\0';
            u2.printf("$L:3:Can msg count: %s\n",arr1);
            CAN_count=0;
            break;
        default:
            printf("Case 5\n");
            u2.printf("$L:0:Displaying data:\n");
            j=0;
            uint16_to_char(CAN_count);
            arr1[j]=' ';
            j++;
            arr1[j]='\0';
            u2.printf("$L:3:Can msg count: %s\n",arr1);
            CAN_count=0;
            break;

    };
    boardindex=(boardindex+1)%3;
    }

  //      break;
        /*
    case 2:
        j=0;
        uint16_to_char(CAN_count);
        arr1[j]=' ';
        j++;
        arr1[j]='\0';
        u2.printf("$L:3:Can message count: %s\n",arr1);
        CAN_count=0;

        break;

    case 3:
        j=0;
        printf("Case 3\n");
        j=0;
        uint16_to_char(CAN_count);
        arr1[j]=' ';
        j++;
        arr1[j]='\0';
        u2.printf("$L:3:Can message count: %s\n",arr1);

        CAN_count=0;
        break;

    case 4:
        printf("Case 4\n");
        u2.printf("$L:0:Android data:\n");
        j=0;
        uint16_to_char(CAN_count);
        arr1[j]=' ';
        j++;
        arr1[j]='\0';
        u2.printf("$L:3:Can message count: %s\n",arr1);
        CAN_count=0;
        break;
    default:
        printf("Case 5\n");
        u2.printf("$L:0:Displaying data:\n");
        j=0;
        uint16_to_char(CAN_count);
        arr1[j]=' ';
        j++;
        arr1[j]='\0';
        u2.printf("$L:3:Can message count: %s\n",arr1);
        CAN_count=0;
        break;
    };

   boardindex=(boardindex+1)%3;
*/
    return true;
}
bool canMsgRecieveTask::run(void *p)
{
    if(CAN_rx(can1,&mmsg,portMAX_DELAY))
    {
        ++CAN_count;
        printf("CAN count = %d\n",CAN_count);

        logCANMessageRecieve();
        switch(mmsg.msg_id&(0xfffU))
        {
            case 0x300 ... 0x3FF:
#ifdef DEBUG_SUBSCRIPTION_TASK
            Publish_data_display(mmsg);
#endif
            break;
            case 0x400 ... 0x4FF:
            {
                can_msg_t* msg= new can_msg_t;

                if(msg==NULL)
                    return false;
#ifdef DEBUG_SUBSCRIPTION_TASK
            puts("DISPLAY: CAN subscription message got");
            printf(" with message id:%x, %x\n",(mmsg.msg_id),(mmsg.msg_id&0xfffU) );
#endif

            memset(msg, 0, sizeof(msg));
            *msg=mmsg;
            subscription_add(msg);

            break;
            }

            case 0x500 ... 0x5FF:
#ifdef DEBUG_SUBSCRIPTION_TASK
            Publish_data_display(mmsg);

            puts("DISPLAY: CAN subscribed message got");

            printf(" with message id:%x, %x\n",(mmsg.msg_id),(mmsg.msg_id&0xfffU) );

#endif
            break;

            default:
               // delete msg;
#ifdef DEBUG_SUBSCRIPTION_TASK
                printf("MASTER: Unexpected message id got(master):%x, %x\n",(mmsg.msg_id),(mmsg.msg_id&0xfffU) );
#endif
                break;
        };

    }

    return true;
}
