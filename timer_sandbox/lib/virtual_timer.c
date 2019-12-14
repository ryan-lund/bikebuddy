// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"


static void handle_timers()
{
  node_t *first = list_get_first();
  bool found = false;
  while (first != NULL && !found) {
    if (read_timer() > first->timer_value) {
      first->cb();
      list_remove_first();
      free(first);
      first = list_get_first();
    } else {
      NRF_TIMER4->CC[0] = first->timer_value;
      found = true;
    }
  }
}

// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void) {
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[0] = 0;

  // Place your interrupt handler code here
  // printf("Timer Fired!\n");
  node_t *currTimer = list_remove_first();
  currTimer->cb();
  if (currTimer->isRepeated)
  {
    currTimer->timer_value += currTimer->repeatTime;
    list_insert_sorted(currTimer);
  }
  else
  {
    free(currTimer);
  }
  __disable_irq();
  handle_timers();
  __enable_irq();
}

// Read the current value of the timer counter
uint32_t read_timer(void) {

  // Should return the value of the internal counter for TIMER4
  NRF_TIMER4->TASKS_CAPTURE[1] = 1;

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
  NRF_TIMER4->BITMODE = 3;
  NRF_TIMER4->PRESCALER = 4;
  NRF_TIMER4->TASKS_CLEAR = 1;
  NRF_TIMER4->TASKS_START = 1;

  NRF_TIMER4->INTENSET |= 1 << 16;
  NVIC_EnableIRQ(TIMER4_IRQn);
  NVIC_SetPriority(TIMER4_IRQn, 1);
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
  __disable_irq();
  uint32_t newTime = read_timer() + microseconds;

  // create new node
  node_t *newNode = malloc(sizeof(node_t));
  newNode->timer_value = newTime;
  newNode->cb = cb;
  newNode->isRepeated = repeated;
  newNode->repeatTime = microseconds;
  list_insert_sorted(newNode);

  // check if expired
  handle_timers();

  __enable_irq();

  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
  return (uint32_t) newNode;
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
  __disable_irq();
  node_t *currNode = (node_t*) timer_id;

  // while(currNode != NULL) {
  //   if (currNode->timerID == timer_id) {
  //     if (NRF_TIMER4->CC[0] == currNode->timer_value) {
  //       if(currNode->next != NULL) {
  //         NRF_TIMER4->CC[0] = currNode->next->timer_value;
  //       } else {
  //         NRF_TIMER4->CC[0] = 0;
  //       }
  //     }
  //     list_remove(currNode);
  //     free(currNode);
  //     return;
  //   }
  //   currNode = currNode->next;
//   }

  // if (NRF_TIMER4->CC[0] == currNode->timer_value) {
  //   if(currNode->next != NULL) {
  //     NRF_TIMER4->CC[0] = currNode->next->timer_value;
  //   } else {
  //     NRF_TIMER4->CC[0] = 0;
  //   }
  // }
  // SET TO ZERO JUST IN CASE QUEUED UP
  // BELOW CODE WILL REQUEUE IF NECESSARY
  NRF_TIMER4->CC[0] = 0;
  list_remove(currNode);
  free(currNode);

  handle_timers();
  __enable_irq();
}

