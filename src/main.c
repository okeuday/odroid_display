/* -*- coding: utf-8; Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
 * ex: set softtabstop=4 tabstop=4 shiftwidth=4 expandtab fileencoding=utf-8:
 *
 * BSD LICENSE
 * 
 * Copyright (c) 2015, Michael Truog <mjtruog at gmail dot com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * All advertising materials mentioning features or use of this
 *       software must display the following acknowledgment:
 *         This product includes software developed by Michael Truog
 *     * The name of the author may not be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include "cloudi.h"
// depends on https://github.com/hardkernel/wiringPi
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <lcd.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "utf8_to_hd44780u.h"

static int display_handle = 0;
static unsigned char (*display_character)(unsigned char const ** const);
 
static int const display_16_2_rows = 2;
static int const display_16_2_columns = 16;
static int const display_16_2_port_led[] = {
    21, // GPIOX.BIT4(#101)
    22, // GPIOX.BIT3(#100)
    23, // GPIOX.BIT11(#108):PWM_B
    24, // GPIOX.BIT0(#97)
    11, // GPIOX.BIT21(#118)
    26, // GPIOX.BIT2(#99)
    27, // GPIOX.BIT1(#98)
};
static int const display_16_2_port_led_count = sizeof(display_16_2_port_led) /
                                               sizeof(display_16_2_port_led[0]);
 
void display_16_2_initialize(int const hd44780u_character_set)
{
    int const bus = 4; // interface 4 bit mode
    int const port_lcd_rs = 7; // GPIOY.BIT3(#83)
    int const port_lcd_e = 0;  // GPIOY.BIT8(#88)
    int const port_lcd_d4 = 2; // GPIOX.BIT19(#116)
    int const port_lcd_d5 = 3; // GPIOX.BIT18(#115)
    int const port_lcd_d6 = 1; // GPIOY.BIT7(#87)
    int const port_lcd_d7 = 4; // GPIOX.BIT7(#104)
    int const port_button1 = 5; // GPIOX.BIT5(#102)
    int const port_button2 = 6; // GPIOX.BIT6(#103)
    int i;
    display_handle = lcdInit(display_16_2_rows, display_16_2_columns, bus,
                             port_lcd_rs, port_lcd_e,
                             port_lcd_d4, port_lcd_d5, port_lcd_d6, port_lcd_d7,
                             0, 0, 0, 0);
 
    if (display_handle < 0)
    {
        fprintf(stderr, "lcdInit failed!\n");
        exit(1);
    }
 
    // GPIO Init (all output for LED ports)
    assert(display_16_2_port_led_count <= 8);
    for (i = 0; i < display_16_2_port_led_count; i++)
    {
        pinMode(display_16_2_port_led[i], OUTPUT);
        pullUpDnControl(port_button1, PUD_OFF);
    }

    switch (hd44780u_character_set)
    {
        case 0:
            display_character = &utf8_to_hd44780u_0;
            break;
        default:
            fprintf(stderr, "hd44780u character set unknown: %d\n",
                    hd44780u_character_set);
            exit(1);
    }
}

void display_16_2_update(unsigned char const * const request,
                         uint32_t const request_size)
{
    uint8_t const * const leds = (uint8_t *) request;
    uint8_t const leds_off = leds[0];
    uint8_t const leds_on = leds[1];
    uint8_t const leds_toggle = leds[2];
    unsigned char const * message = &(request[3]);
    unsigned char const * message_end = &(request[request_size]);
    int i, j, reposition;
    unsigned char c;

    /* update LEDs */
    if (leds_off || leds_on || leds_toggle)
    {
        for (i = 0; i < display_16_2_port_led_count; i++)
        {
            j = 1 << i;
            if (j & leds_off)
            {
                digitalWrite(display_16_2_port_led[i], 0);
            }
            else if (j & leds_on)
            {
                digitalWrite(display_16_2_port_led[i], 1);
            }
            else if (j & leds_toggle)
            {
                digitalWrite(display_16_2_port_led[i],
                             1 - digitalRead(display_16_2_port_led[i]));
            }
        }
    }

    /* update LCD */
    for (i = 0; i < display_16_2_rows && message != message_end; i++)
    {
        reposition = 0;
        lcdPosition(display_handle, 0, i);
        for (j = 0; j < display_16_2_columns && message != message_end; j++)
        {
            c = (*display_character)(&message);
            if (c == '\0')
            {
                reposition = 1;
            }
            else
            {
                if (reposition)
                {
                    reposition = 0;
                    lcdPosition(display_handle, j, i);
                }
                lcdPutchar(display_handle, c);
            }
        }
    }

}

