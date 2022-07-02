#include "display.h"

Display::Display() {}

Display::Display(i2c_inst_t* i2cport, int sda_pin, int scl_pin, uint8_t i2c_address, uint8_t cols, uint8_t rows, int pages) {

    Display::writer = new i2cWriter(i2cport, i2c_address, sda_pin, scl_pin);

    Display::cols = cols;
    Display::rows = rows;
    Display::pages = pages;
    
    //cols+1 accounts for null-terminator, so row lengths don't need to be passed down to writer
    display_buffer = vector<vector<vector<char>>>(pages, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));

    //Put null terminator at end of each line
    for(size_t i = 0; i < pages; i++) {
        for(size_t j = 0; j < rows; j++) {
            display_buffer[i][j][cols] = 0x00;
        }
    }

    init_lcd_4bit();
}

Display::Display(int rs_pin, int en_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, uint8_t cols, uint8_t rows, int pages) {

    Display::writer = new parallel4Writer(rs_pin, en_pin, d4_pin, d5_pin, d6_pin, d7_pin);

    Display::cols = cols;
    Display::rows = rows;
    Display::pages = pages;

    //cols+1 accounts for null-terminator, so row lengths don't need to be passed down to writer
    display_buffer = vector<vector<vector<char>>>(pages, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));

    //Put null terminator at end of each line
    for(size_t i = 0; i < pages; i++) {
        for(size_t j = 0; j < rows; j++) {
            display_buffer[i][j][cols] = 0x00;
        }
    }

    init_lcd_4bit();
}

Display::Display(int rs_pin, int en_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, int backlight_anode_pin, uint8_t cols, uint8_t rows, int pages) {

    Display::writer = new parallel4Writer(rs_pin, en_pin, d4_pin, d5_pin, d6_pin, d7_pin, backlight_anode_pin);

    Display::cols = cols;
    Display::rows = rows;
    Display::pages = pages;

    //cols+1 accounts for null-terminator, so row lengths don't need to be passed down to writer
    display_buffer = vector<vector<vector<char>>>(pages, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));

    //Put null terminator at end of each line
    for(size_t i = 0; i < pages; i++) {
        for(size_t j = 0; j < rows; j++) {
            display_buffer[i][j][cols] = 0x00;
        }
    }

    init_lcd_4bit();
    writer->backlight_on();
}


Display::Display(int rs_pin, int en_pin, int d0_pin, int d1_pin, int d2_pin, int d3_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, uint8_t cols, uint8_t rows, int pages) {

    Display::writer = new parallel8Writer(rs_pin, en_pin, d0_pin, d1_pin, d2_pin, d3_pin, d4_pin, d5_pin, d6_pin, d7_pin);

    Display::cols = cols;
    Display::rows = rows;
    Display::pages = pages;

    //cols+1 accounts for null-terminator, so row lengths don't need to be passed down to writer
    display_buffer = vector<vector<vector<char>>>(pages, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));

    //Put null terminator at end of each line
    for(size_t i = 0; i < pages; i++) {
        for(size_t j = 0; j < rows; j++) {
            display_buffer[i][j][cols] = 0x00;
        }
    }

    init_lcd_8bit();
}

Display::Display(int rs_pin, int en_pin, int d0_pin, int d1_pin, int d2_pin, int d3_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin, int backlight_anode_pin, uint8_t cols, uint8_t rows, int pages) {

    Display::writer = new parallel8Writer(rs_pin, en_pin, d0_pin, d1_pin, d2_pin, d3_pin, d4_pin, d5_pin, d6_pin, d7_pin, backlight_anode_pin);

    Display::cols = cols;
    Display::rows = rows;
    Display::pages = pages;

    //cols+1 accounts for null-terminator, so row lengths don't need to be passed down to writer
    display_buffer = vector<vector<vector<char>>>(pages, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));

    //Put null terminator at end of each line
    for(size_t i = 0; i < pages; i++) {
        for(size_t j = 0; j < rows; j++) {
            display_buffer[i][j][cols] = 0x00;
        }
    }

    init_lcd_8bit();
    writer->backlight_on();
}

