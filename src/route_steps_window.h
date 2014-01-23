#include <pebble.h>
#include "route_step_stor.h"

typedef void (*SendRequestCallback)(void);
void route_steps_window_init(struct RouteSteps *steps, SendRequestCallback callback);
void route_steps_window_show(void);
void route_steps_window_deinit(void);
void route_steps_window_reload_data(void);
bool route_steps_window_is_loaded(void);
void route_steps_window_update(bool current_step_changed);