void display_16_2_merge(unsigned char const * const request,
                        uint32_t const request_size)
{
    uint8_t leds_off = 0;
    uint8_t leds_on = 0;
    uint8_t leds_toggle = 0;
    unsigned char message[display_16_2_columns * display_16_2_rows] =
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char * message_p;
    unsigned char * message_end;
    uint32_t message_size = 0;
    int i, j, reposition;
    unsigned char c;

    /* merge separate requests for the display
     * (last entry is on top)
     */
    for (i = 0; i < request_size; i += message_size)
    {
        message_size = *((uint32_t *) &request[i]) - 3;
        i += 4;
        leds_off |= *((uint8_t *) &request[i]);
        i++;
        leds_on |= *((uint8_t *) &request[i]);
        i++;
        leds_toggle |= *((uint8_t *) &request[i]);
        i++;
        message_p = &request[i];
        message_end = &message_p[message_size];
        for (j = 0; message_p != message_end; j++)
        {
            c = (*display_character)(&message_p);
            if (c == '\0')
                continue;
            if (c == ' ')
            {
                if (message[j] == '\0')
                {
                    message[j] = c;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                message[j] = c;
            }
        }
    }

    /* update LEDs */
    if (leds_off || leds_on || leds_toggle)
    {
        for (i = 0; i < display_16_2_port_led_count; i++)
        {
            j = 1 << i;
            if (j & leds_off)
            {
                digitalWrite(display_16_2_port_led[i], 0);
            }
            else if (j & leds_on)
            {
                digitalWrite(display_16_2_port_led[i], 1);
            }
            else if (j & leds_toggle)
            {
                digitalWrite(display_16_2_port_led[i],
                             1 - digitalRead(display_16_2_port_led[i]));
            }
        }
    }

    /* update LCD */
    for (i = 0; i < display_16_2_rows; i++)
    {
        reposition = 0;
        lcdPosition(display_handle, 0, i);
        for (j = 0; j < display_16_2_columns; j++)
        {
            c = message[i * display_16_2_columns + j];
            if (c == '\0')
            {
                reposition = 1;
            }
            else
            {
                if (reposition)
                {
                    reposition = 0;
                    lcdPosition(display_handle, j, i);
                }
                lcdPutchar(display_handle, c);
            }
        }
    }
}

typedef enum
{
    display_16_2_a00
} display_t;

static display_t display = display_16_2_a00;

static void display_request(cloudi_instance_t * api,
                            int const command,
                            char const * const name,
                            char const * const pattern,
                            void const * const request_info,
                            uint32_t const request_info_size,
                            void const * const request,
                            uint32_t const request_size,
                            uint32_t timeout,
                            int8_t priority,
                            char const * const trans_id,
                            char const * const pid,
                            uint32_t const pid_size)
{
    switch (display)
    {
        case display_16_2_a00:
            // request: 1 byte for (7) status LEDs to off,
            //          1 byte for (7) status LEDs to on,
            //          1 byte for (7) status LEDs to toggle
            //          32 byte message (null characters are ignored),
            assert(request_size >= 3 && request_size <= (3 + 32 * 3));
            display_16_2_update((unsigned char *) request, request_size);
            break;
        default:
            assert(0);
            break;
    }
    // no response is necessary
    cloudi_return(api, command, name, pattern, "", 0, "", 0,
                  timeout, trans_id, pid, pid_size);
}

static void display_merge_request(cloudi_instance_t * api,
                                  int const command,
                                  char const * const name,
                                  char const * const pattern,
                                  void const * const request_info,
                                  uint32_t const request_info_size,
                                  void const * const request,
                                  uint32_t const request_size,
                                  uint32_t timeout,
                                  int8_t priority,
                                  char const * const trans_id,
                                  char const * const pid,
                                  uint32_t const pid_size)
{
    switch (display)
    {
        case display_16_2_a00:
            // request: 4 byte unsigned size (of the inner request)
            //          1 byte for (7) status LEDs to off,
            //          1 byte for (7) status LEDs to on,
            //          1 byte for (7) status LEDs to toggle
            //          32 byte message (null characters are ignored),
            //          (repeated as necessary)
            display_16_2_merge((unsigned char *) request, request_size);
            break;
        default:
            assert(0);
            break;
    }
    // no response is necessary
    cloudi_return(api, command, name, pattern, "", 0, "", 0,
                  timeout, trans_id, pid, pid_size);
}

static void help(char const * const command)
{
    fprintf(stderr, "Usage: %s [-h] [-d 16_2 | 16_2_a00]\n", command);
}

extern char * optarg;

int main(int argc, char ** argv)
{
    cloudi_instance_t api;
    unsigned int thread_count;
    int result = cloudi_initialize_thread_count(&thread_count);
    assert(result == cloudi_success);
    assert(thread_count == 1);
    while ((result = getopt(argc, argv, "hd:")) != -1)
    {
        switch (result)
        {
            case 'd':
                if ((strcmp("16_2", optarg) == 0) ||
                    (strcmp("16_2_a00", optarg) == 0))
                {
                    display = display_16_2_a00;
                }
                else
                {
                    fprintf(stderr, "Unknown display \"%s\"\n", optarg);
                    return 1;
                }
                break;
            case 'h':
                help(argv[0]);
                return 0;
            default:
                help(argv[0]);
                return 1;
        }
    }
    result = cloudi_initialize(&api, 0);
    assert(result == cloudi_success);
    result = cloudi_subscribe(&api, "display",
                              &display_request);
    assert(result == cloudi_success);
    result = cloudi_subscribe(&api, "display/merge",
                              &display_merge_request);
    assert(result == cloudi_success);
    wiringPiSetup();
    switch (display)
    {
        case display_16_2_a00:
            display_16_2_initialize(0);
            break;
        default:
            assert(0);
            return 1;
    }
    result = cloudi_poll(&api, -1);
    if (result != cloudi_success &&
        result != cloudi_terminate)
    {
        fprintf(stderr, "error %d\n", result);
    }
    cloudi_destroy(&api);
    return 0;
}
