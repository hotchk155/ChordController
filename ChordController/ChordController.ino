// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
// RS, E, D4, D5, D6, D7
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(15, 19, 14, 12, 11, 10);

#define P_SCAN0   3
#define P_SCAN1   8
#define P_SCAN2   10
#define P_SCAN3   11
#define P_SCAN4   12
#define P_SCAN5   7
#define P_SCAN6   14
#define P_SCAN7   15

#define P_RET0    2
#define P_RET1    5
#define P_RET2    6
#define P_RET3    4



#define KEY_TRANS_UP      0x10000000UL
#define KEY_TRANS_DOWN    0x04000000UL
#define KEY_TRANS_MODE    0x08000000UL
#define KEY_TRANS_SPARE   0x02000000UL

#define KEY_INVERSION1    0x00008000UL
#define KEY_MAJ7          0x00004000UL
#define KEY_MIN7          0x00800000UL
#define KEY_6TH           0x00400000UL
#define KEY_MIN6          0x00200000UL
#define KEY_9TH           0x00100000UL
#define KEY_DIM           0x00080000UL
#define KEY_SUS4          0x00040000UL
#define KEY_MIN           0x00020000UL
#define KEY_7TH           0x00010000UL
#define KEY_INVERSION2    0x01000000UL

#define KEY_OCT_DOWN      0x00000080UL
#define KEY_C             0x00000040UL
#define KEY_CSHARP        0x00002000UL
#define KEY_D             0x00000020UL
#define KEY_DSHARP        0x00001000UL
#define KEY_E             0x00000010UL
#define KEY_F             0x00000008UL
#define KEY_FSHARP        0x00000800UL
#define KEY_G             0x00000004UL
#define KEY_GSHARP        0x00000400UL
#define KEY_A             0x00000002UL
#define KEY_ASHARP        0x00000200UL
#define KEY_B             0x00000001UL
#define KEY_OCT_UP        0x00000100UL

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

#define CH_WU     (byte)0
#define CH_WD     (byte)1
#define CH_WU_BU  (byte)2
#define CH_WU_BD  (byte)3
#define CH_WD_BU  (byte)4
#define CH_WD_BD  (byte)5
#define CH_SH     (byte)6


#define MIDI_CHAN 0
void midiSetup()
{
  Serial.begin(31250);
}
void midiNote(byte chan, byte note, byte vel)
{
  Serial.write(0x90|chan);
  Serial.write(note&0x7F); 
  Serial.write(vel&0x7f);
}

unsigned long keyStatus = 0;
byte noteHeld[128];


enum 
{
  ROOT_NONE = -1,
  ROOT_C = 0,
  ROOT_CSHARP,
  ROOT_D,
  ROOT_DSHARP,
  ROOT_E,
  ROOT_F,
  ROOT_FSHARP,
  ROOT_G,
  ROOT_GSHARP,
  ROOT_A,
  ROOT_ASHARP,
  ROOT_B
};

enum 
{
  CHORD_NONE = -1,
  CHORD_MAJ = 0,
  CHORD_MAJ7,
  CHORD_MIN7,
  CHORD_6,
  CHORD_MIN6,
  CHORD_9,
  CHORD_DIM,
  CHORD_SUS4,
  CHORD_MIN,
  CHORD_7
};

enum
{
  INVERSION_NONE   = -1,
  INVERSION_FIRST  = 0,
  INVERSION_SECOND = 1
};

byte determineChord(int& rootNote, int& chordType, int& inversionType)
{
  rootNote = ROOT_NONE;
  chordType = CHORD_NONE;
  inversionType = INVERSION_NONE;
  
  if(keyStatus & KEY_C)               rootNote = ROOT_C;
  else if(keyStatus & KEY_CSHARP)     rootNote = ROOT_CSHARP;
  else if(keyStatus & KEY_D)          rootNote = ROOT_D;
  else if(keyStatus & KEY_DSHARP)     rootNote = ROOT_DSHARP;
  else if(keyStatus & KEY_E)          rootNote = ROOT_E;
  else if(keyStatus & KEY_F)          rootNote = ROOT_F;
  else if(keyStatus & KEY_FSHARP)     rootNote = ROOT_FSHARP;
  else if(keyStatus & KEY_G)          rootNote = ROOT_G;
  else if(keyStatus & KEY_GSHARP)     rootNote = ROOT_GSHARP;
  else if(keyStatus & KEY_A)          rootNote = ROOT_A;
  else if(keyStatus & KEY_ASHARP)     rootNote = ROOT_ASHARP;
  else if(keyStatus & KEY_B)          rootNote = ROOT_B;  
  else return 0;
  
  chordType = CHORD_MAJ;
  if(keyStatus & KEY_MAJ7)       chordType = CHORD_MAJ7;
  else if(keyStatus & KEY_MIN7)  chordType = CHORD_MIN7;
  else if(keyStatus & KEY_6TH)   chordType = CHORD_6;
  else if(keyStatus & KEY_MIN6)  chordType = CHORD_MIN6;
  else if(keyStatus & KEY_9TH)   chordType = CHORD_9;
  else if(keyStatus & KEY_DIM)   chordType = CHORD_DIM;
  else if(keyStatus & KEY_SUS4)  chordType = CHORD_SUS4;
  else if(keyStatus & KEY_MIN)   chordType = CHORD_MIN;
  else if(keyStatus & KEY_7TH)   chordType = CHORD_7;  
  
  if (keyStatus & KEY_INVERSION1)       inversionType = INVERSION_FIRST;
  else if (keyStatus & KEY_INVERSION2)  inversionType = INVERSION_SECOND;
  return 1;
}

