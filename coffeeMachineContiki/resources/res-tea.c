/**
 * \file
 *      Coffee Machine example - tea
 * \author
 * 		Mario Bolaños <mandres1@javerianacali.edu.co>
 * Based on
 *      Andres Rudqvist <asrudqvist@javerianacali.edu.co> 
 */

#include "contiki.h"
#include <string.h>
#include "contiki.h"
#include "rest-engine.h"

#include "coffeeMachine.h"

extern process_event_t sTea; 
extern struct process pController;

static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* Ask for tea */
RESOURCE(resTea,
         "title=\"Ask for tea\";rt=\"Signal\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  	process_post (&pController, sTea, NULL);
  	printf("post sTea\n");
}
