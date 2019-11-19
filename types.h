typedef enum {
  LIGHT_OFF,
  LIGHT_ON,
} light_state_t;

typedef enum {
  TURN_OFF,
  TURN_LEFT,
  TURN_RIGHT,
} turn_light_state_t;

typedef enum {
  ON,
  OFF,
} navigation_state_t;

typedef struct {
  char  loc[64]; // TODO: is this lorge enough for most addresses?
  float dist;
} navigation_update_t;