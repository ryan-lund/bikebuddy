typedef enum {
  OFF,
  ON,
} head_light_state_t;

typedef enum {
  OFF,
  LEFT,
  RIGHT,
} turn_light_state_t;

typedef enum {
  ON,
  OFF,
} navigation_state_t;

typedef struct {
  char  loc[64]; // TODO: is this lorge enough for most addresses?
  float dist;
} navigation_update_t;