#include <pebble.h>
#include "appmessage.h"
#include "route_step_stor.h"

struct RouteSteps* route_step_stor_create() {
  struct RouteSteps *stor = malloc(sizeof(struct RouteSteps));
  stor->count = 0; // init stations count
  return stor;
}

void route_step_stor_add(struct RouteSteps *stor, DictionaryIterator *received) {
  if (stor->count == MAX_ITEMS) return;

  Tuple *instructions = dict_find(received, APPMESSAGE_KEY_ROUTESTEP_INSTRUCTIONS);
  Tuple *distance = dict_find(received, APPMESSAGE_KEY_ROUTESTEP_DISTANCE);
  
  if (distance && instructions) {
    stor->data[stor->count].distance = (char*)malloc(distance->length);
    strncpy(stor->data[stor->count].distance, distance->value->cstring, distance->length);
    
    stor->data[stor->count].instructions = (char*)malloc(instructions->length);
    strncpy(stor->data[stor->count].instructions, instructions->value->cstring, instructions->length);

    /* APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", */
	  /*   stor->data[stor->count].distance); */
    stor->count++;
  }
}

// purge the stor
void route_step_stor_purge(struct RouteSteps *stor) {
  while(stor->count > 0) {
    stor->count--;
    free(stor->data[stor->count].distance);
    free(stor->data[stor->count].instructions);
  }
}

void route_step_stor_destroy(struct RouteSteps *stor) {
  route_step_stor_purge(stor);
  free(stor);
}
