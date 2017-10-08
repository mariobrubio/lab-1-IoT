/**
 * \file
 *      Coffee Machine example - coins
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

extern process_event_t sCoffee; 
extern struct process pController;

static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* Ask for coffee */
RESOURCE(resCoffee,
         "title=\"Ask for coffee\";rt=\"Signal\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  	process_post (&pController, sCoffee, NULL);
  	printf("post sCoffee\n");
}
