/**
* Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <nrfx_timer.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_uart.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "app_timer.h"
#include "app_util.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "ble_nus_c.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_ble_scan.h"
#include "custom_timer.h"
#include "fds.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "sensorsim.h"

#include "nrf_queue.h"

#include "atapy_service.h"
#include "defs.h"
//#include "queue.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define DEVICE_NAME                     "TAGATAPY"                       /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                   /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */

#define APP_ADV_DURATION                18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_CONN_CFG_TAG    1                                       /**< Tag that refers to the BLE stack configuration set with @ref sd_ble_cfg_set. The default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */
#define APP_BLE_OBSERVER_PRIO   3                                       /**< BLE observer priority of the application. There is no need to modify this value. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#define UART_TX_BUF_SIZE        4096//512//256                                     /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE        512                                     /**< UART RX buffer size. */

#define NRF_BLE_SCAN_SCAN_INTERVAL_SLOW         1600
#define NRF_BLE_SCAN_SCAN_WINDOW_SLOW           800

#define NRF_BLE_SCAN_SCAN_INTERVAL_NORMAL         480
#define NRF_BLE_SCAN_SCAN_WINDOW_NORMAL           480

//#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_BLE//BLE_UUID_TYPE_VENDOR_BEGIN              /**< UUID type for the Nordic UART Service (vendor specific). */

//#define ECHOBACK_BLE_UART_DATA  1                                       /**< Echo the UART data that is received over the Nordic UART Service (NUS) back to the sender. */


NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                             /**< Advertising module instance. */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

BLE_ATAPY_DEF(m_atapy);
//BLE_NUS_C_DEF(m_ble_nus_c);                                             /**< BLE Nordic UART Service (NUS) client instance. */
//NRF_BLE_GATT_DEF(m_gatt);                                               /**< GATT module instance. */
//BLE_DB_DISCOVERY_DEF(m_db_disc);                                        /**< Database discovery module instance. */

NRF_BLE_SCAN_DEF(m_scan);                                               /**< Scanning Module instance. */
//NRF_QUEUE_DEF(char, m_queue, 1000, NRF_QUEUE_MODE_OVERFLOW);
NRF_QUEUE_DEF(char, m_queue, 870, NRF_QUEUE_MODE_OVERFLOW);
NRF_QUEUE_INTERFACE_DEC(char, my_queue);
NRF_QUEUE_INTERFACE_DEF(char, my_queue, &m_queue);
//char *ax[1000];
int round_evt = 0;
//uint8_t id[6];
//int num_id = 0;

 bool status_empty = false;

char data_ble[34];
char data_queue[29]; 
uint8_t * aux;
uint16_t aux_len ;    
uint8_t aux_array[100];

struct wifi_conf var_wifi;


struct status_scan chk_scan;

static ble_gap_scan_params_t const m_scan_param_slow =
{
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL_SLOW,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW_SLOW

};

static ble_gap_scan_params_t const m_scan_param_normal =
{
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL_NORMAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW_NORMAL

};

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] =                                               /**< Universally unique service identifiers. */
{
    {ATAPY_SERVICE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN}
};

static ble_uuid_t const m_atapy_uuid =
{
  .uuid = 0xFEAA,//BLE_UUID_NUS_SERVICE,
  .type = BLE_UUID_TYPE_BLE//NUS_SERVICE_UUID_TYPE
};


static void advertising_start(bool erase_bonds);

//static struct status_scan chk_status_scan;


//queue_mesg uart_mesg;

//static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH; /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */


/**@brief Function for handling asserts in the SoftDevice.
*
* @details This function is called in case of an assert in the SoftDevice.
*
* @warning This handler is only an example and is not meant for the final product. You need to analyze
*          how your product is supposed to react in case of assert.
* @warning On assert from the SoftDevice, the system can only recover on reset.
*
* @param[in] line_num     Line number of the failing assert call.
* @param[in] p_file_name  File name of the failing assert call.
*/
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
  app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for starting scanning. */
