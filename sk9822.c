#include "gpiohs.h"
#include "sleep.h"
#include "sk9822.h"
#include "fpioa.h"

#define SK9822_LED_NUMBER                (12)

#define SK9822_DAT_SET()                 \
    {                                    \
        gpiohs->output_val.bits.b20 = 1; \
    }

#define SK9822_DAT_CLR()                 \
    {                                    \
        gpiohs->output_val.bits.b20 = 0; \
    }

#define SK9822_CLK_SET()                 \
    {                                    \
        gpiohs->output_val.bits.b21 = 1; \
    }

#define SK9822_CLK_CLR()                 \
    {                                    \
        gpiohs->output_val.bits.b21 = 0; \
    }

//储存每个灯的颜色
static sk9822_color_t led_strip_buff[SK9822_LED_NUMBER] = {0};

//初始化
void sk9822_init(void)
{
    // sk9822 pin
    fpioa_set_function(24, FUNC_GPIOHS20);
    fpioa_set_function(25, FUNC_GPIOHS21);

    gpiohs_set_drive_mode(20, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(21, GPIO_DM_OUTPUT);
    gpiohs_set_pin(20, 0);
    gpiohs_set_pin(21, 0);
}

//发送数据
void sk9822_send_data(uint32_t data)
{
    for (uint32_t mask = 0x80000000; mask > 0; mask >>= 1)
    {
        SK9822_CLK_CLR();
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        // usleep(1);
        if (data & mask)
        {
            SK9822_DAT_SET();
        }
        else
        {
            SK9822_DAT_CLR();
        }
        SK9822_CLK_SET();
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        // usleep(2);
    }
}

//开始发送数据
void sk9822_start_frame(void)
{
    sk9822_send_data(0);
}

//发送数据结束
void sk9822_stop_frame(void)
{
    sk9822_send_data(0xffffffff);
}


// 所有灯同时亮同一种颜色 color:0xffbbggrr
void sk9822_all_on(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i=0; i < SK9822_LED_NUMBER; i++)
    {
        led_strip_buff[i].gray = 0xff;
        led_strip_buff[i].red = r;
        led_strip_buff[i].green = g;
        led_strip_buff[i].blue = b;
    }
    sk9822_update_display();
}

// 直接关闭所有灯
void sk9822_all_off(void)
{
    sk9822_clear();
    sk9822_update_display();
}

// 拼接成32位RGB值
uint32_t sk9822_get_color_data(uint8_t gray, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t tosend;

    gray &= 0x1f; //for make sure no error data in
    tosend = ((0xe0 | gray) << 24) | (b << 16) | (g << 8) | r;
    return tosend;
}

// 灯条移动
static void arraymove(uint32_t array[], uint8_t len)
{
    uint8_t index;
    uint32_t tmp;

    tmp = array[0];
    for (index = 0; index < len - 1; index++)
    {
        array[index] = array[index + 1];
    }
    array[len - 1] = tmp;
}


// 跑马灯
void sk9822_horse_race(uint8_t r, uint8_t g, uint8_t b, uint32_t interval)
{
    uint32_t led_buff[SK9822_LED_NUMBER] = {0};
    uint8_t i;

    for (i = 0; i < SK9822_LED_NUMBER; i++)
    {
        led_buff[i] = 0xff000000;
    }

    for (i = 0; i < SK9822_LED_NUMBER/2; i++)
    {
        led_buff[i] = sk9822_get_color_data((0xe0 | (32 - i * 4)), r, g, b);
    }

    while (1)
    {
        sk9822_start_frame();
        for (i = 0; i < SK9822_LED_NUMBER; i++)
        {
            sk9822_send_data(led_buff[i]);
        }
        sk9822_stop_frame();
        arraymove(led_buff, SK9822_LED_NUMBER);
        msleep(interval);
    }
}


// 呼吸灯
void sk9822_breath(uint8_t r, uint8_t g, uint8_t b, uint32_t interval)
{
    uint8_t index, cnt, dir;
    uint32_t color = sk9822_get_color_data(0xff, r, g, b);
    cnt = 0;
    dir = 1;

    while (1)
    {
        if (cnt >= 30)
        {
            dir = !dir;
            cnt = 0;
        }
        cnt++;
        color = sk9822_get_color_data((0xe0 | (dir ? cnt : 31 - cnt)), r, g, b);
        sk9822_start_frame();
        for (index = 0; index < SK9822_LED_NUMBER; index++)
        {
            sk9822_send_data(color);
        }
        sk9822_stop_frame();
        msleep(interval);
    }
}

// 设置灯的颜色
static void sk9822_set_led(sk9822_color_t strip_buff)
{
    uint32_t color;

    strip_buff.gray &= 0x1f;
    color = ((0xe0 | strip_buff.gray) << 24) | (strip_buff.blue << 16) |  \
    (strip_buff.green << 8) | strip_buff.red;
    sk9822_send_data(color);
}

// 刷新显示
void sk9822_update_display(void)
{
    uint8_t i = 0;
    sk9822_start_frame();
    for (i = 0; i < SK9822_LED_NUMBER; i++)
    {
        sk9822_set_led(led_strip_buff[i]);
    }
    sk9822_stop_frame();
}

// 清除led_strip_buff，运行update才会熄灭所有灯
void sk9822_clear(void)
{
    for (int i = 0; i < SK9822_LED_NUMBER; i++)
    {
        sk9822_set_pixel_color(i, 0, 0, 0);
    }
}

// 单独设置某个灯的颜色,update后才显示
void sk9822_set_pixel_color(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b)
{
    if (pixel >= 0 && pixel < SK9822_LED_NUMBER)
    {
        led_strip_buff[pixel].gray = 0xff;
        led_strip_buff[pixel].red = r;
        led_strip_buff[pixel].green = g;
        led_strip_buff[pixel].blue = b;
    }
}