int buildChord(int rootNote, int chordType, int inversionType, byte *chord)
{
   int rootOfs = 0;
   int thirdOfs = 0;
   if(inversionType == INVERSION_FIRST)
   {
     rootOfs = 12;
   }
   else if(inversionType == INVERSION_SECOND)
   {
     rootOfs = 12;
     thirdOfs = 12;
   }
   
   
   int len = 0;
   switch(chordType)
   {
     case CHORD_MAJ:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 4 + rootNote + thirdOfs;   // major third
       chord[len++] = 7 + rootNote;   // fifth
       break;
     case CHORD_MIN:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 3 + rootNote + thirdOfs;   // minor third
       chord[len++] = 7 + rootNote;   // fifth
       break;
     case CHORD_7:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 4 + rootNote + thirdOfs;   // major third
       chord[len++] = 7 + rootNote;   // fifth
       chord[len++] = 10 + rootNote;  // dominant 7
       break;
     case CHORD_MAJ7:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 4 + rootNote + thirdOfs;   // major third
       chord[len++] = 7 + rootNote;   // fifth
       chord[len++] = 11 + rootNote;  // major 7
       break;
     case CHORD_MIN7:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 3 + rootNote + thirdOfs;   // minor third
       chord[len++] = 7 + rootNote;   // fifth
       chord[len++] = 10 + rootNote;  // dominant 7
       break;
     case CHORD_6:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 4 + rootNote + thirdOfs;   // major third
       chord[len++] = 9 + rootNote;   // sixth
       break;
     case CHORD_MIN6:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 3 + rootNote + thirdOfs;   // minor third
       chord[len++] = 9 + rootNote;   // sixth
       break;
     case CHORD_9:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 4 + rootNote + thirdOfs;   // major third
       chord[len++] = 7 + rootNote;   // fifth
       chord[len++] = 14 + rootNote;  // ninth
       break;
     case CHORD_SUS4:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 5 + rootNote + thirdOfs;   // suspended fourth
       chord[len++] = 7 + rootNote;   // fifth
       break;
     case CHORD_DIM:
       chord[len++] = 0 + rootNote + rootOfs;   // root
       chord[len++] = 3 + rootNote + thirdOfs;   // minor third
       chord[len++] = 6 + rootNote;   // sharpened fifth
       chord[len++] = 10 + rootNote;  // dominant 7
       break;
   }
  return len;   
}

void playChord(byte *chord, int length)
{
  int i;
  
  // unflag existing notes which are also in the
  // new chord
  for(i=0; i<length; ++i)
  {
    byte ch = (chord[i]&0x7F);
    noteHeld[ch] = 0;
  }
   
  // unflag and mute the remaining notes
  for(i=0; i<128; ++i)
  {
    if(noteHeld[i])
    {
      midiNote(MIDI_CHAN, i, 0);
      noteHeld[i] = 0;
    }
  }
  
  // play the new notes
  for(i=0; i<length; ++i)
  {
    byte ch = (chord[i]&0x7F);
    midiNote(MIDI_CHAN, ch, 127);
    noteHeld[ch] = 1;
  }
  
}