static void scan_start(void)
{
  ret_code_t ret;
  
  ret = nrf_ble_scan_start(&m_scan);
  APP_ERROR_CHECK(ret);
  
  ret = bsp_indication_set(BSP_INDICATE_SCANNING);
  APP_ERROR_CHECK(ret);
}


/**@brief Function for handling Scanning Module events.
*/
static void  scan_evt_handler(scan_evt_t const * p_scan_evt)
{
  
  //printf("scan_evt_handler: %d\r\n",p_scan_evt->scan_evt_id);
  switch(p_scan_evt->scan_evt_id)
  {
    //  ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;
    //  
    //  const ble_gap_evt_adv_report_t * p_adv_report = &p_gap_evt->params.adv_report;
    //  
    //  uint8_t * aux = p_adv_report->data.p_data;
    //  uint16_t aux_len = p_adv_report->data.len;
    //  uint8_t aux_array[100];
    //  memcpy(aux_array,aux,aux_len);
    //char data_ble[29];
    

    
  case NRF_BLE_SCAN_EVT_NOT_FOUND:
    {
      //do something
      //printf("not found\r\n");
      
      
      /*printf("Received advertising from: %02x:%02x:%02x:%02x:%02x:%02x\n\r", p_scan_evt->params.p_not_found->peer_addr.addr[5], 
      p_scan_evt->params.p_not_found->peer_addr.addr[4], 
      p_scan_evt->params.p_not_found->peer_addr.addr[3], 
      p_scan_evt->params.p_not_found->peer_addr.addr[2], 
      p_scan_evt->params.p_not_found->peer_addr.addr[1], 
      p_scan_evt->params.p_not_found->peer_addr.addr[0]);
      
      */
      break;
    }

  case NRF_BLE_SCAN_EVT_FILTER_MATCH:
    {
      //printf("match\r\n");
      //char data_ble[29];
      //      uint8_t * aux = p_scan_evt->params.filter_match.p_adv_report->data.p_data;
      //      uint16_t aux_len = p_scan_evt->params.filter_match.p_adv_report->data.len;    
      //      uint8_t aux_array[100];[100];
      memset(aux_array,0,sizeof(aux_array));
      
      aux = p_scan_evt->params.filter_match.p_adv_report->data.p_data;
      aux_len = p_scan_evt->params.filter_match.p_adv_report->data.len;  
      
      memcpy(aux_array,aux,aux_len);
      
      if((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (aux_array[15] == 0x54)){
        memset(data_ble,0,sizeof(data_ble));
        /*printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",p_scan_evt->params.filter_match.p_adv_report->peer_addr.addr[5],
        p_scan_evt->params.filter_match.p_adv_report->peer_addr.addr[4],
        p_scan_evt->params.filter_match.p_adv_report->peer_addr.addr[3],
        p_scan_evt->params.filter_match.p_adv_report->peer_addr.addr[2],
        p_scan_evt->params.filter_match.p_adv_report->peer_addr.addr[1],
        p_scan_evt->params.filter_match.p_adv_report->peer_addr.addr[0]);
        */
        sprintf(data_ble,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02d",aux_array[16]                          //src_id
                                                                               ,aux_array[17]                          //src_id
                                                                               ,aux_array[18]                          //src_id    
                                                                               ,aux_array[19]                          //variance_0   
                                                                               ,aux_array[20]                          //variance_1   
                                                                               ,aux_array[21]                          //variance_2
                                                                               ,aux_array[23]                          //cnt
                                                                               ,aux_array[24]                          //vt_0
                                                                               ,aux_array[25]                          //vt_1
                                                                               ,aux_array[26]                          //vt_2
                                                                               ,aux_array[27]                          //activity    
                                                                               ,aux_array[28]                          //batt                                                                  
                                                                               ,p_scan_evt->params.filter_match.p_adv_report->rssi);    //rssi
        printf("data:%s\r\n",data_ble);
        
        //ret_code_t err_code = my_queue_write(data_ble, sizeof(data_ble));     
        //APP_ERROR_CHECK(err_code);
        round_evt++;
      }
      break;
    }    
  case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
    
    NRF_LOG_INFO("Scan timed out.");
    scan_start();
    break;
    
  default:
    break;
    
  }
}


/**@brief Function for initializing the scanning and setting the filters.
*/
static void scan_init(void)
{
  ret_code_t          err_code;
  nrf_ble_scan_init_t init_scan;
  
  memset(&init_scan, 0, sizeof(init_scan));
  
  init_scan.connect_if_match = false;
  init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;
  
  err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
  APP_ERROR_CHECK(err_code);
  
  err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &m_atapy_uuid);
  APP_ERROR_CHECK(err_code);
  
  err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_UUID_FILTER, false);
  APP_ERROR_CHECK(err_code);
  
}


