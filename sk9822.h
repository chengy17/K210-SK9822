#ifndef _SK9822_H_
#define _SK9822_H_
#include <stdint.h>

typedef struct {
	uint8_t gray;	
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} sk9822_color_t;

void sk9822_init(void);

void sk9822_start_frame(void);
void sk9822_stop_frame(void);
void sk9822_send_data(uint32_t data);

void sk9822_all_off(void);
void sk9822_all_on(uint8_t r, uint8_t g, uint8_t b);
uint32_t sk9822_get_color_data(uint8_t gray, uint8_t r, uint8_t g, uint8_t b);

void sk9822_set_pixel_color(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void sk9822_clear(void);
void sk9822_update_display(void);

void sk9822_horse_race(uint8_t r, uint8_t g, uint8_t b, uint32_t interval);
void sk9822_breath(uint8_t r, uint8_t g, uint8_t b, uint32_t interval);

#endif /* _SK9822_H_ */
