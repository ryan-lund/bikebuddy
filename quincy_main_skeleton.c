// NOTE: THIS FILE IS JUST BARE SKELETON FOR THE BRAKE AND REAR TURN LIGHTS
// WILL BE INTEGRATED INTO COMPLETE FILE

#include types.h

static simple_ble_config_t ble_config = {
        // c0:98:e5:49:xx:xx
        .platform_id       = 0x49,    // used as 4th octect in device BLE address
        .device_id         = 0xBBBB, // TODO: replace with your lab bench number
        .adv_name          = "Bike Buddy Rear", // used in advertisements if there is room
        .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

// 3db02924-b2a6-4d47-be1f-0f90ad62a048
static simple_ble_service_t map_service = {{
    .uuid128 = {0x48,0xA0,0x62,0xAD,0x90,0x0F,0x1F,0xBE,
                0x47,0x4D,0xA6,0xB2,0x24,0x29,0xB0,0x3D}
}};

static simple_ble_char_t brake_char = {.uuid16 = 0x2925};
static simple_ble_char_t turn_char = {.uuid16 = 0x2926};


// configure initial state
brake_light_state_t brake_light_state = OFF;
rear_turn_light_state_t rear_turn_light_state = OFF;
// Initialize sensors here


// Main application state
simple_ble_app_t* simple_ble_app;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
  if (simple_ble_is_char_event(p_ble_evt, &brake_char)) {
    if (brake_light_state == 1) {
      brake_light_state = ON;
    } else {
      brake_light_state = OFF;
    }
  }
  if (simple_ble_is_char_event(p_ble_evt, &rear_turn_light_char)) {
    if (rear_turn_light_state == 0) {
      rear_turn_light_state = OFF;
    } else if (rear_turn_light_state == 1) {
      rear_turn_light_state = LEFT;
    } else if (rear_turn_light_state == 2) {
      rear_turn_light_state = RIGHT;
    }
  }  
}

int main(void) {
  
  // Initialize EVERYTHING - Left blank for now
  // Board, different sensors, display, etc

  // FSMs GO HERE ===================================================================

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