/**@brief Function for handling database discovery events.
*
* @details This function is a callback function to handle events from the database discovery module.
*          Depending on the UUIDs that are discovered, this function forwards the events
*          to their respective services.
*
* @param[in] p_event  Pointer to the database discovery event.
*/
//static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
//{
//    ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
//}


/**@brief Function for handling characters received by the Nordic UART Service (NUS).
*
* @details This function takes a list of characters of length data_len and prints the characters out on UART.
*          If @ref ECHOBACK_BLE_UART_DATA is set, the data is sent back to sender.
*/


//static void ble_nus_chars_received_uart_print(uint8_t * p_data, uint16_t data_len)
//{
//    ret_code_t ret_val;
//
//    NRF_LOG_DEBUG("Receiving data.");
//    NRF_LOG_HEXDUMP_DEBUG(p_data, data_len);
//
//    for (uint32_t i = 0; i < data_len; i++)
//    {
//        do
//        {
//            ret_val = app_uart_put(p_data[i]);
//            if ((ret_val != NRF_SUCCESS) && (ret_val != NRF_ERROR_BUSY))
//            {
//                //NRF_LOG_ERROR("app_uart_put failed for index 0x%04x.", i);
//                printf("app_uart_put failed for index 0x%04x.", i);
//                APP_ERROR_CHECK(ret_val);
//            }
//        } while (ret_val == NRF_ERROR_BUSY);
//    }
//    if (p_data[data_len-1] == '\r')
//    {
//        while (app_uart_put('\n') == NRF_ERROR_BUSY);
//    }
//    if (ECHOBACK_BLE_UART_DATA)
//    {
//        // Send data back to the peripheral.
//        do
//        {
//            ret_val = ble_nus_c_string_send(&
//, p_data, data_len);
//            if ((ret_val != NRF_SUCCESS) && (ret_val != NRF_ERROR_BUSY))
//            {
//                NRF_LOG_ERROR("Failed sending NUS message. Error 0x%x. ", ret_val);
//                APP_ERROR_CHECK(ret_val);
//            }
//        } while (ret_val == NRF_ERROR_BUSY);
//    }
//}


