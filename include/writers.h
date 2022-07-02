#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <iostream>

#ifndef _WRITERS_H
#define _WRITERS_H

#define E_DELAY 20
#define E_BIT 0x04

using namespace std;

class Writer {
    public:
        void virtual send_byte(char byte, bool reg) = 0;
        void virtual backlight_on() = 0;
        void virtual backlight_off() = 0;
};

class i2cWriter : public Writer {
    public:

        i2cWriter();
        i2cWriter(i2c_inst_t* i2cport, uint8_t i2caddr, uint8_t sda_pin, uint8_t scl_pin) {

            i2cWriter::i2cport = i2cport;
            i2cWriter::i2caddr = i2caddr;

            i2c_init(i2cport, 100 * 1000);
            gpio_set_function(scl_pin, GPIO_FUNC_I2C);
            gpio_set_function(sda_pin, GPIO_FUNC_I2C);
            gpio_pull_up(scl_pin);
            gpio_pull_up(sda_pin);
        };

        void send_byte(char byte, bool reg) {

            uint8_t high = reg | (byte & 0xF0) | backlight << 3;
            uint8_t low = reg | ((byte << 4) & 0xF0) | backlight << 3;

            i2c_write_byte(high);
            lcd_toggle_enable(high);
            i2c_write_byte(low);
            lcd_toggle_enable(low);
        };

        void backlight_on() {
            if(backlight) {
                return;
            }
            backlight = true;
            i2c_write_byte(0b00001000);
        };

        void backlight_off() {
            if(backlight) {
                backlight = false;
                i2c_write_byte(0b00000000);
            }
        };

    private:

        inline void i2c_write_byte(uint8_t byte) {
            i2c_write_blocking(i2cport, i2caddr, &byte, 1, false);
        };

        inline void lcd_toggle_enable(uint8_t val) {
            sleep_us(E_DELAY);
            i2c_write_byte(val | E_BIT);
            sleep_us(E_DELAY);
            i2c_write_byte(val & ~E_BIT);
            sleep_us(E_DELAY);
        };

        bool backlight;
        i2c_inst_t* i2cport;
        uint8_t i2caddr;
};

class parallel4Writer : public Writer {
    public:

        parallel4Writer();
        //Pin 255 indicates no backlight control
        parallel4Writer(int rs_pin, int en_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, int backlight_anode_pin = 255) {

            parallel4Writer::rs_pin = rs_pin;
            parallel4Writer::en_pin = en_pin;
            parallel4Writer::d4_pin = d4_pin;
            parallel4Writer::d5_pin = d5_pin;
            parallel4Writer::d6_pin = d6_pin;
            parallel4Writer::d7_pin = d7_pin;

            gpio_init(rs_pin);
            gpio_set_dir(rs_pin, true);
            gpio_init(en_pin);
            gpio_set_dir(en_pin, true);
            gpio_init(d4_pin);
            gpio_set_dir(d4_pin, true);
            gpio_init(d5_pin);
            gpio_set_dir(d5_pin, true);
            gpio_init(d6_pin);
            gpio_set_dir(d6_pin, true);
            gpio_init(d7_pin);
            gpio_set_dir(d7_pin, true);

            if(backlight_anode_pin != 255) {
                backlight_controllable = true;
                parallel4Writer::backlight_pin = backlight_anode_pin;
                gpio_init(backlight_anode_pin);
                gpio_set_dir(backlight_anode_pin, true);
            }

        };
        void send_byte(char byte, bool reg) {
            gpio_put(rs_pin, reg);
            sleep_us(E_DELAY);

            //High nibble
            gpio_put(d7_pin, byte & 0b10000000);
            gpio_put(d6_pin, byte & 0b01000000);
            gpio_put(d5_pin, byte & 0b00100000);
            gpio_put(d4_pin, byte & 0b00010000);
            sleep_us(E_DELAY);
            gpio_put(en_pin, true);
            sleep_us(E_DELAY);
            gpio_put(en_pin, false);

            sleep_us(E_DELAY);

            //Low nibble
            gpio_put(d7_pin, byte & 0b00001000);
            gpio_put(d6_pin, byte & 0b00000100);
            gpio_put(d5_pin, byte & 0b00000010);
            gpio_put(d4_pin, byte & 0b00000001);
            sleep_us(E_DELAY);
            gpio_put(en_pin, true);
            sleep_us(E_DELAY);
            gpio_put(en_pin, false);
        };

