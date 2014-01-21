#define MAX_ITEMS 128

// Haltestelle
struct RouteStep {
  char *distance;
  char *instructions;
};

// Liste aller Haltestellen
struct RouteSteps {
  int count;
  struct RouteStep data[MAX_ITEMS];
};

struct RouteSteps* route_step_stor_create();
void route_step_stor_destroy(struct RouteSteps* stations);
void route_step_stor_add(struct RouteSteps *stations, DictionaryIterator *received);
void route_step_stor_purge(struct RouteSteps *stor);

