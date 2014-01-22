#include <pebble.h>
#include <route_steps_window.h>

static struct RouteSteps *route_steps;
static SendRequestCallback request_callback;

static struct RouteStepsWindowUi {
  Window *window;
  MenuLayer *detail_menu_layer;
  TextLayer *status_text_layer; // status text on the bottom
  //  char status_string[32]; // status string for it
  GBitmap *location_arrow_bitmap;
} ui;

// perform a scroll to the current position on list (according to the user location / GPS)
static void menu_scroll_to_current_location() {
  menu_layer_set_selected_index(ui.detail_menu_layer, (MenuIndex) { .row = route_steps->current_step, .section = 0 }, MenuRowAlignCenter, true);
}

// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return route_steps->count;
}

static void route_step_cell_draw(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, const char *distance, const char *instructions) {
  graphics_context_set_text_color(ctx, GColorBlack);
  int y = 0;
  if (cell_index->row > 0) {
    graphics_draw_text(ctx,
		       distance,
		       fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),  
		       GRect(0,-2,144,24),
		       GTextOverflowModeWordWrap,  
		       GTextAlignmentLeft,  
		       NULL);
    y += 24;
  }
	
  graphics_draw_text(ctx,
		     instructions,
		     fonts_get_system_font(FONT_KEY_GOTHIC_24),  
		     GRect(0,y,144,144),
		     GTextOverflowModeWordWrap,  
		     GTextAlignmentLeft,  
		     NULL);

  if (cell_index->row == route_steps->current_step) {
    graphics_draw_bitmap_in_rect(
				 ctx,
				 ui.location_arrow_bitmap,
				 GRect(124,6,17,17)
				 );
  }
}

static int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  GSize size = graphics_text_layout_get_content_size(
						     route_steps->data[cell_index->row].instructions,
						     fonts_get_system_font(FONT_KEY_GOTHIC_24),
						     GRect(0,0,144,144),
						     GTextOverflowModeWordWrap,
						     GTextAlignmentLeft
						     );
  return size.h + (cell_index->row == 0 ? 4 : 30);
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  if (cell_index->row >= route_steps->count) return; // index out of bounds

  route_step_cell_draw(ctx,
		       cell_layer,
		       cell_index,
		       cell_index->row == route_steps->current_step ?
		       route_steps->current_distance  :
		       route_steps->data[cell_index->row].distance,
		       route_steps->data[cell_index->row].instructions
		       );
}

void route_steps_window_reload_data(void) {
  if (window_is_loaded(ui.window))
    menu_layer_reload_data(ui.detail_menu_layer);
}

// Here we capture when a user selects a menu item
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  menu_scroll_to_current_location();  
}

// Here we capture when a user performs a long click on the select button
static void menu_select_longclick_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // call a request for recalculating the route to iOS
  request_callback();
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);  
  GRect bounds = layer_get_bounds(window_layer);

#define STATUS_BAR_HEIGHT 14
#define STATUS_BAR_OFFSET 0

  // Create the bottom status bar layer
  ui.status_text_layer = text_layer_create((GRect) {
      .origin = { 0, bounds.size.h-STATUS_BAR_HEIGHT+STATUS_BAR_OFFSET },
	.size = { bounds.size.w, STATUS_BAR_HEIGHT } });
  //  text_layer_set_font(ui.status_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(ui.status_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(ui.status_text_layer, GColorBlack);
  text_layer_set_text_color(ui.status_text_layer, GColorWhite);

  // Create the menu layer
  ui.detail_menu_layer = menu_layer_create((GRect) {
      .origin = { 0, 0 },
	.size = { bounds.size.w, bounds.size.h-STATUS_BAR_HEIGHT } });
  
  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(ui.detail_menu_layer, NULL, (MenuLayerCallbacks){
      .get_num_sections = menu_get_num_sections_callback,
	.get_num_rows = menu_get_num_rows_callback,
	.get_cell_height = menu_get_cell_height_callback,
	.draw_row = menu_draw_row_callback,
	.select_click = menu_select_callback,
	.select_long_click = menu_select_longclick_callback,
	});

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(ui.detail_menu_layer, window);
  
  // Add the layers to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(ui.detail_menu_layer));
  layer_add_child(window_layer, text_layer_get_layer(ui.status_text_layer));

  // bitmaps
  ui.location_arrow_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOCATION_ARROW);
}

static void window_unload(Window *window) {

  // Destroy the menu layer and Bitmap
  menu_layer_destroy(ui.detail_menu_layer);
  text_layer_destroy(ui.status_text_layer);
  gbitmap_destroy(ui.location_arrow_bitmap);
}

void route_steps_window_show() {
  // load the ui
  const bool animated = true;
  window_stack_push(ui.window, animated);
}

void route_steps_window_init(struct RouteSteps *stor, SendRequestCallback cb) {
  request_callback = cb;
  route_steps = stor;

  ui.window = window_create();
  window_set_window_handlers(ui.window, (WindowHandlers) {
      .load = window_load,
	.unload = window_unload,
	});
}

void route_steps_window_deinit() {
  window_destroy(ui.window);
}

bool route_steps_window_is_loaded() {
  return window_is_loaded(ui.window);
}

void route_steps_window_update(void) {
  //  snprintf(ui.status_string, sizeof(ui.status_string), ">| %s", route_steps->total_distance);
  //  text_layer_set_text(ui.status_text_layer, ui.status_string);
  
  text_layer_set_text(ui.status_text_layer, route_steps->total_distance);
  menu_layer_reload_data(ui.detail_menu_layer);
  menu_scroll_to_current_location();
}
