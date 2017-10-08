/*******************************************************************************
*
*  sping.c -    A program implementing the the coffee machine tutorial Example
*  @author Andres Sebastian Rudqvist
*  Based on the work of the profesor Eugenio Tamura
*   Notes:                Error checking omitted...
*
*******************************************************************************/

#include "contiki.h"
#include <stdio.h>
#include <inttypes.h>

#include "contiki-net.h"
#include "rest-engine.h"

#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "sys/etimer.h"

#include "coffeeMachine.h"


/**************** Coap Restfull *******************************************************/
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding
 * sub-directory.
 */
extern resource_t
res_hello,
res_toggle,
resCoin,
resCoffee,
resTea,
cup_event;



/**************** process *******************************************************/
PROCESS (pUserInterface, "User Interface");
PROCESS (pController,    "Controller");
PROCESS (pHardware,      "Hardware Simulation");
PROCESS (pTimer,		 "Timer");

AUTOSTART_PROCESSES (&pUserInterface, &pController, &pHardware, &pTimer);

/**************** signals *******************************************************/
/* Signals from UI */
process_event_t sCoffee, sTea, sCoin;
/* Signals to Hardware */
process_event_t sFillWater, sFillCoffee, sHeatWater;
/* Signals from Hardware */
process_event_t sWaterOk, sCoffeeOk, sWarm;
/* Signals from Timer */
process_event_t sSetTimer, sResetTimer;
/* to the customer */
process_event_t sCupOfCoffee, sCupOfTea, sChange, sMoreCoins;

/*---------------------------------------------------------------------------*/
void allocSignals ()
{
	printf("start to alloc signals\n");
	sCoffee = process_alloc_event();
	sCoin   = process_alloc_event();
	sTea   = process_alloc_event();
	sFillWater = process_alloc_event();
	sFillCoffee = process_alloc_event();
	sHeatWater = process_alloc_event();
	sWaterOk = process_alloc_event();
	sCoffeeOk = process_alloc_event();
	sWarm = process_alloc_event();
	sCupOfCoffee = process_alloc_event();
	sCupOfTea   = process_alloc_event();
	sChange  = process_alloc_event();
	sMoreCoins  = process_alloc_event();
}
/*---------------------------------------------------------------------------*/
static void
print_radio_values(void)
{
	radio_value_t aux;

	printf("\n* Radio parameters:\n");

	NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &aux);
	printf("   Channel %u", aux);

	NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MIN, &aux);
	printf(" (Min: %u, ", aux);

	NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MAX, &aux);
	printf("Max: %u)\n", aux);

	NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &aux);
	printf("   Tx Power %3d dBm", aux);

	NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MIN, &aux);
	printf(" (Min: %3d dBm, ", aux);

	NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &aux);
	printf("Max: %3d dBm)\n", aux);

	/* This value is set in contiki-conf.h and can be changed */
	printf("   PAN ID: 0x%02X\n", IEEE802154_CONF_PANID);
}
/*---------------------------------------------------------------------------*/


/**************** process code ************************************************/
/* pUserInterface process
* UI Rest
*/
PROCESS_THREAD (pUserInterface, ev, data)
{

	PROCESS_BEGIN();

	allocSignals();

	PROCESS_PAUSE();

	/* Initialize the REST engine. */
	rest_init_engine();

	// rest_activate_resource(&res_hello, "test/hello");
	// rest_activate_resource(&res_toggle, "actuators/toggle");
	rest_activate_resource(&resCoin,    "ui/coin");
	rest_activate_resource(&resCoffee,    "ui/coffee");
	rest_activate_resource(&resTea,    "ui/tea");
	rest_activate_resource(&cup_event,    "ui/drink");


	printf("\nCoAP Coffee server started\n");
	print_radio_values();

	while (1) {
		/* Wait forever */
		PROCESS_YIELD();

		if (ev == sCupOfCoffee) {
			/* Call the event_handler for this application-specific event. */
			printf("A cup of coffee ready to drink\n");
			cup_event.trigger();
		}
		else if (ev == sCupOfTea) {
			printf("A cup of tea ready to drink\n");
			cup_event.trigger();
		}
	}


	PROCESS_END ();
}

