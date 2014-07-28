#ifndef __CONTROLSURFACE_H__
#define __CONTROLSURFACE_H__

typedef void (*KeyHandlerFunc)(unsigned long which, boolean isPress);

class CControlSurface
{
  unsigned long debounceTime;
  
public:  
  unsigned long keyStatus;


  KeyHandlerFunc pfnOnKeyEvent;
  
  enum {
    DEBOUNCE_PERIOD = 200 //ms
  };
  
  // define the pins used to scan the keyboard
  enum {     
    P_SCAN0 = 3,
    P_SCAN1 = 8,
    P_SCAN2 = 10,
    P_SCAN3 = 11,
    P_SCAN4 = 12,
    P_SCAN5 = 7,
    P_SCAN6 = 14,
    P_SCAN7 = 15,
    P_RET0  = 2,
    P_RET1  = 5,
    P_RET2  = 6,
    P_RET3  = 4,
    
    P_LED0  = 16,
    P_LED1  = 17,
    P_LED2  = 18
  };

  enum {  
    K_F1     = 0x10000000UL,
    K_F2     = 0x04000000UL,
    K_F3     = 0x08000000UL,
    K_F4     = 0x02000000UL,

    K_INV1    = 0x00008000UL,
    K_CHORD0  = 0x00004000UL,
    K_CHORD1  = 0x00800000UL,
    K_CHORD2  = 0x00400000UL,
    K_CHORD3  = 0x00200000UL,
    K_CHORD4  = 0x00100000UL,
    K_CHORD5  = 0x00080000UL,
    K_CHORD6  = 0x00040000UL,
    K_CHORD7  = 0x00020000UL,
    K_CHORD8  = 0x00010000UL,
    K_INV2    = 0x01000000UL,

    K_OCT_DOWN= 0x00000080UL,
    K_NOTE0   = 0x00000040UL,
    K_NOTE1   = 0x00002000UL,
    K_NOTE2   = 0x00000020UL,
    K_NOTE3   = 0x00001000UL,
    K_NOTE4   = 0x00000010UL,
    K_NOTE5   = 0x00000008UL,
    K_NOTE6   = 0x00000800UL,
    K_NOTE7   = 0x00000004UL,
    K_NOTE8   = 0x00000400UL,
    K_NOTE9   = 0x00000002UL,
    K_NOTE10  = 0x00000200UL,
    K_NOTE11  = 0x00000001UL,
    K_OCT_UP  = 0x00000100UL,        
  };

  
  /////////////////////////////////////////////////////////////////////
  void setup(KeyHandlerFunc onKeyEvent)
  {
      pinMode(P_SCAN0, OUTPUT);
      pinMode(P_SCAN1, OUTPUT);
      pinMode(P_SCAN2, OUTPUT);
      pinMode(P_SCAN3, OUTPUT);
      pinMode(P_SCAN4, OUTPUT);
      pinMode(P_SCAN5, OUTPUT);
      pinMode(P_SCAN6, OUTPUT);
      pinMode(P_SCAN7, OUTPUT);
  
      pinMode(P_RET0, INPUT);
      pinMode(P_RET1, INPUT);
      pinMode(P_RET2, INPUT);
      pinMode(P_RET3, INPUT);

      pinMode(P_LED0, OUTPUT);
      pinMode(P_LED1, OUTPUT);
      pinMode(P_LED2, OUTPUT);

      digitalWrite(P_LED0, LOW);
      digitalWrite(P_LED1, LOW);
      digitalWrite(P_LED2, LOW);
    
      keyStatus = 0;
      debounceTime = 0;      
      pfnOnKeyEvent = onKeyEvent;
  }
  
  /////////////////////////////////////////////////////////////////////  
  void run(unsigned long milliseconds)
  {
    // check if still in debounce period
    if(milliseconds && debounceTime > milliseconds)
      return;      
          
    // perform the raw scan to see which keys are pressed
    unsigned long result = 0;
    unsigned long mask0 = (1UL<<0);
    unsigned long mask1 = (1UL<<8);
    unsigned long mask2 = (1UL<<16);
    unsigned long mask3 = (1UL<<24);
    for(int i=0; i<8; ++i)
    {
        digitalWrite(P_SCAN0, (i==0)); 
        digitalWrite(P_SCAN1, (i==1)); 
        digitalWrite(P_SCAN2, (i==2)); 
        digitalWrite(P_SCAN3, (i==3)); 
        digitalWrite(P_SCAN4, (i==4)); 
        digitalWrite(P_SCAN5, (i==5)); 
        digitalWrite(P_SCAN6, (i==6)); 
        digitalWrite(P_SCAN7, (i==7)); 
        
        delayMicroseconds(10);
        byte keys = 0;
        if(digitalRead(P_RET0)) result |= mask0;
        if(digitalRead(P_RET1)) result |= mask1;
        if(digitalRead(P_RET2)) result |= mask2;
        if(digitalRead(P_RET3)) result |= mask3;
        
        mask0<<=1;
        mask1<<=1;
        mask2<<=1;
        mask3<<=1;
    }  
    digitalWrite(P_SCAN0, LOW); 
    digitalWrite(P_SCAN1, LOW); 
    digitalWrite(P_SCAN2, LOW); 
    digitalWrite(P_SCAN3, LOW); 
    digitalWrite(P_SCAN4, LOW); 
    digitalWrite(P_SCAN5, LOW); 
    digitalWrite(P_SCAN6, LOW); 
    digitalWrite(P_SCAN7, LOW); 
  
    // nothing changed
    if(result == keyStatus)
      return;

    
    // find out which key states have changed
    unsigned long delta = result ^ keyStatus;
    if(result & delta)
      debounceTime = milliseconds + DEBOUNCE_PERIOD;           
    keyStatus = result;
    unsigned long b = 0x80000000UL;
    while(b)
    {
      if(delta & b)
      {
        if(pfnOnKeyEvent) 
            pfnOnKeyEvent(b, !!(result&b));
      }
      b>>=1;
    }    
  }
  
/*  
  char *getKeyName()
  {
    switch(keyStatus)
    {
      case 0: return "";
      case K_UP: return "up";
      case K_DOWN: return "down";
      case K_MODE: return "mode";
      case K_SPARE: return "sp";
      case K_INV1: return "inv1";
      case K_MAJ7: return "maj7";
      case K_MIN7: return "min7";
      case K_6TH: return "6";
      case K_MIN6: return "min6";
      case K_9TH: return "9";
      case K_DIM: return "dim";
      case K_SUS4: return "sus4";
      case K_MIN: return "min";
      case K_7TH: return "7";
      case K_INV2: return "inv2";
      case K_OCT_DOWN: return "oct dn";
      case K_C: return "c";
      case K_CSHARP: return "c#";
      case K_D: return "d";
      case K_DSHARP: return "d#";
      case K_E: return "e";
      case K_F: return "f";
      case K_FSHARP: return "f#";
      case K_G: return "g";
      case K_GSHARP: return "g#";
      case K_A: return "a";
      case K_ASHARP: return "a#";
      case K_B: return "b";
      case K_OCT_UP: return "oct+";
      default: return "?";
    }
  }
*/  
  void setLED(int which, int status)
  {
    switch(which)
    {
      case 0: digitalWrite(P_LED0, !!status); break;
      case 1: digitalWrite(P_LED1, !!status); break;
      case 2: digitalWrite(P_LED2, !!status); break;
    }
  }
};

#endif // __CONTROLSURFACE_H__
