#include <stdint.h>
#include <stdio.h>
#include "sleep.h"
#include "sk9822.h"

int main(void)
{
    printf("start init\n");
    sk9822_init();

    sk9822_all_on(0x00, 0x00, 0xff);
    sleep(1);
    sk9822_all_off();
    sleep(1);

    sk9822_set_pixel_color(4, 0xff, 0xff, 0x00);
    sk9822_set_pixel_color(0, 0xff, 0x00, 0x00);
    sk9822_set_pixel_color(8, 0x00, 0x00, 0x50);
    sk9822_update_display();
    sleep(1);

    sk9822_clear();
    sk9822_update_display();
    sleep(1);

    sk9822_horse_race(0xff, 0x00, 0x00, 100);
    // sk9822_breath(0xff, 0x00, 0x00, 50);

    while (1)
        ;
    return 0;
}
