#ifndef __CONTROLSURFACE_H__
#define __CONTROLSURFACE_H__

class CControlSurface
{
  unsigned long scanResult;
  unsigned long debounceTime;
  
public:  
  unsigned long rootKey;
  unsigned long chordKey;
  unsigned long inversionKey;
  unsigned long commandKey;

  enum {
    DEBOUNCE_PERIOD = 20 //ms
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
    K_UP      = 0x10000000UL,
    K_DOWN    = 0x04000000UL,
    K_MODE    = 0x08000000UL,
    K_SPARE   = 0x02000000UL,

    K_INV1    = 0x00008000UL,
    K_MAJ7    = 0x00004000UL,
    K_MIN7    = 0x00800000UL,
    K_6TH     = 0x00400000UL,
    K_MIN6    = 0x00200000UL,
    K_9TH     = 0x00100000UL,
    K_DIM     = 0x00080000UL,
    K_SUS4    = 0x00040000UL,
    K_MIN     = 0x00020000UL,
    K_7TH     = 0x00010000UL,
    K_INV2    = 0x01000000UL,

    K_OCT_DOWN= 0x00000080UL,
    K_C       = 0x00000040UL,
    K_CSHARP  = 0x00002000UL,
    K_D       = 0x00000020UL,
    K_DSHARP  = 0x00001000UL,
    K_E       = 0x00000010UL,
    K_F       = 0x00000008UL,
    K_FSHARP  = 0x00000800UL,
    K_G       = 0x00000004UL,
    K_GSHARP  = 0x00000400UL,
    K_A       = 0x00000002UL,
    K_ASHARP  = 0x00000200UL,
    K_B       = 0x00000001UL,
    K_OCT_UP  = 0x00000100UL,
    
    K_ROOT_MASK     = (K_C|K_CSHARP|K_D|K_DSHARP|K_E|K_F|K_FSHARP|K_G|K_GSHARP|K_A|K_ASHARP|K_B),
    K_CHORD_MASK    = (K_MAJ7|K_MIN7|K_6TH|K_MIN6|K_9TH|K_DIM|K_SUS4|K_MIN|K_7TH),
    K_INV_MASK      = (K_INV1|K_INV2),
    K_COMMAND_MASK  = (K_UP|K_DOWN|K_MODE|K_SPARE|K_OCT_DOWN|K_OCT_UP)
    
  };

  
  /////////////////////////////////////////////////////////////////////
  void setup()
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
    
      scanResult = 0;
      rootKey = 0;
      chordKey = 0;
      inversionKey = 0;
      commandKey = 0;
      debounceTime = 0;
  }
  
  /////////////////////////////////////////////////////////////////////  
  int scan(unsigned long milliseconds)
  {
    // check if still in debounce period
    if(milliseconds && debounceTime > milliseconds)
      return 0;      
          
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
    if(result == scanResult)
      return 0;
      
    // find out which keys have been 
    unsigned long delta = result ^ scanResult;
    unsigned long newPressed = result & delta;
    if(newPressed)
      debounceTime = milliseconds + DEBOUNCE_PERIOD;           
    
    // Key logic
    // For each category; root note buttons, chord buttons, inversions...
    // a) If a button is pressed, it becomes the "current" selection
    // b) When the current selection is released, any remaining pressed button 
    //    becomes the current selection
    //
    if(delta & K_ROOT_MASK)
    {
      if(newPressed & K_ROOT_MASK)
        rootKey = (newPressed & K_ROOT_MASK);
      else if(!(rootKey & result))
        rootKey = (result & K_ROOT_MASK);;
    }
    if(delta & K_CHORD_MASK)
    {
      if(newPressed & K_CHORD_MASK)
         chordKey = (newPressed & K_CHORD_MASK);
      else if(!(chordKey & result))
         chordKey = (result & K_CHORD_MASK);
    }
    if(delta & K_INV_MASK)
    {
      if(newPressed & K_INV_MASK)
        inversionKey = (newPressed & K_INV_MASK);
      else if(!(inversionKey & result))
        inversionKey = (result & K_INV_MASK);
    }
    if(delta & K_COMMAND_MASK)
    {
      if(newPressed & K_COMMAND_MASK)
        commandKey = (newPressed & K_COMMAND_MASK);
      else 
        commandKey = 0;
    }
      
    scanResult = result;
    return 1;        
  }
};

#endif // __CONTROLSURFACE_H__
