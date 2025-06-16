#ifndef __data_h
#define __data_h

#define PIN_READ D4

extern bool publish_data_to_mqtt(void*);
extern bool check_button(void*);
extern bool run_alarm(void*);

#endif // __data_h