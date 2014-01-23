#define MAX_ITEMS 128
#define DISTANCE_MAX_LEN 16
#define TOTAL_DISTANCE_MAX_LEN 40 // longer string, e.g. "Remaining distance: .."

// Haltestelle
struct RouteStep {
  char *distance;
  char *instructions;
};

// Liste aller Haltestellen
struct RouteSteps {
  int count;
  int current_step;
  char total_distance[TOTAL_DISTANCE_MAX_LEN];
  char current_distance[DISTANCE_MAX_LEN];
  struct RouteStep data[MAX_ITEMS];
};

struct RouteSteps* route_step_stor_create();
void route_step_stor_destroy(struct RouteSteps* stations);
void route_step_stor_add(struct RouteSteps *stations, DictionaryIterator *received);
void route_step_stor_purge(struct RouteSteps *stor);

