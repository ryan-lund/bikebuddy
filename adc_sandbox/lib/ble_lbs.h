/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup ble_lbs Bike Buddy Service Server
 * @{
 * @ingroup ble_sdk_srv
 *
 * @brief Bike Buddy Service Server module.
 *
 * @details This module implements a custom Bike Buddy Service with an LED and Button Characteristics.
 *          During initialization, the module adds the Bike Buddy Service and Characteristics
 *          to the BLE stack database.
 *
 *          The application must supply an event handler for receiving Bike Buddy Service
 *          events. Using this handler, the service notifies the application when the
 *          LED value changes.
 *
 *          The service also provides a function for letting the application notify
 *          the state of the Button Characteristic to connected peers.
 *
 * @note    The application must register this module as BLE event observer using the
 *          NRF_SDH_BLE_OBSERVER macro. Example:
 *          @code
 *              ble_hids_t instance;
 *              NRF_SDH_BLE_OBSERVER(anything, BLE_HIDS_BLE_OBSERVER_PRIO,
 *                                   ble_hids_on_ble_evt, &instance);
 *          @endcode
 */

#ifndef BLE_LBS_H__
#define BLE_LBS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_lbs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_LBS_DEF(_name)                                                                          \
static ble_lbs_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_LBS_BLE_OBSERVER_PRIO,                                                     \
                     ble_lbs_on_ble_evt, &_name)

#define LBS_UUID_BASE        {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
                              0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define LBS_UUID_SERVICE           0x1523
#define LBS_UUID_NAV_CHAR          0x1524
#define LBS_UUID_DIRECTION_CHAR    0x1525
#define LBS_UUID_BLIND_CHAR        0x1526
#define LBS_UUID_LIGHT_CHAR        0x1527  
#define LBS_UUID_DISTANCE_CHAR     0x1528                        
#define LBS_UUID_BACK_CHAR         0x1529


// Forward declaration of the ble_lbs_t type.
typedef struct ble_lbs_s ble_lbs_t;

typedef void (*ble_lbs_nav_write_handler_t) (uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * new_state, uint16_t len);
typedef void (*ble_lbs_direction_write_handler_t) (uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * new_state, uint16_t len);
typedef void (*ble_lbs_light_write_handler_t) (uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * new_state, uint16_t len);
typedef void (*ble_lbs_blind_write_handler_t) (uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * new_state, uint16_t len);
typedef void (*ble_lbs_distance_write_handler_t) (uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * new_state, uint16_t len);

/** @brief Bike Buddy Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_lbs_nav_write_handler_t nav_write_handler; /**< Event handler to be called when the LED Characteristic is written. */
    ble_lbs_direction_write_handler_t direction_write_handler;
    ble_lbs_light_write_handler_t light_write_handler;
    ble_lbs_blind_write_handler_t blind_write_handler;
    ble_lbs_distance_write_handler_t distance_write_handler;
} ble_lbs_init_t;

/**@brief Bike Buddy Service structure. This structure contains various status information for the service. */
struct ble_lbs_s
{
    uint16_t                    service_handle;      /**< Handle of Bike Buddy Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    nav_char_handles;    
    ble_gatts_char_handles_t    direction_char_handles; 
    ble_gatts_char_handles_t    blind_char_handles;    
    ble_gatts_char_handles_t    light_char_handles; 
    ble_gatts_char_handles_t    back_char_handles;
    ble_gatts_char_handles_t    distance_char_handles;
    uint8_t                     uuid_type;           /**< UUID type for the Bike Buddy Service. */
    ble_lbs_nav_write_handler_t nav_write_handler; /**< Event handler to be called when the LED Characteristic is written. */
    ble_lbs_direction_write_handler_t direction_write_handler;
    ble_lbs_light_write_handler_t light_write_handler;
    ble_lbs_blind_write_handler_t blind_write_handler;
    ble_lbs_distance_write_handler_t distance_write_handler;
};


/**@brief Function for initializing the Bike Buddy Service.
 *
 * @param[out] p_lbs      Bike Buddy Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_lbs_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_lbs_init(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the Bike Buddy Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  Bike Buddy Service structure.
 */
void ble_lbs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a button state notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the button state notification will be sent.
 * @param[in] p_lbs         Bike Buddy Service structure.
 * @param[in] button_state  New button state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_lbs_on_state_change(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t button_state);


#ifdef __cplusplus
}
#endif

#endif // BLE_LBS_H__

/** @} */
