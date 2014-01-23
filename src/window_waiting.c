#include "pebble.h"
#include "window_waiting.h"

static struct UI {
  Window *window;
  /* TextLayer *text_layer; */
  BitmapLayer *iphone_layer;
  GBitmap *iphone_bmp;
  // Activity Layer
  Layer *activity_layer;
  uint8_t active_point;
} ui;

#define ACTIVITY_NUM_POINTS 3
#define ACTIVITY_RADIUS 6

void activity_layer_update_proc(Layer *layer, GContext *ctx) {
  for (uint8_t i=0; i<ACTIVITY_NUM_POINTS; i++) {
    GPoint point = GPoint(i*24+6,7);
    if (i == ui.active_point) {
      graphics_fill_circle(ctx, point, ACTIVITY_RADIUS);
    } else {
      graphics_draw_circle(ctx, point, ACTIVITY_RADIUS);
    }
  }
}

void second_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (++ui.active_point == ACTIVITY_NUM_POINTS) ui.active_point = 0;
  layer_mark_dirty(ui.activity_layer);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Loading Text
  /* ui.text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } }); */
  /* text_layer_set_text(ui.text_layer, "Waiting for data .."); */
  /* text_layer_set_text_alignment(ui.text_layer, GTextAlignmentCenter); */
  /* layer_add_child(window_layer, text_layer_get_layer(ui.text_layer)); */

  // Activity Indicator
  ui.active_point = 0;
  ui.activity_layer = layer_create((GRect) { .origin = { 42, 110 }, .size = { 62, 14 } });
  layer_set_update_proc(ui.activity_layer, activity_layer_update_proc);
  layer_add_child(window_layer, ui.activity_layer);

  // iPhone Symbol
  ui.iphone_bmp = gbitmap_create_with_resource(RESOURCE_ID_IPHONE_ROUTE);
  ui.iphone_layer = bitmap_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 74 } });
  bitmap_layer_set_bitmap(ui.iphone_layer, ui.iphone_bmp);
  bitmap_layer_set_alignment(ui.iphone_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(ui.iphone_layer));

  tick_timer_service_subscribe(SECOND_UNIT, second_tick_handler);
}

static void window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  /* text_layer_destroy(ui.text_layer); */
  bitmap_layer_destroy(ui.iphone_layer);
  gbitmap_destroy(ui.iphone_bmp);
  layer_destroy(ui.activity_layer);
}

Window * window_waiting_create(void) {
  ui.window = window_create();
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  return ui.window;
}
