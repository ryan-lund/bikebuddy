// NOTE: THIS FILE IS JUST BARE SKELETON FOR THE HEAD AND TURN LIGHTS
// WILL BE INTEGRATED INTO COMPLETE FILE

#include types.h

// Dummy values for now


// configure initial states
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

// TODO: handle BLE notification with interrrupt, set boolean values, or update with new instruction


int main(void) {
  
  // Initialize EVERYTHING - Left blank for now
  // Board, different sensors, display, etc

  // FSMs GO HERE ===================================================================

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