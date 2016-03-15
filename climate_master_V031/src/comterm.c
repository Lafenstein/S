
#include "comterm.h"
#include "sys/queue.h"

#define MAILBOXSIZE 256


thread_t *tp_W = NULL;  // can send task pointer
uint8_t *temda[];

static THD_WORKING_AREA(serialListen, 128);/*Thread area for rxListen*/
static THD_WORKING_AREA(serialTalk, 128);/*Thread area for txWrite*/

int32_t txMailboxArea[MAILBOXSIZE];/*Mailbox area for sending (uint8_t) objects */

mailbox_t serialMbox;/*mailbox name*/


uint8_t a;
uint8_t *head;

struct pizza_t{
    int order;
    int pizza;
    TAILQ_ENTRY(pizza_t) entries;
};


static THD_FUNCTION (rxListen, arg)
{
	(void)(arg);
	uint8_t buffer;
	while(!0)
	{
        /* Read arg3 bytes from device pointed by arg1(SD3 for this example)
		 * and write data to arg2(buffer for this example). Type of buffer
		 * should be (uint8_t *). This function blocks this thread until
		 * desired number of bytes have read*/

		//sdRead(&SD3, &header, 9);
		sdRead(&SD3, &buffer, 1);
		/* Post contents of arg2 to mailbox pointed by arg1, wait to post maximum arg3 mSeconds
		 * arg3 may be TIME_IMMEDIATE(post if you can) or TIME_INFINITE(wait until post).
		 * If mailbox object is full, function waits for a free slot for arg3 mSeconds */
		chMBPost(&serialMbox, buffer, TIME_INFINITE);
		chThdSleepMilliseconds(10);

	}
}

uint8_t pubA(void){

    uint8_t toSend;

    if(chMBFetch(&serialMbox, (msg_t *)&toSend, TIME_INFINITE) == MSG_OK)
    return toSend;
}

uint8_t pubH(void){
    return *head;
}

void wifiWriteDATA(uint8_t *WDATA[] ){

    *temda = *WDATA;
 // run send can
    chSysLock();
    thread_t *ntp = tp_W;
    if(ntp){
        tp_W = NULL;
        chSchWakeupS(ntp, MSG_OK);
    }
    chSysUnlock();
// auto stop
}

static THD_FUNCTION(txWrite, arg)
{
	(void)(arg);
	while(!0){
            chSysLock();
            tp_W = chThdGetSelfX();
            chSchGoSleepS(CH_STATE_SUSPENDED);
            chSysUnlock();

            sdWrite(&SD3, &temda, strlen(temda));
		/* Get contents of next mail in mailbox pointed by arg1 to arg2
		 * and wait mail arg3 mSeconds if there is no message is present.
		 *
		 * Returns MSG_OK if succesfully a mail fetched from mailbox*/
		//if(chMBFetch(&serialMbox, (msg_t *)&toSend, TIME_INFINITE) == MSG_OK)
	//	{
			/* Write arg3 bytes from arg2 to device pointed by arg1(SD3 for this example).
			 * Type of arg2 should be (uint8_t *) otherwise only first 8 bit will be send.*/
		//	sdWrite(&SD3, &toSend,1);

		//}
		//wifiInit();
		chThdSleepMilliseconds(1000);
	}
}


 static SerialConfig uartCfg =
{
115200, // bit rate
NULL,
};

uint8_t comAT[] = "AT\r\n";
uint8_t comATCWLAP[] = "AT+CWLAP\r\n";     /** avaible neworks search **/
uint8_t comATCWMODE[] = "AT+CWMODE=1\r\n"; /** working mode 1 clinet 2 sender 3 booth **/
uint8_t comAtCIFSR[] =  "AT+CIFSR\r\n";    /** request IP addres **/
uint8_t sep = 0x22;                        /**  ( " ) separetor  **/
uint8_t comATCWJAP[] = "AT+CWJAP=";
uint8_t SSID[] = "Firefly Wifi 2.4GHz";
uint8_t dot[] = ",";
uint8_t password[] = "f1refly-w1f1";
uint8_t end[] = "\r\n";
uint8_t comATCIPMUX[] = "AT+CIPMUX=1\r\n";
uint8_t comATCIPSERVER[] = "AT+CIPSERVER=1,23\r\n";


void telnetReconfig(void){

 sdWrite(&SD3, &comATCIPMUX,strlen(comATCIPMUX));
 chThdSleepMilliseconds(5000);
}

int wifiInit(void){

 sdWrite(&SD3, &comAT,strlen(comAT));

 chThdSleepMilliseconds(10000);

 sdWrite(&SD3, &comATCWMODE,strlen(comATCWMODE));
 chThdSleepMilliseconds(10000);

 //sdWrite(&SD3, &comATCWLAP,strlen(comATCWLAP));
 //chThdSleepMilliseconds(10000);

 sdWrite(&SD3, &comATCWJAP,strlen(comATCWJAP));
 sdWrite(&SD3, &sep,strlen(1));
 sdWrite(&SD3, &SSID,strlen(SSID));
 sdWrite(&SD3, &sep,strlen(1));
 sdWrite(&SD3, &dot,strlen(dot));
 sdWrite(&SD3, &sep,strlen(1));
 sdWrite(&SD3, &password,strlen(password));
 sdWrite(&SD3, &sep,strlen(1));
 sdWrite(&SD3, &end,strlen(end));
  chThdSleepMilliseconds(10000);


 sdWrite(&SD3, &comATCWMODE,strlen(comATCWMODE));
 chThdSleepMilliseconds(10000);

 sdWrite(&SD3, &comATCIPMUX,strlen(comATCIPMUX));
 chThdSleepMilliseconds(10000);

 sdWrite(&SD3, &comATCIPSERVER,strlen(comATCIPSERVER));
 chThdSleepMilliseconds(10000);

 sdWrite(&SD3, &comAtCIFSR,strlen(comAtCIFSR));
}


int wconfInit(void)
{
	thread_t *listener;

	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7)); // used function : USART3_TX
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7)); // used function : USART3_RX
	/* Start the serial driver(change it's state to ready) pointed by arg1 with configurations in arg2
     * if arg2 is NULL then use default configuration in halconf.h*/
	 sdStart(&SD3, &uartCfg);

	/*Initialize mailbox object pointed by arg1, which uses buffer pointed by arg2 and have
	 * arg3 objects*/
	chMBObjectInit(&serialMbox, txMailboxArea, MAILBOXSIZE);

	/* Create a thread uses arg2 of area arg1 with priority of
	 * arg3(LOWPRIO,NORMALPRIO,HIGHPRIO or any value between 0-255), thread function is arg4
	 *
	 * Returns pointer to thread object*/
    listener = chThdCreateStatic(serialListen, sizeof(serialListen), LOWPRIO, rxListen, NULL);

    chThdCreateStatic(serialTalk, sizeof(serialTalk), LOWPRIO, txWrite, NULL);

    /*Wait until thread pointed in arg1 finish*/
	//chThdWait(listener);
}