void Display::init_lcd_4bit() {

    //15ms after power on
    busy_wait_us_32(15000);

    //Initialization sequence from datasheet
    writer->send_byte(0b00000011, Register::COMMAND);
    sleep_ms(5);
    writer->send_byte(0b00000011, Register::COMMAND);
    sleep_us(150);
    writer->send_byte(0b00000011, Register::COMMAND);
    writer->send_byte(0b00000010, Register::COMMAND);

    //Entry-mode set register; cursor right, no shift
    writer->send_byte(0b00000110, Register::COMMAND);

    //Function set register; 4-bit mode, 2 line mode, and 5x8 character mode
    writer->send_byte(0b00101000, Register::COMMAND);

    //Display control register; display on, cursor off, cursor blink on
    writer->send_byte(0b00001100, Register::COMMAND);

    //Clear display
    writer->send_byte(0b00000001, Register::COMMAND);
}

void Display::init_lcd_8bit() {

    //15ms after power on
    busy_wait_us_32(15000);

    //Initialization sequence from datasheet
    writer->send_byte(0b00110000, Register::COMMAND);
    sleep_ms(5);
    writer->send_byte(0b00110000, Register::COMMAND);
    sleep_us(150);
    writer->send_byte(0b00110000, Register::COMMAND);

    //Entry-mode set register; cursor right, no shift
    writer->send_byte(0b00000110, Register::COMMAND);

    //Function set register; 8-bit mode, 2 line mode, and 5x8 character mode
    writer->send_byte(0b00111000, Register::COMMAND);

    //Display control register; display on, cursor off, cursor blink on
    writer->send_byte(0b00001100, Register::COMMAND);

    //Clear display
    writer->send_byte(0b00000001, Register::COMMAND);

    sleep_ms(2);
}

void Display::show_cursor() {
    if(display_reg & 0b00000010) {
        return;
    }
    display_reg |= 0b00000010;
    writer->send_byte(display_reg, Register::COMMAND);
}

void Display::hide_cursor() {
    if(display_reg & 0b00000010) {
        display_reg &= ~0b00000010;
        writer->send_byte(display_reg, Register::COMMAND);
    }
}

void Display::cursor_blink_on() {
    if(display_reg & 0b00000001) {
        return;
    }
    display_reg |= 0b00000001;
    writer->send_byte(display_reg, Register::COMMAND);
}

void Display::cursor_blink_off() {
    if(display_reg & 0b00000001) {
        display_reg &= ~0b00000001;
        writer->send_byte(display_reg, Register::COMMAND);
    }
}

void Display::display_on() {
    if(display_reg & 0b00000100) {
        return;
    }
    display_reg |= 0b00000100;
    writer->send_byte(display_reg, Register::COMMAND);
}

void Display::display_off() {
    if(display_reg & 0b00000100) {
        display_reg &= ~0b00000100;
        writer->send_byte(display_reg, Register::COMMAND);
    }
}

void Display::define_character(uint8_t slot, char* character) {
    //EEPROM has space for 8 custom characters, but each character can be accessed by two ascii codes
    //Character 0 is accessible by either 0x00 or 0x08, 1 by 0x01 or 0x09 etc.
    //The second set of addresses is used so as not to use 0x00, and interfere with null-terminator behaviour
    writer->send_byte(0b01000000 | slot << 3, Register::COMMAND);
    for(uint8_t i = 0; i < 8; i++) {
        writer->send_byte(character[i], Register::CHARACTER);
    }
    set_address(get_cursor_address());
}

void Display::clear_lcd() {
    writer->send_byte(0b00000001, Register::COMMAND);
}

void Display::set_address(uint8_t address) {
    writer->send_byte(0b10000000 | address, Register::COMMAND);
}

uint8_t Display::get_cursor_address(uint8_t col, uint8_t row) {
    if(cols == 16 && rows == 2) {
        switch(row) {
            case 0:
                return col;
                break;
            case 1:
                return 64 + col;
                break;
        }
    }
    else if(cols == 20 && rows == 4) {
        switch(row) {
            case 0:
                return col;
                break;
            case 1:
                return 64 + col;
                break;
            case 2:
                return 20 + col;
                break;
            case 3:
                return 84 + col;
                break;
        }
    }
    return 0;
}

uint8_t Display::get_cursor_address() {
    return get_cursor_address(cursor.col, cursor.row);
}

void Display::send_char(char text) {
    writer->send_byte(text, Register::CHARACTER);
}

void Display::send_text(const char* text) {
    send_text((char*)text);
}