void showKeys()
{
  lcd.setCursor(0, 0);
  if(keyStatus & KEY_TRANS_UP) lcd.print("trans+ ");
  if(keyStatus & KEY_TRANS_DOWN) lcd.print("trans- ");
  if(keyStatus & KEY_TRANS_MODE) lcd.print("mode ");
  if(keyStatus & KEY_TRANS_SPARE) lcd.print("spare ");
  if(keyStatus & KEY_INVERSION1) lcd.print("inv1 ");
  if(keyStatus & KEY_MAJ7) lcd.print("maj7 ");
  if(keyStatus & KEY_MIN7) lcd.print("min7 ");
  if(keyStatus & KEY_6TH) lcd.print("6 ");
  if(keyStatus & KEY_MIN6) lcd.print("min6 ");
  if(keyStatus & KEY_9TH) lcd.print("9 ");
  if(keyStatus & KEY_DIM) lcd.print("dim ");
  if(keyStatus & KEY_SUS4) lcd.print("sus4 ");
  if(keyStatus & KEY_MIN) lcd.print("min ");
  if(keyStatus & KEY_7TH) lcd.print("7 ");
  if(keyStatus & KEY_INVERSION2) lcd.print("inv2 ");
  if(keyStatus & KEY_OCT_DOWN) lcd.print("oct- ");
  if(keyStatus & KEY_C) lcd.print("c ");
  if(keyStatus & KEY_CSHARP) lcd.print("c# ");
  if(keyStatus & KEY_D) lcd.print("d ");
  if(keyStatus & KEY_DSHARP) lcd.print("d# ");
  if(keyStatus & KEY_E) lcd.print("e ");
  if(keyStatus & KEY_F) lcd.print("f ");
  if(keyStatus & KEY_FSHARP) lcd.print("f# ");
  if(keyStatus & KEY_G) lcd.print("g ");
  if(keyStatus & KEY_GSHARP) lcd.print("g# ");
  if(keyStatus & KEY_A) lcd.print("a ");
  if(keyStatus & KEY_ASHARP) lcd.print("a# ");
  if(keyStatus & KEY_B) lcd.print("b ");
  if(keyStatus & KEY_OCT_UP) lcd.print("oct+");

}

void keysSetup()
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
  
}


int keysScan()
{
  keyStatus = 0;
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
      
      delayMicroseconds(100);
      byte keys = 0;
      if(digitalRead(P_RET0)) keyStatus |= mask0;
      if(digitalRead(P_RET1)) keyStatus |= mask1;
      if(digitalRead(P_RET2)) keyStatus |= mask2;
      if(digitalRead(P_RET3)) keyStatus |= mask3;
      
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
}


void showChord()
{
  char disp[30];
  memset(disp, ' ', sizeof disp);
  char *notes = "CCDDEFFGGAAB";
  int textCount = 0;
  
  lcd.setCursor(0, 1);
  int note = 0;
  while(note < 128 && !noteHeld[note]) 
    note++;
  note = 12 * (note/12);
  int charCount = 0;
  while(charCount < 16)
  {
    char chDisp = -1;
    if(textCount < charCount)
      textCount = charCount;
    int n = note%12;
    switch(n)
    {
      case 0:  // C
      case 2:  // D
      case 5:  // F
      case 7:  // G
      case 9:  // A      
        if(note < 128 && noteHeld[note])
        {
          if(note < 127 && noteHeld[note+1])          
          {
            disp[textCount++] = notes[n];
            disp[textCount++] = notes[n];
            disp[textCount++] = '#';
            chDisp = CH_WD_BD;
          }
          else
          {
            disp[textCount++] = notes[n];
            chDisp = CH_WD_BU;
          }
        }
        else
        {
          if(note < 127 && noteHeld[note+1])          
          {
            disp[textCount++] = notes[n];
            disp[textCount++] = CH_SH;
            chDisp = CH_WU_BD;
          }
          else
          {
            chDisp = CH_WU_BU;
          }
        }
        note+=2;
        break;
      case 4:  // E      
      case 11: // B
        if(note < 128 && noteHeld[note])
        {
          disp[textCount++] = notes[n];
          chDisp = CH_WD;
        }
        else
        {
          chDisp = CH_WU;
        }
        ++note;
        break;
      
      case 1:  // C#
      case 3:  // D#
      case 6:  // F#
      case 8:  // G#
      case 10: // A#
        ++note;
        break;
    }
    if(chDisp >= 0)
    {
      lcd.write(chDisp);
      ++charCount;
    }
  }
  disp[16] = '\0';
  lcd.setCursor(0, 0);
  lcd.print(disp);
}

void setup() 
{
  // set up the LCD's number of columns and rows: 
  lcd.createChar(CH_WU, g_WU);
  lcd.createChar(CH_WD, g_WD);
  lcd.createChar(CH_WU_BU, g_WU_BU);
  lcd.createChar(CH_WU_BD, g_WU_BD);
  lcd.createChar(CH_WD_BU, g_WD_BU);
  lcd.createChar(CH_WD_BD, g_WD_BD);
  lcd.createChar(CH_SH,    g_SH);
  lcd.begin(16, 2);
  keysSetup();
  midiSetup();
  memset(noteHeld, 0, sizeof noteHeld);
  showChord();
}


int rootNote = ROOT_NONE;
int chordType = CHORD_NONE;  
int inversionType = INVERSION_NONE;  
void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  keysScan();
//  lcd.setCursor(0, 1);
  // print the number of seconds since reset:

  int rt;
  int ch;  
  int it;
  determineChord(rt, ch, it);
  if(rt != rootNote || ch != chordType || it != inversionType)
  {
    rootNote = rt;
    chordType = ch;  
    inversionType = it;
    byte chord[20];
    int len = buildChord(60 + rootNote, chordType, inversionType, chord);
    playChord(chord, len);    
    showChord();
  }  
}

