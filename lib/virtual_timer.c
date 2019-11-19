// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"

// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void) {
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[2] = 0;

  // Place your interrupt handler code here
  node_t* node = list_remove_first();
  if (node != NULL) {
    // Call the callback function
    node->cb();
    // If the node is repeated, update the value and re-insert
    if (node->repeated) {
      node->timer_value += node->period;
      list_insert_sorted(node);
    }
    // Update the capture/compare register to the first node
    node_t* first_node = list_get_first();
    if (first_node != NULL) {
      // Make sure that the comparison and the CC register write don't have an interrupt between them
      __disable_irq();
      // If the next node has already passed, call its callback
      while (first_node->timer_value < read_timer()){
        // Call the callback
        first_node->cb();
        // Remove the node from the list
        list_remove(first_node);
        // If the node is repeated, update the value and re-insert
        if (first_node->repeated) {
          first_node->timer_value += first_node->period;
          list_insert_sorted(first_node);
        } else {
          free(first_node);
        }
        // Peek at the next node
        first_node = list_get_first();
      }
      NRF_TIMER4->CC[2] = first_node->timer_value;
      __enable_irq();
    }
    // Free the node if it is not repeated
    if (!node->repeated) {
      free(node);
    }
  }
}

// Read the current value of the timer counter
uint32_t read_timer(void) {
  NRF_TIMER4->TASKS_CAPTURE[1] = 0x01;
  // Should return the value of the internal counter for TIMER4
  return NRF_TIMER4->CC[1];
}

// Initialize TIMER4 as a free running timer
// 1) Set to be a 32 bit timer
// 2) Set to count at 1MHz
// 3) Enable the timer peripheral interrupt (look carefully at the INTENSET register!)
// 4) Clear the timer
// 5) Start the timer
void virtual_timer_init(void) {
  // Place your timer initialization code here
  NRF_TIMER4->BITMODE = 0x03;
  NRF_TIMER4->PRESCALER = 0x04;
  // Enable two CC registers for TIMER4
  NRF_TIMER4->INTENSET = 0x01 << 17;
  NRF_TIMER4->INTENSET |= 0x01 << 18;
  // Enable the TIMER4 interrupt in the NVIC
  NVIC_EnableIRQ(TIMER4_IRQn);

  NRF_TIMER4->TASKS_CLEAR = 0x01;
  NRF_TIMER4->TASKS_START = 0x01;
}

// Start a timer. This function is called for both one-shot and repeated timers
// To start a timer:
// 1) Create a linked list node (This requires `malloc()`. Don't forget to free later)
// 2) Setup the linked list node with the correct information
//      - You will need to modify the `node_t` struct in "virtual_timer_linked_list.h"!
// 3) Place the node in the linked list
// 4) Setup the compare register so that the timer fires at the right time
// 5) Return a timer ID
//
// Your implementation will also have to take special precautions to make sure that
//  - You do not miss any timers
//  - You do not cause consistency issues in the linked list (hint: you may need the `__disable_irq()` and `__enable_irq()` functions).
//
// Follow the lab manual and start with simple cases first, building complexity and
// testing it over time.
static uint32_t timer_start(uint32_t microseconds, virtual_timer_callback_t cb, bool repeated) {
  // Create a linked list node with input parameters
  node_t* node = malloc(sizeof(node_t));
  node->timer_value = read_timer() + microseconds;
  node->cb = cb;
  node->repeated = repeated;
  if (repeated) {
    node->period = microseconds;
  }
  // Insert the node to the timer linked list
  list_insert_sorted(node);
  // Update the capture/compare register to the first node
  node_t* first_node = list_get_first();
  if (first_node != NULL) {
    // Make sure that the comparison and the CC register write don't have an interrupt between them
    __disable_irq();
    // If the next node has already passed, call its callback
    while (first_node->timer_value < read_timer()){
      // Call the callback
      first_node->cb();
      // Remove the node from the list
      list_remove(first_node);
      // If the node is repeated, update the value and re-insert
      if (first_node->repeated) {
        first_node->timer_value += first_node->period;
        list_insert_sorted(first_node);
      } else {
        free(first_node);
      }
      // Peek at the next node
      first_node = list_get_first();
    }
    NRF_TIMER4->CC[2] = first_node->timer_value;
    __enable_irq();
  }  
  NRF_TIMER4->EVENTS_COMPARE[2] = 0x01;
  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
  return node;
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, false);
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start_repeated(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, true);
}

// Remove a timer by ID.
// Make sure you don't cause linked list consistency issues!
// Do not forget to free removed timers.
void virtual_timer_cancel(uint32_t timer_id) {
  list_remove((node_t*) timer_id);
  free((node_t*) timer_id);
  // Update the capture/compare register to the first node
  node_t* first_node = list_get_first();
  if (first_node != NULL) {
    // Make sure that the comparison and the CC register write don't have an interrupt between them
    __disable_irq();
    // If the next node has already passed, call its callback
    while (first_node->timer_value < read_timer()){
      // Call the callback
      first_node->cb();
      // Remove the node from the list
      list_remove(first_node);
      // If the node is repeated, update the value and re-insert
      if (first_node->repeated) {
        first_node->timer_value += first_node->period;
        list_insert_sorted(first_node);
      } else {
        free(first_node);
      }
      // Peek at the next node
      first_node = list_get_first();
    }
    NRF_TIMER4->CC[2] = first_node->timer_value;
    __enable_irq();
  }
}

