// NOTE: THIS FILE IS JUST BARE SKELETON FOR THE BRAKE AND REAR TURN LIGHTS
// WILL BE INTEGRATED INTO COMPLETE FILE

#include types.h

// Dummy values for now
const float BRAKELIGHT_THRESHOLD = 123456789;
const float REARTURNLIGHT_ROLL_LEFT_THRESHOLD = -123456789;
const float REARTURNLIGHT_ROLL_RIGHT_THRESHOLD = 123456789;
const float REARTURNLIGHT_YAW_LEFT_THRESHOLD = -123456789;
const float REARTURNLIGHT_YAW_RIGHT_THRESHOLD = 123456789;

// configure initial state
brake_light_state_t brake_light_state = OFF;
rear_turn_light_state_t rear_turn_light_state = OFF;
// Initialize sensors here


int main(void) {
  
  // Initialize EVERYTHING - Left blank for now
  // Board, different sensors, display, etc

  // FSMs GO HERE ===================================================================

  // FSM for brake light -- Note: We can simplify this, as we don't really need a FSM...
  switch (brake_light_state) {
    case OFF: {
      // If current light is above threshold
      if () {
        // TURN LIGHT ON
        brake_light_state = ON;
      } else {
        // KEEP LIGHT OFF
        brake_light_state = OFF;
      }
    }

    case ON: {
      // If current light is below threshold
      if () {
        // TURN LIGHT OFF
        brake_light_state = OFF;
      } else {
        // KEEP LIGHT ON
        brake_light_state = ON;
      }
    }
  }


  // FSM for turnlights. Again it seems like each state has the same thing...
  switch (rear_turn_light_state) {
    case OFF: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        rear_turn_light_state = LEFT;
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        rear_turn_light_state = RIGHT;
      } else {
        // If neither, then not turning
        rear_turn_light_state = OFF:
      }
    }

    case LEFT: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        rear_turn_light_state = LEFT;
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        rear_turn_light_state = RIGHT;
      } else {
        // If neither, then not turning
        rear_turn_light_state = OFF:
      }
    }

    case RIGHT: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        rear_turn_light_state = LEFT;
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        rear_turn_light_state = RIGHT;
      } else {
        // If neither, then not turning
        rear_turn_light_state = OFF:
      }
    }
  }
}