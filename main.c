#include <reg51.h>

// ---------------- LCD control pins ----------------
sbit RS = P3^0;
sbit EN = P3^1;

// ---------------- Sensors and devices --------------
sbit IR_ENTRY = P1^0;
sbit IR_EXIT  = P1^1;
sbit ACTIVATE = P1^2;
sbit RESET_BTN= P1^3;
sbit BUZZER   = P2^0;

// ---------------- Function Prototypes --------------
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_string(char *);
void lcd_init(void);
void delay(unsigned int);

// ---------------- Global Variables -----------------
unsigned int entry_count = 0;
unsigned int exit_count  = 0;
bit system_on = 0;

// ---------------- Delay ----------------------------
void delay(unsigned int ms)
{
    unsigned int i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<1275;j++);
}

// ---------------- LCD Command ----------------------
void lcd_cmd(unsigned char cmd)
{
    P2 = (P2 & 0x0F) | (cmd & 0xF0);
    RS = 0;
    EN = 1;
    delay(2);
    EN = 0;

    P2 = (P2 & 0x0F) | (cmd << 4);
    EN = 1;
    delay(2);
    EN = 0;
}

// ---------------- LCD Data -------------------------
void lcd_data(unsigned char dat)
{
    P2 = (P2 & 0x0F) | (dat & 0xF0);
    RS = 1;
    EN = 1;
    delay(2);
    EN = 0;

    P2 = (P2 & 0x0F) | (dat << 4);
    EN = 1;
    delay(2);
    EN = 0;
}

// ---------------- LCD String -----------------------
void lcd_string(char *str)
{
    while(*str)
        lcd_data(*str++);
}

// ---------------- LCD Initialization ---------------
void lcd_init()
{
    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

// ---------------- Display Counts -------------------
void display_counts()
{
    lcd_cmd(0x80);
    lcd_string("IN:");
    lcd_data(entry_count/10 + '0');
    lcd_data(entry_count%10 + '0');

    lcd_cmd(0xC0);
    lcd_string("OUT:");
    lcd_data(exit_count/10 + '0');
    lcd_data(exit_count%10 + '0');
}

// ---------------- Main Program ---------------------
void main()
{
    lcd_init();
    lcd_string("Security System");
    delay(2000);
    lcd_cmd(0x01);

    while(1)
    {
        // -------- Activate / Deactivate system ------
        if(ACTIVATE == 0)
        {
            system_on = !system_on;
            BUZZER = 1;
            delay(200);
            BUZZER = 0;
            while(ACTIVATE == 0); // wait release
        }

        // -------- Reset counter ---------------------
        if(RESET_BTN == 0)
        {
            entry_count = 0;
            exit_count  = 0;
            lcd_cmd(0x01);
            while(RESET_BTN == 0);
        }

        // -------- System active ---------------------
        if(system_on)
        {
            // Entry detection
            if(IR_ENTRY == 0)
            {
                entry_count++;
                BUZZER = 1;
                delay(100);
                BUZZER = 0;
                while(IR_ENTRY == 0);
            }

            // Exit detection
            if(IR_EXIT == 0)
            {
                exit_count++;
                BUZZER = 1;
                delay(100);
                BUZZER = 0;
                while(IR_EXIT == 0);
            }

            display_counts();
        }
        else
        {
            lcd_cmd(0x80);
            lcd_string("System OFF     ");
        }
    }
}