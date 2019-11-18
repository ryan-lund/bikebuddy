#include types.h

// Dummy values for now
const float HEADLIGHT_THRESHOLD = 123456789;
const float TURNLIGHT_ROLL_LEFT_THRESHOLD = -123456789;
const float TURNLIGHT_ROLL_RIGHT_THRESHOLD = 123456789;
const float TURNLIGHT_YAW_LEFT_THRESHOLD = -123456789;
const float TURNLIGHT_YAW_RIGHT_THRESHOLD = 123456789;

// configure initial state
head_light_state_t head_light_state = OFF;
turn_light_state_t turn_light_state = OFF;
navigation_state_t nav_state = OFF;

// global variables
struct navigation_update_t nav_update;
float dst;
char loc[64];

// booleans to be set by BLE interrupt handler for maps API
bool start_nav = false;
bool stop_nav = false;
bool update_nav = false;

// Initialize senors here

// Initialize BLE, connect to BACK module

// TODO: handle BLE notification with interrrupt, set boolean values, or update with new instruction


int main(void) {
  
  // Initialize EVERYTHING - Left blank for now
  // Board, different sensors, display, etc

  // FSMs GO HERE ===================================================================

  // FSM for headlight
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


  // FSM for turnlights.
  switch (turn_light_state) {
    case OFF: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        turn_light_state = LEFT;
        // Send update to BACK module
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        turn_light_state = RIGHT;
        // Send update to BACK module
      } else {
        // If neither, then not turning
        turn_light_state = OFF;
        // Send update to BACK module
      }
    }

    case LEFT: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        turn_light_state = LEFT;
        // Send update to BACK module
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        turn_light_state = RIGHT;
        // Send update to BACK module
      } else {
        // If neither, then not turning
        turn_light_state = OFF;
        // Send update to BACK module
      }
    }

    case RIGHT: {
      if () {
        // If current roll and yaw are less than left thresholds
        // Will call function to handle lights to blink
        turn_light_state = LEFT;
        // Send update to BACK module
      } else if () {
        // If current roll and yaw are greater than right thresholds
        // Will call function to handle lights to blink
        turn_light_state = RIGHT;
        // Send update to BACK module
      } else {
        // If neither, then not turning
        turn_light_state = OFF;
        // Send update to BACK module
      }
    }
  }

  // FSM for nav system
  switch (nav_state) {
    case OFF: {
      // If we recieve a BLE notification from the phone to start navigation
      dst = 0.0;
      loc = "";
      if (start_nav) {
        // Start navigating
        nav_state = ON;
        start_nav = false;
      } else {
        // Remain with no navigataion
        head_light_state = OFF;
      }
    }

    case ON: {
      // If we recieve arrived notification from the phone, stop navigation
      if (stop_nav) {
        // Turn off navigation
        nav_state = OFF;
        stop_nav = false;
      } 
      if (update_nav) {
        // Keep navigation on
        nav_state = ON;
        update_nav = false;

        // parse update
        dst = nav_update.dst;
        loc = nav_update.loc;

        // update display to show new turn location, new distance
      } else {
        // Keep navigation on
        nav_state = ON;

        // Update distance 
        dst = update_distance(); // TODO: define function
        // Refresh display
      }
    }
  }
}