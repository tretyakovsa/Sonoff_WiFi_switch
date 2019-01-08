#ifndef HW_TIMER_H
#define HW_TIMER_H

#ifdef  __cplusplus
extern "C" {
#endif

#define US_TO_RTC_TIMER_TICKS(t)          \
    ((t) ?                                   \
     (((t) > 0x35A) ?                   \
      (((t)>>2) * ((APB_CLK_FREQ>>4)/250000) + ((t)&0x3) * ((APB_CLK_FREQ>>4)/1000000))  :    \
      (((t) *(APB_CLK_FREQ>>4)) / 1000000)) :    \
     0)

#define FRC1_ENABLE_TIMER  BIT7
#define FRC1_AUTO_LOAD  BIT6

//TIMER PREDIVED MODE
typedef enum {
    DIVDED_BY_1 = 0,    //timer clock
    DIVDED_BY_16 = 4, //divided by 16
    DIVDED_BY_256 = 8,  //divided by 256
} TIMER_PREDIVED_MODE;

typedef enum {      //timer interrupt mode
    TM_LEVEL_INT = 1, // level interrupt
    TM_EDGE_INT   = 0,  //edge interrupt
} TIMER_INT_MODE;

typedef enum {
    FRC1_SOURCE = 0,
    NMI_SOURCE = 1,
} FRC1_TIMER_SOURCE_TYPE;


void  hw_timer_arm(u32 val);
void  hw_timer_set_func(void (* user_hw_timer_cb_set)(void));
void  hw_timer_init(FRC1_TIMER_SOURCE_TYPE source_type, u8 req);
//void ICACHE_RAM_ATTR blink_gpio(void);
//volatile bool state;



#ifdef  __cplusplus
}
#endif

#endif  /* HW_TIMER_H */
