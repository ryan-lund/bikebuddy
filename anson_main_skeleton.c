// NOTE: THIS FILE IS JUST BARE SKELETON FOR THE HEAD AND TURN LIGHTS
// WILL BE INTEGRATED INTO COMPLETE FILE

typedef enum {
  OFF,
  ON,
} head_light_state_t;

typedef enum {
  OFF,
  LEFT_ON,
  RIGHT_ON,
} turn_light_state_t;

// Dummy values for now
const float HEADLIGHT_THRESHOLD = 123456789;
const float TURNLIGHT_ROLL_LEFT_THRESHOLD = -123456789;
const float TURNLIGHT_ROLL_RIGHT_THRESHOLD = 123456789;
const float TURNLIGHT_YAW_LEFT_THRESHOLD = -123456789;
const float TURNLIGHT_YAW_RIGHT_THRESHOLD = 123456789;

// configure initial state
head_light_state_t head_light_state = OFF;
turn_light_state_t turn_light_state = OFF;
// Initialize senors here


int main(void) {
  
  // Initialize EVERYTHING - Left blank for now
  // Board, different sensors, display, etc

  // FSMs GO HERE ===================================================================

  // FSM for headlight -- Note: We can simplify this, as we don't really need a FSM...
  switch (head_light_state) {
    case OFF: {
      // If current light is above threshold
      if () {
        // TURN LIGHT ON
        head_light_state = ON;
      } else {
        // KEEP LIGHT OFF
        head_light_state = OFF;
      }
    }

    case ON: {
      // If current light is below threshold
      if () {
        // TURN LIGHT OFF
        head_light_state = OFF;
      } else {
        // KEEP LIGHT ON
        head_light_state = ON;
      }
    }
  }


  // FSM for turnlights. Again it seems like each state has the same thing...
  switch (turn_light_state) {
    case OFF: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        turn_light_state = LEFT_ON;
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        turn_light_state = RIGHT_ON;
      } else {
        // If neither, then not turning
        turn_light_state = OFF:
      }
    }

    case LEFT_ON: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        turn_light_state = LEFT_ON;
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        turn_light_state = RIGHT_ON;
      } else {
        // If neither, then not turning
        turn_light_state = OFF:
      }
    }

    case RIGHT_ON: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        turn_light_state = LEFT_ON;
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        turn_light_state = RIGHT_ON;
      } else {
        // If neither, then not turning
        turn_light_state = OFF:
      }
    }
  }
}