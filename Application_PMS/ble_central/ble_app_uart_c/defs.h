#ifndef _DEFS_H_
#define _DEFS_H_

struct status_scan
{
  
  bool flag_empty_scan;         //check empty queue
  
  bool flag_stop_scan;          //flag trigger scan stop
  
  bool chk_round;               //check round scan
  
  bool flag_slow;               //flag trigger slow mode

  uint32_t cnt;                 //count for disable scan
  
};

extern struct status_scan chk_scan;


#endif