/**@brief   Function for handling app_uart events.
*
* @details This function receives a single character from the app_uart module and appends it to
*          a string. The string is sent over BLE when the last character received is a
*          'new line' '\n' (hex 0x0A) or if the string reaches the maximum data length.
*/
void uart_event_handle(app_uart_evt_t * p_event)
{
  switch (p_event->evt_type)
  {
    
  case APP_UART_DATA_READY:
    {          
      
      uint8_t get;
      //uint8_t id[7];
      //int i = 0;
      //char ID;
      //printf("insert\r\n");
      if(app_uart_get(&get) == NRF_SUCCESS)
      {
        
        //printf("%d",get);
        //printf("%c",get);
        
        if ((get == 'A') || (get == 'a'))
        {
          //printf("insert\r\n");
          
          
          /*
          //int a = 5;
          uint8_t  *data;
          data = (uint8_t *) malloc(sizeof(uint8_t)*a);
          data[0] = 1;
          data[1] = 1;
          data[2] = 2;
          data[3] = 3;
          data[4] = 4;
          */  
          /*
          if(round_evt == 0){
          //do something
          //printf("round_evt :%d\r\n",round_evt);
          for (int a =0 ;a <= num_id; a++ ){
          //printf("%c num_id = %d\r\n",id[a],num_id);
        }
        }*/
          status_empty = my_queue_is_empty();
          if(status_empty != true)
            //if(round_evt > 0)
          {
            chk_scan.chk_round = true;
            //int size_data = 25*round_evt;
            //uint8_t  *dat;
            //dat = (uint8_t *) malloc(sizeof(uint8_t)*size_data);
            //char  *dat;
            //dat = (char *) malloc(sizeof(uint8_t)*size_data);        
            //printf("%d", nrf_queue_available_get(&m_queue));
            //int free_queue = nrf_queue_available_get(&m_queue);
            //int evt_queue = free_queue/25;
            //printf("%d:::::",evt_queue);
            
            
            printf("%d",round_evt);    
            for(uint8_t i=0;i<round_evt;i++){
              
              //char data_queue[29];   
              memset(data_queue,0,sizeof(data_queue));
              ret_code_t err_code = my_queue_read(data_queue, sizeof(data_queue));
              APP_ERROR_CHECK(err_code);
              printf(",%s",data_queue);  
              //printf(",%d",i);  
              //memset(data_queue, 0, sizeof(data_queue));
              
            }  
            printf(":\r\n");       
            round_evt = 0;
          }
          else
          {
            chk_scan.chk_round = false;
          }
        }
        else if((get == 'W') || (get == 'w'))
        {
          
          if((var_wifi.ssid_len != 0) && (var_wifi.user_len == 0) && (var_wifi.pass_len != 0))
          {
            for(uint8_t i =0;i<var_wifi.ssid_len+1;i++)printf("%c",var_wifi.ssid[i]);
            for(uint8_t i =0;i<var_wifi.pass_len+1;i++)printf("%c",var_wifi.pass[i]);
            
          }
          else if((var_wifi.ssid_len != NULL) && (var_wifi.user_len != NULL) &&(var_wifi.pass_len != NULL))
          {
            for(uint8_t i =0;i<var_wifi.ssid_len+1;i++)printf("%c",var_wifi.ssid[i]);          
            for(uint8_t i =0;i<var_wifi.user_len+1;i++)printf("%c",var_wifi.user[i]);
            for(uint8_t i =0;i<var_wifi.pass_len+1;i++)printf("%c",var_wifi.pass[i]);
          }
          else printf("request wifi\r\n"); 
        }     
      } 
      break;
    }
  default:
    break;
  }
}


/**@brief Callback handling Nordic UART Service (NUS) client events.
*
* @details This function is called to notify the application of NUS client events.
*
* @param[in]   p_ble_nus_c   NUS client handle. This identifies the NUS client.
* @param[in]   p_ble_nus_evt Pointer to the NUS client event.
*/

/**@snippet [Handling events from the ble_nus_c module] */

//static void ble_nus_c_evt_handler(ble_nus_c_t * p_ble_nus_c, ble_nus_c_evt_t const * p_ble_nus_evt)
//{
//    ret_code_t err_code;
//
//    switch (p_ble_nus_evt->evt_type)
//    {
//        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
//            NRF_LOG_INFO("Discovery complete.");
//            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
//            APP_ERROR_CHECK(err_code);
//
//            err_code = ble_nus_c_tx_notif_enable(p_ble_nus_c);
//            APP_ERROR_CHECK(err_code);
//            NRF_LOG_INFO("Connected to device with Nordic UART Service.");
//            break;
//
//        case BLE_NUS_C_EVT_NUS_TX_EVT:
//            ble_nus_chars_received_uart_print(p_ble_nus_evt->p_data, p_ble_nus_evt->data_len);
//            break;
//
//        case BLE_NUS_C_EVT_DISCONNECTED:
//            NRF_LOG_INFO("Disconnected.");
//            scan_start();
//            break;
//    }
//}
//
///**@snippet [Handling events from the ble_nus_c module] */
//
//
///**
// * @brief Function for handling shutdown events.
// *
// * @param[in]   event       Shutdown type.
// */
//static bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
//{
//    ret_code_t err_code;
//
//    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
//    APP_ERROR_CHECK(err_code);
//
//    switch (event)
//    {
//        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
//            // Prepare wakeup buttons.
//            err_code = bsp_btn_ble_sleep_mode_prepare();
//            APP_ERROR_CHECK(err_code);
//            break;
//
//        default:
//            break;
//    }
//
//    return true;
//}

//NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler, APP_SHUTDOWN_HANDLER_PRIORITY);


/**@brief Function for handling BLE events.
*
* @param[in]   p_ble_evt   Bluetooth stack event.
* @param[in]   p_context   Unused.
*/


static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context){
  ret_code_t  err_code = NRF_SUCCESS;
//  ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;
//  
//  const ble_gap_evt_adv_report_t * p_adv_report = &p_gap_evt->params.adv_report;
//  
//  uint8_t * aux = p_adv_report->data.p_data;
//  uint16_t aux_len = p_adv_report->data.len;
//  uint8_t aux_array[100];
//  memcpy(aux_array,aux,aux_len);
//  char data_ble[29];
//  char des_id[7];
  
  //printf("ble_evt_handler: %d\r\n",p_ble_evt->header.evt_id);
  switch (p_ble_evt->header.evt_id)
  {
//      case BLE_GAP_EVT_ADV_REPORT:
//        {
//    
//          //       for(int i = 0;i<aux_len;i++){
//          //         printf("%02x",aux[i]);
//          //       }
//          //       printf("\r\n");
//    
//          //sprintf(des_id,"%x%x%x%x%x%x" ,aux_array[19] >> 4, aux_array[19] & 15, aux_array[20] >> 4, aux_array[20] & 15, aux_array[21] >> 4, aux_array[21] & 15);    
//          //if( ((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (des_id[0] == id[0] ) && (des_id[1] == id[1]) && (des_id[2] == id[2]) && (des_id[3] == id[3]) && (des_id[4] == id[4]) && (des_id[5] == id[5])) || ((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (aux_array[15] == 0x54) && (aux_array[19] == 0x00) && (aux_array[20] == 0x00) && (aux_array[21] == 0x00)) ){        
//           //if( ((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (aux_array[15] == 0x54) && (aux_array[16] != 0x00) && (aux_array[17] != 0x00) && (aux_array[18] != 0x00) && (aux_array[19] == 0x00) && (aux_array[20] == 0x00) && (aux_array[21] == 0x00)) || ((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (des_id[0] == id[0] ) && (des_id[1] == id[1]) && (des_id[2] == id[2]) && (des_id[3] == id[3]) && (des_id[4] == id[4]) && (des_id[5] == id[5])) ) {
//           //if( ((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (aux_array[15] == 0x54) && (aux_array[19] == 0x00) && (aux_array[20] == 0x00) && (aux_array[21] == 0x00)) || ((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (des_id[0] == id[0]) && (des_id[1] == id[1]) && (des_id[2] == id[2]) && (des_id[3] == id[3]) && (des_id[4] == id[4]) && (des_id[5] == id[5])) ) {
//           if((aux_array[13] == 0x41) && (aux_array[14] == 0x50) && (aux_array[15] == 0x54)){
//            //printf("des_id = %s\r\n",des_id);
//            //printf("id = %c%c%c%c%c%c \r\n",id[0],id[1],id[2],id[3],id[4],id[5]);
//             
//            /*         
//            
//            //NRF_LOG_RAW_INFO("RSSI2: %d\n", p_gap_evt->params.adv_report.rssi);
//            char data_ble[25];
//            sprintf(data_ble,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",p_adv_report->peer_addr.addr[5]
//            ,p_adv_report->peer_addr.addr[4]
//            ,p_adv_report->peer_addr.addr[3]
//            ,p_adv_report->peer_addr.addr[2]
//            ,p_adv_report->peer_addr.addr[1]
//            ,p_adv_report->peer_addr.addr[0]
//            ,aux_array[23]
//            ,aux_array[24]
//            ,aux_array[25]
//            ,aux_array[26]
//            ,aux_array[27]    
//            ,aux_array[28]);
//             */
//            
//            //datacenter
//             /*
//             char data_ble[25];
//             sprintf(data_ble,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02d",aux_array[16]                          //src_id
//                                                                            ,aux_array[17]                          //src_id
//                                                                            ,aux_array[18]                          //src_id        
//                                                                            ,aux_array[23]                          //timestamp
//                                                                            ,aux_array[24]                          //timestamp
//                                                                            ,aux_array[25]                          //timestamp
//                                                                            ,aux_array[26]                          //cnt
//                                                                            ,aux_array[27]                          //activity    
//                                                                            ,aux_array[28]                          //batt
//                                                                            ,aux_array[15]                          //type
//                                                                              //,p_adv_report->rssi);
//                                                                            ,p_gap_evt->params.adv_report.rssi);    //rssi
//             */
//            
//             sprintf(data_ble,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02d",aux_array[16]                          //src_id
//                                                                                    ,aux_array[17]                          //src_id
//                                                                                    ,aux_array[18]                          //src_id    
//                                                                                    ,aux_array[19]                          //variance_0   
//                                                                                    ,aux_array[20]                          //variance_1   
//                                                                                    ,aux_array[21]                          //variance_2
//                                                                                    ,aux_array[23]                          //cnt
//                                                                                    ,aux_array[24]                          //vt_0
//                                                                                    ,aux_array[25]                          //vt_1
//                                                                                    ,aux_array[26]                          //vt_2
//                                                                                    ,aux_array[27]                          //activity    
//                                                                                    ,aux_array[28]                          //batt
//                                                                                    //p_adv_report->rssi);
//                                                                                    ,p_gap_evt->params.adv_report.rssi);    //rssi
//             
//             //printf("aux 23 : %02x \r\n",aux_array[23]);
//             //printf("%s\r\n",data_ble);
//             
//             ret_code_t err_code = my_queue_write(data_ble, sizeof(data_ble));     
//             APP_ERROR_CHECK(err_code);
//             //memset(data_ble,0,sizeof(data_ble));
//             round_evt++;
//             
//             //memset(data_ble, 0, sizeof(data_ble));     
//             //}
//           } 
//        }
//        break;
//    
  case BLE_GAP_EVT_DISCONNECTED:
    printf("Disconnected.\r\n");
    // LED indication will be changed when advertising starts.
    break;
    
  case BLE_GAP_EVT_CONNECTED:
    printf("Connected.\r\n");
    err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
    APP_ERROR_CHECK(err_code);
    m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
    APP_ERROR_CHECK(err_code);
    break;
    
  case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
      NRF_LOG_DEBUG("PHY update request.");
      ble_gap_phys_t const phys =
      {
        .rx_phys = BLE_GAP_PHY_AUTO,
        .tx_phys = BLE_GAP_PHY_AUTO,
      };
      err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
      APP_ERROR_CHECK(err_code);
    } break;
    
  case BLE_GATTC_EVT_TIMEOUT:
    // Disconnect on GATT Client timeout event.
    NRF_LOG_DEBUG("GATT Client Timeout.");
    err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                     BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
    break;
    
  case BLE_GATTS_EVT_TIMEOUT:
    // Disconnect on GATT Server timeout event.
    NRF_LOG_DEBUG("GATT Server Timeout.");
    err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                     BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
    break;
    
  default:
    break;
  }
}


