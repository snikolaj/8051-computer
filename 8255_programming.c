#include <8052.h>
#include <stdint.h>
#include <compiler.h> 

void DELAY_milliseconds(uint16_t milliseconds) 
{
    while (milliseconds--)
    {
        int x = 81; // The while statement consumes 11.89uS (11 clock cycles)
        while (x--)
            ; // So 10500 * 11.89uS = nearly 125mS
    }
}

__xdata __at (0x0000) uint8_t A_8255;
__xdata __at (0x0001) uint8_t B_8255;
__xdata __at (0x0002) uint8_t C_8255;
__xdata __at (0x0003) uint8_t CONTROL_8255;

__xdata __at (0x0008) uint8_t INSN_16x2;
__xdata __at (0x0009) uint8_t DATA_16x2;
/*
SFRX(A_8255, 0x8000);
SFRX(B_8255, 0x8001);
SFRX(C_8255, 0x8002);
SFRX(CONTROL_8255, 0x8003);
*/

void write_data(uint8_t data){
    DATA_16x2 = data;
    for(uint8_t i = 0; i < 10; i++);
}

void write_command(uint8_t command){
    INSN_16x2 = command;
    for(uint8_t i = 0; i < 10; i++);
}

void change_cursor(uint8_t row, uint8_t line){
    write_command(0b10000000 + row + line * 40);
}

void write_string(char* str){
    for(uint8_t i = 0; str[i] != '\0'; i++){
        write_data(str[i]);
    }
}

void clear_display(){
    write_command(0b00000001);
    DELAY_milliseconds(3);
}

// https://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
void init_display(){
    write_command(0b00110000);
    DELAY_milliseconds(4);
    write_command(0b00110000);
    write_command(0b00110000);

    // function set DL = 1 (8 bit interface), N = 1 (2 line display), F = 0 (5x8 dots per character)
    write_command(0b00111000);
    // display off (with cursor and blinking off)
    write_command(0b00001000);
    // clear display
    clear_display();
    // increment towards the right after each character
    write_command(0b00000110);
    // display on with cursor and blanking
    write_command(0b00001110);
    // set cursor to 1st line
    write_command(0b10000000);
}


void main(void){
    DELAY_milliseconds(1000);
    CONTROL_8255 = 0b10001001;
    A_8255 = 0b10101010;

    __xdata uint8_t RAMnum;
    RAMnum = 51;

    init_display();
    write_string("Shoutout to");
    change_cursor(0, 1);
    write_string("Andrej <3");
    while(1){
        B_8255 = 0;
        P1_4 = 0;
        DELAY_milliseconds(1000);
        B_8255 = RAMnum;
        P1_4 = 1;
        RAMnum++;
        DELAY_milliseconds(1000);
    }
}