void Display::send_text(char* text) {
    while(*text != 0x00) {
        writer->send_byte(*text, Register::CHARACTER);
        text++;
    }
}

void Display::print(char* text) {
    
    if(visible_page == target_page) {
        set_address(get_cursor_address());

        while(cursor.col != cols && *text != 0x00) {
            send_char(*text);
            display_buffer[target_page][cursor.row][cursor.col] = *text;
            cursor.col++;
            text++;
        }
    }
    else {
        while(cursor.col != cols && *text != 0x00) {
            display_buffer[target_page][cursor.row][cursor.col] = *text;
            cursor.col++;
            text++;
        }
    }
    if(cursor.col == cols) {
        cursor.col = 0;
        cursor.row = cursor.row == rows - 1 ? 0 : cursor.row + 1;
        set_address(get_cursor_address());
    }
};

void Display::clear_page() {
    for(size_t i = 0; i < rows; i++) {
        std::fill(display_buffer[target_page][i].begin(), display_buffer[target_page][i].end() - 1, ' ');
    }
    clear_lcd();
}

void Display::clear_all() {

    display_buffer = vector<vector<vector<char>>>(pages, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));
    for(size_t i = 0; i < pages; i++) {
        for(size_t j = 0; j < rows; j++) {
            display_buffer[i][j][cols] = 0x00;
        }
    }
    clear_lcd();
}

void Display::add_page() {
    display_buffer.push_back(vector<vector<char>>(rows, vector<char>(cols+1, ' ')));
    for(size_t i = 0; i < rows; i++) {
        display_buffer[pages][i][cols] = 0x00;
    }
    pages++;
}

void Display::add_pages(size_t num) {
    for(size_t i = 0; i < num; i++) {
        add_page();
    }
}

void Display::add_page_at(size_t index) {
    display_buffer.insert(display_buffer.begin() + index, vector<vector<char>>(rows, vector<char>(cols+1, ' ')));
    for(size_t i = 0; i < rows; i++) {
        display_buffer[index][i][cols] = 0x00;
    }
    pages++;
    target_page = target_page >= index ? target_page + 1 : target_page;
    visible_page = visible_page >= index ? visible_page + 1 : visible_page;
}

void Display::add_pages_at(size_t index, size_t num) {
    for(size_t i = 0; i < num; i++) {
        add_page_at(index);
    }
}

void Display::remove_page() {
    if(pages == 1) {
        clear_page();
    }
    else {
        if(visible_page == pages - 1) {
            visible_page--;
            print_page();
        }
        if(target_page == pages - 1) {
            target_page--;
        }
        display_buffer.erase(display_buffer.end());
        pages--;
    }
}

void Display::remove_pages(size_t num) {
    for(size_t i = 0; i < num; i++) {
        remove_page();
    }
}

void Display::remove_page_at(size_t index) {
    if(pages == 1) {
        clear_page();
    }
    else {
        display_buffer.erase(display_buffer.begin()+index);
        pages--;
        if(target_page >= index) {
            target_page--;
        }
        if(visible_page >= index) {
            visible_page--;
            print_page();
        }
    }
}

void Display::remove_pages_at(size_t index, size_t num) {
    for(size_t i = 0; i < num; i++) {
        remove_page_at(index);
    }
}

void Display::set_cursor(uint8_t col, uint8_t row) {
    cursor = (Cursor){.col = col, .row = row};
    set_address(get_cursor_address());
}

void Display::set_cursor(uint8_t col, uint8_t row, size_t page) {
    cursor = (Cursor){.col = col, .row = row};
    set_address(get_cursor_address());
    set_target_page(page);
}

void Display::set_target_page(size_t page) {
    target_page = page >= pages ? pages-1 : page;
}

void Display::set_visible_page(size_t page) {
    visible_page = page >= pages ? pages-1 : page;
    print_page();
}

void Display::show_next_page() {
    set_visible_page(visible_page + 1 >= pages ? 0 : visible_page + 1);
}

void Display::show_previous_page() {
    set_visible_page(visible_page - 1 < 0 ? pages-1 : visible_page - 1);
}

void Display::print_page() {
    for(size_t i = 0; i < rows; i++) {
        set_address(get_cursor_address(0, i));
        send_text(display_buffer[visible_page][i].data());
    }
    set_address(get_cursor_address());
}
