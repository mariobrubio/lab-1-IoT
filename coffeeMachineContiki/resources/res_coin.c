/**
 * \file
 *      Coffee Machine example - coffee
 * \author
 *      Andres Rudqvist <asrudqvist@javerianacali.edu.co>
 * Based on
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include "contiki.h"
#include <string.h>
#include "contiki.h"
#include "rest-engine.h"

#include "coffeeMachine.h"

extern process_event_t sCoin; 
extern struct process pController;

static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* Receives coins */
RESOURCE(resCoin,
         "title=\"Receive Coin\";rt=\"Signal\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char * coinName = NOCOIN;
  //COINS coin ;
  static uint8_t value;
  REST.get_query_variable(request, "coin", &coinName);
  printf("coin = %s\n",coinName);

  if (strncmp("NICKEL",coinName,6) == 0)
  {
  	//coin = NICKEL;
    value = 5;
  	process_post (&pController, sCoin, &value);
  	printf("post sCoin(NICKEL)\n");
  } else  if (strncmp("DIME",coinName,4) == 0)
  {
  	//coin = DIME;
    value = 10;
  	process_post (&pController, sCoin, &value);
  	printf("post sCoin(DIME)\n");
  }  else {
	REST.set_response_status(response, REST.status.BAD_REQUEST);
	printf("error unknown coin\n");
  }

}
