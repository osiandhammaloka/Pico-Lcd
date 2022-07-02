#include "writers.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include "pico/time.h"
#include <cstdlib>
#include <vector>
#include <cstdarg>

#ifndef _DISPLAY_H
#define _DISPLAY_H

enum Register {
    COMMAND = 0,
    CHARACTER = 1
};

class Display {

    public:

        Display();
        Display(i2c_inst_t* i2cport, int sda_pin, int scl_pin, uint8_t i2c_address, uint8_t cols, uint8_t rows, int pages);
        Display(int rs_pin, int en_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, uint8_t cols, uint8_t rows, int pages);
        Display(int rs_pin, int en_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, int backlight_anode_pin, uint8_t cols, uint8_t rows, int pages);
        Display(int rs_pin, int en_pin, int d0_pin, int d1_pin, int d2_pin, int d3_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, uint8_t cols, uint8_t rows, int pages);
        Display(int rs_pin, int en_pin, int d0_pin, int d1_pin, int d2_pin, int d3_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, int backlight_anode_pin, uint8_t cols, uint8_t rows, int pages);

        void printf(const char* fmt...) {

            va_list args;
            va_start(args, fmt);

            char* buffer = (char*)malloc(cols+1);
            vsnprintf(buffer, cols+1, fmt, args);
            print(buffer);
            free(buffer);

            va_end(args);
        };
        
        template <class T>
        Display& operator<<(T x) {

            //rtti is generally bad practice, but I see no
            //other way to print pointers of arbitrary types
            if(typeid(T) == typeid(char*) || typeid(T) == typeid(const char*)) {
                printf("%s", x);
            }
            else if(typeid(T) == typeid(char) || typeid(T) == typeid(const char)) {
                printf("%c", x);
            }
            else if(std::is_integral<T>::value) {
                printf("%d", x);
            }
            else if(std::is_floating_point<T>::value) {
                printf("%g", x);
            }
            else if(std::is_pointer<T>::value) {
                printf("%p", x);
            }
            else {
                printf("?");
            }

            return *this;
        };

        //Display register methods
        void cursor_blink_on();
        void cursor_blink_off();
        void show_cursor();
        void hide_cursor();
        void display_on();
        void display_off();

        void define_character(uint8_t slot, char* character);

        void clear_page();
        void clear_all();

        //Not Implemented
        //void enable_wrap_text() {wrap_text = true;};
        //void disable_wrap_text() {wrap_text = false;};
        //bool wrap_text = false;

        void add_page();
        void add_pages(size_t num);
        void add_page_at(size_t index);
        void add_pages_at(size_t index, size_t num);

        void remove_page();
        void remove_pages(size_t num);
        void remove_page_at(size_t index);
        void remove_pages_at(size_t index, size_t num);

        void set_cursor(uint8_t col, uint8_t row);
        void set_cursor(uint8_t col, uint8_t row, size_t page);
        
        void set_target_page(size_t page);
        void set_visible_page(size_t page);

        size_t get_target_page() {return target_page;};
        size_t get_visible_page() {return visible_page;};

        void show_next_page();
        void show_previous_page();

        size_t get_page_count() {return pages;};
        size_t get_row_count() {return rows;};
        size_t get_col_count() {return cols;};


    private:

        void init_lcd_4bit();
        void init_lcd_8bit();
        void clear_lcd();
        void send_char(char text);
        void send_text(char* text);
        void send_text(const char* text);
        void set_address(uint8_t address);
        uint8_t get_cursor_address();
        uint8_t get_cursor_address(uint8_t col, uint8_t row);
        void print_page();

        void print(char* text);

        Writer* writer;

        vector<vector<vector<char>>> display_buffer;
        
        struct Cursor {
            uint8_t col = 0;
            uint8_t row = 0;
        }cursor;

        size_t target_page = 0;
        size_t visible_page = 0;

        uint8_t cols;
        uint8_t rows;
        size_t pages;

        uint8_t display_reg = 0b00001100;
};

#endif