/************************ pController process ******************************************************/
PROCESS_THREAD (pController, InSignal, data)
{
	static PCONTROLLER_STATES  state,
	       state_next;
	static uint8_t nbrOfCoffee = 0;
	static uint8_t nbrOfTea = 0;
	static uint8_t value;
	static uint8_t userChoice;
	//static COINS coin; 

	PROCESS_BEGIN();

	state_next = Idle;
	while ( 1 )
	{
		PROCESS_YIELD ();

		state = state_next;
		value = *((uint8_t *)data);

		switch ( state )
		{
		case Idle:
			if (InSignal == sCoin)
			{
				//coin = (COINS)value;
				printf("pController: sCoin(%d)\n", value);
				if (value == 10) {
					state_next = PaidTen;
					printf("pController: Paid Ten\n");
				}
				else if (value == 5) {
					state_next = PaidFive;
					printf("pController: Paid Five\n");
				}
				else {
					state_next = Idle;
				}

			}
			break;
		case PaidTen:
			if ( InSignal == sCoffee )
			{
				printf("pController: sCoffee\n");
				process_post (&pHardware, sFillWater, NULL);
				userChoice = Coffee;
				state_next = MakingCoffeeOrTea;
				printf("pController: Making Coffee\n");
			}
			if ( InSignal == sTea )
			{
				printf("pController: sTea\n");
				process_post (&pHardware, sFillWater, NULL);
				userChoice = Tea;
				state_next = MakingCoffeeOrTea;
				process_post(&pHardware, sChange, NICKEL);
				printf("pController: Making Tea\n");
			}
		case PaidFive:
			if ( InSignal == sTea )
			{
				printf("pController: sTea\n");
				process_post (&pHardware, sFillWater, NULL);
				userChoice = Tea;
				state_next = MakingCoffeeOrTea;
			}
			if (InSignal == sCoffee )
			{
				printf("pController: sCoffee\n");
				process_post (&pHardware, sMoreCoins, NULL);
			}


		case MakingCoffeeOrTea:
			if ( InSignal == sWaterOk )
			{
				printf("pController: sWaterOk\n");
				if (userChoice == Coffee)
				{
					process_post (&pHardware, sFillCoffee, NULL);
					state_next = WaterForCoffee;
					printf("pController: Water For Coffee\n");
				}
				else if (userChoice == Tea)
				{
					process_post (&pHardware, sHeatWater, NULL);
					state_next = Warming;
				}
				else
				{
					state_next = Idle;
				}
			}
		case WaterForCoffee:
			if ( InSignal == sCoffeeOk )
			{
				printf("pController: sCoffeeOk\n");
				process_post (&pHardware, sHeatWater, NULL);
				state_next = Warming;
				printf("pController: Warming\n");
			}
		case Warming:
			if ( InSignal ==  sWarm)
			{
				printf("pController: sWarm\n");
				if (userChoice == Coffee)
				{
					process_post (&pUserInterface, sCupOfCoffee, NULL);
					state_next = Idle;
					nbrOfCoffee ++;
					printf("pController: coffee Ready (%d)\n", nbrOfCoffee);
				}
				else if (userChoice == Tea)
				{
					process_post (&pUserInterface, sCupOfTea, NULL);
					state_next = Idle;
					nbrOfTea ++;
					printf("pController: tea Ready (%d)\n", nbrOfTea);
				}
				else
				{
					state_next = Idle;
				}
			}
		default:
			printf("pController - unknown state\n");
			break;
		}
	}



	PROCESS_END ();
}

/************************ pHardware process ******************************************************/
PROCESS_THREAD (pHardware, InSignal, data)
{

	static PHARDWARE_STATES  state,
	       state_next;

	PROCESS_BEGIN();

	state_next = IdleH;

	while ( 1 )
	{
		PROCESS_YIELD ();

		state = state_next;

		switch ( state )
		{
		case Idle:
			if (InSignal == sFillWater)
			{
				printf("pHardware: post sWaterOk\n");
				process_post (&pController, sWaterOk, NULL);
				state_next = IdleH;

			}
			if (InSignal == sFillCoffee)
			{
				printf("pHardware: post sCoffee Ok\n");
				process_post (&pController, sCoffeeOk, NULL);
				state_next = IdleH;
			}
			if (InSignal == sHeatWater)
			{
				printf("pHardware: post sWater\n");
				OutSignal = (uint8_t) sSetTimer;
				value = 10;
				process_post (&pTimer, OutSignal, &value);
				state_next = Warming;
			}
			break;
		case WarmingH:
			if (InSignal == Heater_et)
			{
				process_post (&pController, sWarm, NULL);
				state_next = IdleH;
			}
			break;
		default:
			printf("pHardware - unknown state\n");
			break;
		}
	}

	PROCESS_END ();
}

/************************ pTimer process ******************************************************/

PROCESS_THREAD ( pTimer, InSignal, data ) {
	static PTIMER_STATES state,
	       state_next;
	static struct etimer Heater_et;
	static uint8_t OutSignal,
					value;
	PROCESS_BEGIN ();
	state_next = IdleT;
	for ( ; ; ) 
	{
		PROCESS_YIELD ();
		state = state_next;
		/* get value */
		value = *( (uint8_t *) data );
		switch ( state ) 
		{
	      case IdleT:
	        if ( InSignal == sSetTimer)
	        {
	            /* get value */
	            printf("pTimer 1: &value = %p\n",data);
	            printf("pTimer 1: value = %d\n",(*(uint8_t*)data));
	            value = *((uint8_t *)data);

	            printf("Timer set to %u secods\n",value);
	            printf("pTimer 2: &valule = %p\n",&value);
	            printf("pTimer 2: &valule = %d\n",value);
	            
	            etimer_set (&Heater_et, CLOCK_SECOND * value);
	            state_next = WaitTimeout;
	        }
	        break;
	      case WaitTimeout:
	        if ( InSignal == PROCESS_EVENT_TIMER)
	        {
	            value = 0;
	            printf("Timer timeout (send signal to pHardware)\n");
	            process_post (&pHardware, sT, &value);
	            printf("sT\n");
	            state_next = IdleT;
	        } else if ( InSignal == sResetTimer) {
	            printf("Timer reset\n");
	            etimer_stop (&Heater_et);
	            state_next = IdleT;
	        }
	        break;
	      default:
	        printf("pTimer: unknown state");
	        break;
	    }
	}
	PROCESS_END ();
}
