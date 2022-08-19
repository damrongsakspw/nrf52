//#include <stdio.h>
//#include <stdint.h>
//#include <stdbool.h>
//#include <string.h>
//
//#include "nrf_queue.h"
//#include "nrf_ble_scan.h"
//#include "custom_timer.h"
//#include "sdk_errors.h"
//#include "nrf_delay.h"
//#include "math.h"
//#include "boards.h"
//#include "sdk_errors.h"
//#include "defs.h"
//
////static struct status_scan chk_scan;
////struct status_scan test;
//extern const nrf_queue_t m_queue;
//
////void scan_timeout_handler(void * p_context)
////{
////  //bool empty_queue = 0;
////  //static uint16_t flag_scan_interval= 0;
////  //static bool flag_stop_scan = 0;
////  static uint32_t cnt = 0;
////  //chk_scan.flag_empty_scan = nrf_queue_is_empty(&m_queue);
////  test.flag_empty_scan = nrf_queue_is_empty(&m_queue);
////  //if(chk_scan.flag_empty_scan == true)
////  if(test.flag_empty_scan == true)
////  {    
////    //chk_scan.cnt++;
////    test.cnt++;
////    //cnt = chk_scan.cnt;
////    
////    //if (chk_scan.cnt >= SCAN_NOT_FOUND_5SEC)
////    if (test.cnt >= SCAN_NOT_FOUND_5SEC)
////    {
////      //chk_scan.flag_stop_scan = 1;
////      test.flag_stop_scan = 1;
////      printf(",NODEVICE:\r\n");       
////      nrf_ble_scan_stop();  
////    }
////  }
////  else
////  {
////    //chk_scan.cnt = 0;
////    test.cnt = 0;
////  }
////  cnt = test.cnt;
////  
////}
//void scan_timeout_handler(void * p_context)
//{
//
//  static uint32_t cnt = 0;
//  if(chk_scan.chk_round == false)
//  {    
//    chk_scan.cnt++;   
//    if (chk_scan.cnt >= SCAN_NOT_FOUND_5SEC)
//    {
//      chk_scan.flag_stop_scan = 1;
//      nrf_ble_scan_stop();  
//      printf(",NODEVICE:\r\n");       
//    
//    }
//  }
//  else
//  {
//    chk_scan.cnt = 0;
//  }
//  cnt = chk_scan.cnt;
//}
//
///**@brief Function for starting default timers.
//*/
//void application_timers_start(void)
//{
//  cus_timer_start(CHK_SCAN_TIMER,NULL);    //start timer for reading sensor
//}
//
////start custom timer function 
////input ms = 0 for fixed timer interval (defined in custom_timer.h)
//void cus_timer_start(cus_timer_t timer, uint32_t ms)
//{
//  ret_code_t err_code;
//  
//  switch(timer)
//  {    
////  case CHK_SCAN_TIMER:
////    {
////      err_code = app_timer_start(m_chk_scan_id, TIMER_CHK_SCAN_INTERVAL, NULL);
////      APP_ERROR_CHECK(err_code);
////      break;
////    }      
//  default:
//    break;
//  }
//}
//
///**@brief Function for stoping repeating timers.
//*/
//void cus_timer_stop(cus_timer_t timer)
//{
//  switch(timer)
//  {
////  case CHK_SCAN_TIMER:
////    {
////      app_timer_stop(m_chk_scan_id);
////      break;
////    }
//  case ALL:
//    {
//      //app_timer_stop(m_chk_scan_id);
//    }   
//  default:
//    break;
//  }
//}
//
///**@brief Function for initializing the timer. */
//void timers_init(void)
//{
//  ret_code_t err_code = app_timer_init();
//  APP_ERROR_CHECK(err_code);
//  
//  //timer for check empty scan
////  err_code = app_timer_create(&m_chk_scan_id,
////                              APP_TIMER_MODE_REPEATED,
////                              scan_timeout_handler);
////  APP_ERROR_CHECK(err_code); 
//  
//}