/**@brief Function for initializing the BLE stack.
*
* @details Initializes the SoftDevice and the BLE event interrupt.
*/

static void ble_stack_init(void)
{
  ret_code_t err_code;
  
  err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);
  
  // Configure the BLE stack using the default settings.
  // Fetch the start address of the application RAM.
  uint32_t ram_start = 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
  APP_ERROR_CHECK(err_code);
  
  // Enable BLE stack.
  err_code = nrf_sdh_ble_enable(&ram_start);
  APP_ERROR_CHECK(err_code);
  
  // Register a handler for BLE events.
  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for handling events from the GATT library. */
//
//void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
//{
//if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
//{
//NRF_LOG_INFO("ATT MTU exchange completed.");
//
//m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
//NRF_LOG_INFO("Ble NUS max data length set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
//    }
//}


/**@brief Function for initializing the GATT library. */


void gatt_init(void)
{
  
  ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
  APP_ERROR_CHECK(err_code);
  //ret_code_t err_code;
  //
  //err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
  //APP_ERROR_CHECK(err_code);
  //
  //err_code = nrf_ble_gatt_att_mtu_central_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
  //APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */

static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
  ret_code_t                         err_code;
  ble_atapy_init_t                     atapy_init= {0};
  nrf_ble_qwr_init_t qwr_init = {0};
  
  qwr_init.error_handler = nrf_qwr_error_handler;
  
  err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);
  
  
  // Initialize CUS Service init structure to zero.
  memset(&atapy_init, 0, sizeof(atapy_init));
  
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&atapy_init.custom_value_char_attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&atapy_init.custom_value_char_attr_md.write_perm);
  
  err_code = ble_atapy_init(&m_atapy, &atapy_init);
  APP_ERROR_CHECK(err_code);	
  
  
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            //NRF_LOG_INFO("Fast advertising.");
            printf("Fast advertising.\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    
    init.evt_handler = on_adv_evt;
    
    err_code = ble_advertising_init(&m_advertising, &init);
    NRF_LOG_DEBUG("ble_advertising_init returned error %x \r\n",err_code)
    if(err_code != NRF_SUCCESS)
    {
      NRF_LOG_DEBUG("ble_advertising_init returned error %x \r\n",err_code)
    }
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start(false);
            break;

        default:
            break;
    }
}