        void backlight_on() {
            if(backlight_controllable) {
                gpio_put(backlight_pin, true);
            }
        };

        void backlight_off() {
            if(backlight_controllable) {
                gpio_put(backlight_pin, false);
            }
        };
    
    private:

        bool backlight_controllable = false;

        int rs_pin;
        int en_pin;
        int d4_pin;
        int d5_pin;
        int d6_pin;
        int d7_pin;
        int backlight_pin;
};

class parallel8Writer : public Writer {
    public:

        parallel8Writer();
        parallel8Writer(int rs_pin, int en_pin, int d0_pin, int d1_pin, int d2_pin, int d3_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, int backlight_anode_pin = 255) {

            parallel8Writer::rs_pin = rs_pin;
            parallel8Writer::en_pin = en_pin;
            parallel8Writer::d0_pin = d0_pin;
            parallel8Writer::d1_pin = d1_pin;
            parallel8Writer::d2_pin = d2_pin;
            parallel8Writer::d3_pin = d3_pin;
            parallel8Writer::d4_pin = d4_pin;
            parallel8Writer::d5_pin = d5_pin;
            parallel8Writer::d6_pin = d6_pin;
            parallel8Writer::d7_pin = d7_pin;

            gpio_init(rs_pin);
            gpio_set_dir(rs_pin, true);
            gpio_init(en_pin);
            gpio_set_dir(en_pin, true);
            gpio_init(d0_pin);
            gpio_set_dir(d0_pin, true);
            gpio_init(d1_pin);
            gpio_set_dir(d1_pin, true);
            gpio_init(d2_pin);
            gpio_set_dir(d2_pin, true);
            gpio_init(d3_pin);
            gpio_set_dir(d3_pin, true);
            gpio_init(d4_pin);
            gpio_set_dir(d4_pin, true);
            gpio_init(d5_pin);
            gpio_set_dir(d5_pin, true);
            gpio_init(d6_pin);
            gpio_set_dir(d6_pin, true);
            gpio_init(d7_pin);
            gpio_set_dir(d7_pin, true);

            if(backlight_anode_pin != 255) {
                backlight_controllable = true;
                parallel8Writer::backlight_pin = backlight_anode_pin;
                gpio_init(backlight_anode_pin);
                gpio_set_dir(backlight_anode_pin, true);
            }
        };

        void send_byte(char byte, bool reg) {
            gpio_put(rs_pin, reg);
            sleep_us(E_DELAY);

            gpio_put(d7_pin, byte & 0b10000000);
            gpio_put(d6_pin, byte & 0b01000000);
            gpio_put(d5_pin, byte & 0b00100000);
            gpio_put(d4_pin, byte & 0b00010000);
            gpio_put(d3_pin, byte & 0b00001000);
            gpio_put(d2_pin, byte & 0b00000100);
            gpio_put(d1_pin, byte & 0b00000010);
            gpio_put(d0_pin, byte & 0b00000001);
            sleep_us(E_DELAY);
            gpio_put(en_pin, true);
            sleep_us(E_DELAY);
            gpio_put(en_pin, false);
        };

        void backlight_on() {
            if(backlight_controllable) {
                gpio_put(backlight_pin, true);
            }
        };

        void backlight_off() {
            if(backlight_controllable) {
                gpio_put(backlight_pin, false);
            }
        };

    private:

        bool backlight_controllable = false;

        int rs_pin;
        int en_pin;
        int d0_pin;
        int d1_pin;
        int d2_pin;
        int d3_pin;
        int d4_pin;
        int d5_pin;
        int d6_pin;
        int d7_pin;
        int backlight_pin;
};

#endif