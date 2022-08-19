#ifndef _CUSTOM_TIMER_H_
#define _CUSTOM_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_timer.h"
#include "nrf_ble_scan.h"
#include "defs.h"
#include "nrf_ble_scan.h"

//#define MIN                     60000
//#define TIMER_CHK_SCAN          6000

//#define SCAN_NOT_FOUND_1MIN     1  
//#define SCAN_NOT_FOUND_5MIN     5      
//#define SCAN_NOT_FOUND_10MIN    10
//#define SCAN_NOT_FOUND_15MIN    15
//#define SCAN_NOT_FOUND_30MIN    30
//#define SCAN_NOT_FOUND_60MIN    60

//#define OUT_OF_SCAN              (SCAN_NOT_FOUND_5MIN * MIN) / TIMER_CHK_SCAN
//APP_TIMER_DEF(m_chk_scan_id);
//#define TIMER_CHK_SCAN_INTERVAL                     APP_TIMER_TICKS(TIMER_CHK_SCAN)


APP_TIMER_DEF(m_chk_scan_id); 
#define TIMER_CHK_SCAN_INTERVAL                     APP_TIMER_TICKS(6000)

#define SCAN_NOT_FOUND_5SEC     50      
#define SCAN_NOT_FOUND_10SEC    100
#define SCAN_NOT_FOUND_15SEC    150
#define SCAN_NOT_FOUND_30SEC    300


typedef enum
{
  CHK_SCAN_TIMER,
  ALL = 255
}cus_timer_t;

void scan_timeout_handler(void * p_context);
void timers_init(void);
void application_timers_start(void);
void cus_timer_start(cus_timer_t timer, uint32_t ms);
void cus_timer_stop(cus_timer_t timer);

#endif