/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


//
//
//static void gatt_init(void)
//{
//ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
//APP_ERROR_CHECK(err_code);
//}

/**@brief Function for handling events from the BSP module.
*
* @param[in] event  Event generated by button press.
*/
//void bsp_event_handler(bsp_event_t event)
//{
//    ret_code_t err_code;
//
//    switch (event)
//    {
//        case BSP_EVENT_SLEEP:
//            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
//            break;
//
//        case BSP_EVENT_DISCONNECT:
//            err_code = sd_ble_gap_disconnect(m_ble_nus_c.conn_handle,
//                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
//            if (err_code != NRF_ERROR_INVALID_STATE)
//            {
//                APP_ERROR_CHECK(err_code);
//            }
//            break;
//
//        default:
//            break;
//    }
//}

/**@brief Function for initializing the UART. */
static void uart_init(void)
{
  ret_code_t err_code;
  
  app_uart_comm_params_t const comm_params =
  {
    .rx_pin_no    = RX_PIN_NUMBER,
    .tx_pin_no    = TX_PIN_NUMBER,
    .rts_pin_no   = RTS_PIN_NUMBER,
    .cts_pin_no   = CTS_PIN_NUMBER,
    .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
    .use_parity   = false,
    .baud_rate    = UART_BAUDRATE_BAUDRATE_Baud230400
  };
  
  APP_UART_FIFO_INIT(&comm_params,
                     UART_RX_BUF_SIZE,
                     UART_TX_BUF_SIZE,
                     uart_event_handle,
                     APP_IRQ_PRIORITY_LOWEST,
                     err_code);
  
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Nordic UART Service (NUS) client. */


//static void nus_c_init(void)
//{
//    ret_code_t       err_code;
//    ble_nus_c_init_t init;
//
//    init.evt_handler = ble_nus_c_evt_handler;
//
//    err_code = ble_nus_c_init(&m_ble_nus_c, &init);
//    APP_ERROR_CHECK(err_code);
//}


/**@brief Function for initializing buttons and leds. */
//static void buttons_leds_init(void)
//{
//    ret_code_t err_code;
//    bsp_event_t startup_event;
//
//    err_code = bsp_init(BSP_INIT_LEDS, bsp_event_handler);
//    APP_ERROR_CHECK(err_code);
//
//    err_code = bsp_btn_ble_init(NULL, &startup_event);
//    APP_ERROR_CHECK(err_code);
//}


/**@brief Function for initializing the nrf log module. */
static void log_init(void)
{
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
*/
static void power_management_init(void)
{
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}


///** @brief Function for initializing the database discovery module. */
//static void db_discovery_init(void)
//{
//   ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
//    APP_ERROR_CHECK(err_code);
//}


/**@brief Function for handling the idle state (main loop).
*
* @details Handles any pending log operations, then sleeps until the next event occurs.
*/
static void idle_state_handle(void)
{
  if (NRF_LOG_PROCESS() == false)
  {
    nrf_pwr_mgmt_run();
  }
}



/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
static void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETED_SUCEEDED event
    }
    else
    {
        ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);

        
        NRF_LOG_INFO("Start!");
        
        APP_ERROR_CHECK(err_code);
    }
}

