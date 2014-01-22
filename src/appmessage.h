// AppMessage attributes keys
enum {
  APPMESSAGE_KEY_ROUTESTEP_INDEX,
  APPMESSAGE_KEY_ROUTESTEP_DISTANCE,
  APPMESSAGE_KEY_ROUTESTEP_INSTRUCTIONS,
  APPMESSAGE_KEY_UPDATE_INDEX, // index of the current step
  APPMESSAGE_KEY_UPDATE_DISTANCE, // remaining distance in current step
  APPMESSAGE_KEY_UPDATE_TOTAL_DISTANCE, // remaining total distance to destination
};

// AppMessage action keys
enum {
  APPMESSAGE_REQUEST_ROUTE,
};

