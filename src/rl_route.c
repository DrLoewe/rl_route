#include <pebble.h>
#include "window_waiting.h"
#include "appmessage.h"
//#include "route_step_stor.h"
#include "route_steps_window.h"

// debug application?
#define DEBUG

static Window *window;
static struct RouteSteps *route_step_stor;

void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
#ifdef DEBUG
  APP_LOG(APP_LOG_LEVEL_DEBUG, "dropped, reason: %d", reason);
#endif
}

void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *tuple;
  if ( (tuple = dict_find(received, APPMESSAGE_KEY_ROUTESTEP_INDEX)) ) {

    // purge the stor if first element received
    if (tuple->value->int8 == 0)
      route_step_stor_purge(route_step_stor);
		
    route_step_stor_add(route_step_stor, received);
		
		if (!route_steps_window_is_loaded()) {
			window_stack_pop_all(false);
			route_steps_window_show();
		} else {
			route_steps_window_reload_data();
		}
		
  } else if ( (tuple = dict_find(received, APPMESSAGE_KEY_UPDATE_INDEX)) ) {
		route_step_stor->current_step = tuple->value->int8;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "current route step index: %d", route_step_stor->current_step);
		if ( (tuple = dict_find(received, APPMESSAGE_KEY_UPDATE_DISTANCE)) ) {
			strncpy(route_step_stor->current_distance, tuple->value->cstring, tuple->length);
		}
		route_steps_window_update();
	}
	
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
  APP_LOG(APP_LOG_LEVEL_DEBUG, "appmessage sent ok");
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
  APP_LOG(APP_LOG_LEVEL_DEBUG, "appmessage sent FAILED!");
}

static void init(void) {
  // display the "waiting for data.." window and start the communication
  window = window_waiting_create();
  const bool animated = true;
  window_stack_push(window, animated);

  // init the storage for the route steps
  route_step_stor = route_step_stor_create();

  // init the route steps window
  route_steps_window_init(route_step_stor);

  // AppMessage Settings and Constants
  // AppMessage OUT: action (1byte) + value (1byte) = 1 + 2*7 + 1 + 1 = 17 bytes buffer size

#define APP_MESSAGE_OUTBOX_SIZE 64

  // open app message with the desired output buffer size and the minimum
  // guaranteed size for incoming messages

#define APP_MESSAGE_INBOX_SIZE 512

  app_message_open(APP_MESSAGE_INBOX_SIZE, APP_MESSAGE_OUTBOX_SIZE);

  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_inbox_received(in_received_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "trying to send an appmessage ..");
  // request the route name from ios app
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(APPMESSAGE_REQUEST_ROUTE, 0);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void deinit(void) {
  window_destroy(window);
  route_steps_window_deinit();
  route_step_stor_destroy(route_step_stor);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}