void cus_timer_start(cus_timer_t timer, uint32_t ms)
{
  ret_code_t err_code;
  
  switch(timer)
  {    
    case CHK_SCAN_TIMER:
    {
      err_code = app_timer_start(m_chk_scan_id, TIMER_CHK_SCAN_INTERVAL, NULL);
      APP_ERROR_CHECK(err_code);
      break;
    }      
  default:
    break;
  }
}

/**@brief Function for starting default timers.
*/
void application_timers_start(void)
{
  cus_timer_start(CHK_SCAN_TIMER,NULL);    //start timer for reading sensor
}



/**@brief Function for stoping repeating timers.
*/
void cus_timer_stop(cus_timer_t timer)
{
  switch(timer)
  {
  case CHK_SCAN_TIMER:
    {
      app_timer_stop(m_chk_scan_id);
      break;
    }
  case ALL:
    {
      //app_timer_stop(m_chk_scan_id);
    }   
  default:
    break;
  }
}

void scan_timeout_handler(void * p_context)
{
  
  if(chk_scan.chk_round == false)
  {    
    chk_scan.cnt++;
    
    if (chk_scan.cnt >= SCAN_NOT_FOUND_10SEC)
    {
      
      if(chk_scan.flag_slow == false)
      {
        
        //chk_scan.flag_stop_scan = true;
        chk_scan.flag_slow = true;
        
        nrf_ble_scan_stop();          
        nrf_ble_scan_params_set(&m_scan,&m_scan_param_slow);
        scan_start();
        
      }   
    }
  }
  else
  {
    chk_scan.cnt = 0;
    
    if(chk_scan.flag_slow == true)
    {
      nrf_ble_scan_stop();          
      //scan_init();
      nrf_ble_scan_params_set(&m_scan,&m_scan_param_normal);
      scan_start();
   
      chk_scan.flag_slow  = false;
    }
  }
}


void timers_init(void)
{
  ret_code_t err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);
  
  //timer for check empty scan
  err_code = app_timer_create(&m_chk_scan_id,
                              APP_TIMER_MODE_REPEATED,
                              scan_timeout_handler);
  APP_ERROR_CHECK(err_code); 
  
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break; // BSP_EVENT_SLEEP

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break; // BSP_EVENT_KEY_0

        default:
            break;
    }
}

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


 int main(void)
{
 
  bool erase_bonds;
  
  // Initialize.
  log_init();
  timers_init();
  uart_init();
  //buttons_leds_init();
  buttons_leds_init(&erase_bonds);
  //db_discovery_init();
  power_management_init();
  ble_stack_init();
  gap_params_init();
  gatt_init();
  services_init();
  advertising_init();
  conn_params_init();   
  peer_manager_init();
  scan_init();
  //nus_c_init();
  //application_timers_start();

  /* Start execution */
  //queue_clear(&uart_mesg);
  
  /*if((var_wifi.ssid_len == 0) || (var_wifi.pass_len == 0))
  {  
    advertising_start(erase_bonds); 
    while(true){
      if((var_wifi.ssid_len != 0) && (var_wifi.pass_len != 0)) break;
    }
    
  }*/
 
    scan_start();
    //application_timers_start();
  
  
  // Enter main loop.
  
  for (;;)
  {
    idle_state_handle();
  }
}