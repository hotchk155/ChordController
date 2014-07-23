
#ifndef __DISPLAY_H__
#define __DISPLAY_H__


#define CH_WU     (byte)0
#define CH_WD     (byte)1
#define CH_WU_BU  (byte)2
#define CH_WU_BD  (byte)3
#define CH_WD_BU  (byte)4
#define CH_WD_BD  (byte)5
#define CH_SH     (byte)6

class CDisplay
{
  LiquidCrystal lcd;

public:  
  CDisplay() : 
    lcd(15, 19, 14, 12, 11, 10) {}
    
  void setup()
  {
    byte g_WU[8]= {
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b00000
    };
    
    byte g_WD[8]= {
        0b00000,
        0b10101,
        0b01010,
        0b10101,
        0b01010,
        0b10101,
        0b01010,
        0b10101
    };
    
    byte g_WU_BU[8]= {
        0b11100,
        0b11100,
        0b11100,
        0b11100,
        0b11111,
        0b11111,
        0b11111,
        0b00000
    };
    
    byte g_WD_BU[8]= {
        0b00000,
        0b10100,
        0b01000,
        0b10100,
        0b01000,
        0b10101,
        0b01010,
        0b10101
    };
    
    byte g_WU_BD[8]= {
        0b11101,
        0b11110,
        0b11101,
        0b11110,
        0b11101,
        0b11110,
        0b11111,
        0b00000
    };
    
    byte g_WD_BD[8]= {
        0b00001,
        0b10110,
        0b01001,
        0b10110,
        0b01001,
        0b10110,
        0b01010,
        0b10101,
    };
    
    byte g_SH[8]= {
        0b00101,
        0b01111,
        0b01010,
        0b11110,
        0b10100,
        0b00000,
        0b00000,
        0b00000,
    };
    
    delay(100);
    lcd.begin(16, 2);
    delay(100);
    lcd.createChar(CH_WU,    g_WU);
    lcd.createChar(CH_WD,    g_WD);
    lcd.createChar(CH_WU_BU, g_WU_BU);
    lcd.createChar(CH_WU_BD, g_WU_BD);
    lcd.createChar(CH_WD_BU, g_WD_BU);
    lcd.createChar(CH_WD_BD, g_WD_BD);
    lcd.createChar(CH_SH,    g_SH);
    delay(100);
  }

  void showRow(int row, const char *s, int len)
  {
     lcd.setCursor(0, row);
     while(len--)
     {
       lcd.print(*s); 
       ++s;
     }
  }  
  void showRow(int row, const char *s)
  {
     lcd.setCursor(0, row);
     for(int i=0; i<16; ++i)
     {
       if(*s)
       {
         lcd.print(*s); 
         ++s;         
       }
       else
       {
         lcd.print(' '); 
       }
     }
  }  
  
};

#endif // __DISPLAY_H__

