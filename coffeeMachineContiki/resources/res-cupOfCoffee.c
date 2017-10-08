/**
 * \file
 *      Coffee Machine example - cup of coffee
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

/*extern process_event_t sCupOfCoffee; 
extern struct process pController;*/

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

/* coffee , tea etc*/
EVENT_RESOURCE(cup_event,
               "title=\"Drinks\";obs",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

/*
 * Use local resource state that is accessed by res_get_handler() and altered by res_event_handler() or PUT or POST.
 */
static int32_t coffee_counter = 0;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Coffee Cups %u", coffee_counter));

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}
/*
 * Additionally, res_event_handler must be implemented for each EVENT_RESOURCE.
 * It is called through <res_name>.trigger(), usually from the server process.
 */
static void
res_event_handler(void)
{
  /* Do the update triggered by the event here, e.g., sampling a sensor. */
  ++coffee_counter;

  /* Usually a condition is defined under with subscribers are notified, e.g., event was above a threshold. */
  if(1) {
    printf("Coffee cups %u for /%s\n", coffee_counter, cup_event.url);

    /* Notify the registered observers which will trigger the res_get_handler to create the response. */
    REST.notify_subscribers(&cup_event);
  }
}
