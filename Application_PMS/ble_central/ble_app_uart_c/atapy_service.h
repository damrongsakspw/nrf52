#ifndef ATAPY_SERVICE_H_
#define ATAPY_SERVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"

#define BLE_ATAPY_DEF(_name)                                                                            \
static ble_atapy_t _name;                                                                               \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                     \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                         \
                     ble_atapy_on_ble_evt, &_name)

#define CUSTOM_SERVICE_UUID_BASE         {0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, \
                                          0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}

#define ATAPY_SERVICE_UUID        0x1400
#define SSID_CHAR_UUID            0x1401
#define USER_CHAR_UUID            0x1402
#define PASS_CHAR_UUID            0x1403

/*
extern uint8_t ssid[32];
extern uint8_t user[32];
extern uint8_t pass[32];
extern uint8_t ssid_len;
extern uint8_t user_len;
extern uint8_t pass_len;
*/
struct wifi_conf
{
  
  uint8_t ssid[32];
  
  uint8_t user[32];
  
  uint8_t pass[32];
  
  uint8_t ssid_len;
  
  uint8_t user_len;
  
  uint8_t pass_len;
  
};

extern struct wifi_conf var_wifi;

  
  /**@brief Custom Service event type. */
typedef enum
{
  BLE_ATAPY_EVT_NOTIFICATION_ENABLED,                             /**< Custom value notification enabled event. */
  BLE_ATAPY_EVT_NOTIFICATION_DISABLED,                             /**< Custom value notification disabled event. */
  BLE_ATAPY_EVT_DISCONNECTED,
  BLE_ATAPY_EVT_CONNECTED
} ble_atapy_evt_type_t;

/**@brief Custom Service event. */
typedef struct
{
    ble_atapy_evt_type_t evt_type;                                  /**< Type of event. */
} ble_atapy_evt_t;
  
// Forward declaration of the ble_atapy_t type.
typedef struct ble_atapy_s ble_atapy_t;

/**@brief Custom Service event handler type. */
typedef void (*ble_atapy_evt_handler_t) (ble_atapy_t * p_bas,   ble_atapy_evt_t * p_evt);


typedef struct
{
  
  ble_atapy_evt_handler_t       evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
  uint8_t                       initial_custom_value;           /**< Initial custom value */
  ble_srv_cccd_security_mode_t  custom_value_char_attr_md;     /**< Initial security level for Custom characteristics attribute */
  
}ble_atapy_init_t;

struct ble_atapy_s
{
  uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/  // keeps track of the current connection and has nothing to do with attribute table handles
  uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
  ble_atapy_evt_handler_t     evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
  ble_gatts_char_handles_t    ssid_value_handles;           /**< Handles related to the Custom Value characteristic. */
  ble_gatts_char_handles_t    user_value_handles;           /**< Handles related to the Custom Value characteristic. */
  ble_gatts_char_handles_t    pass_value_handles;           /**< Handles related to the Custom Value characteristic. */
  uint8_t                     uuid_type; 
    
};


uint32_t  ble_atapy_init(ble_atapy_t *p_atapy, const ble_atapy_init_t * p_atapy_init);

void ble_atapy_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_atapy_custom_value_update(ble_atapy_t * p_atapy, uint8_t custom_value);


#endif 