#include <pebble.h>
#include <route_steps_window.h>

static struct RouteSteps *route_steps;

static struct RouteStepsWindowUi {
  Window *window;
  MenuLayer *detail_menu_layer;
} ui;

// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return route_steps->count;
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  if (cell_index->row >= route_steps->count) return; // index out of bounds

  menu_cell_basic_draw(ctx, cell_layer, 
		       route_steps->data[cell_index->row].distance,
		       route_steps->data[cell_index->row].instructions,
		       NULL);
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Header Test (static now)");
}

void route_steps_window_reload_data(void) {
  if (window_is_loaded(ui.window))
    menu_layer_reload_data(ui.detail_menu_layer);
}

// Here we capture when a user selects a menu item
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  //  departure_detail_show(&route_steps->data[cell_index->row], ui.selected_station_name);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);  
  GRect bounds = layer_get_bounds(window_layer);

  // Create the menu layer
  ui.detail_menu_layer = menu_layer_create(bounds);
  
  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(ui.detail_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(ui.detail_menu_layer, window);
  
  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(ui.detail_menu_layer));
}

static void window_unload(Window *window) {

  // Destroy the menu layer and Bitmap
  menu_layer_destroy(ui.detail_menu_layer);
}

void route_steps_window_show() {
  // load the ui
  const bool animated = true;
  window_stack_push(ui.window, animated);
}

void route_steps_window_init(struct RouteSteps *stor) {
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
