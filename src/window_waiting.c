#include "pebble.h"
#include "window_waiting.h"

static struct UI {
  Window *window;
  TextLayer *text_layer;
} ui;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  ui.text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(ui.text_layer, "Waiting for data ..");
  text_layer_set_text_alignment(ui.text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ui.text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.text_layer);
}

Window * window_waiting_create(void) {
  ui.window = window_create();
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  return ui.window;
}
