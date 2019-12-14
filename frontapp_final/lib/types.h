typedef enum {
  ON,
  OFF,
} general_state_t;

typedef enum {
  OFF,
  TURN_LEFT,
  TURN_RIGHT,
} turn_light_state_t;

typedef struct {
  char  loc[64]; // TODO: is this large enough for most addresses?
  float dist;
} navigation_update_t;