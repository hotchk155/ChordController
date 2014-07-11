
// include the library code:
#include "Arduino.h"
#include <LiquidCrystal.h>

#include "ChordController.h"
#include "ControlSurface.h"
#include "Display.h"


CControlSurface ControlSurface;
CDisplay        Display;

//////////////////////////////////////////////////////////////////
//
// LOW LEVEL KEYBOARD
//
//////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////
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


byte noteHeld[130];




byte determineChord(byte octave, CHORD_TYPE& chord)
{
  
  chord = 0;
    
  if(ControlSurface.rootKey & CControlSurface::K_C)               chord |= ROOT_C;
  else if(ControlSurface.rootKey & CControlSurface::K_CSHARP)     chord |= ROOT_CSHARP;
  else if(ControlSurface.rootKey & CControlSurface::K_D)          chord |= ROOT_D;
  else if(ControlSurface.rootKey & CControlSurface::K_DSHARP)     chord |= ROOT_DSHARP;
  else if(ControlSurface.rootKey & CControlSurface::K_E)          chord |= ROOT_E;
  else if(ControlSurface.rootKey & CControlSurface::K_F)          chord |= ROOT_F;
  else if(ControlSurface.rootKey & CControlSurface::K_FSHARP)     chord |= ROOT_FSHARP;
  else if(ControlSurface.rootKey & CControlSurface::K_G)          chord |= ROOT_G;
  else if(ControlSurface.rootKey & CControlSurface::K_GSHARP)     chord |= ROOT_GSHARP;
  else if(ControlSurface.rootKey & CControlSurface::K_A)          chord |= ROOT_A;
  else if(ControlSurface.rootKey & CControlSurface::K_ASHARP)     chord |= ROOT_ASHARP;
  else if(ControlSurface.rootKey & CControlSurface::K_B)          chord |= ROOT_B;  
  else return 0;
  
  if(ControlSurface.chordKey & CControlSurface::K_MAJ7)           chord |= CHORD_MAJ7;
  else if(ControlSurface.chordKey & CControlSurface::K_MIN7)      chord |= CHORD_MIN7;
  else if(ControlSurface.chordKey & CControlSurface::K_6TH)       chord |= CHORD_6;
  else if(ControlSurface.chordKey & CControlSurface::K_MIN6)      chord |= CHORD_MIN6;
  else if(ControlSurface.chordKey & CControlSurface::K_9TH)       chord |= CHORD_9;
  else if(ControlSurface.chordKey & CControlSurface::K_DIM)       chord |= CHORD_DIM;
  else if(ControlSurface.chordKey & CControlSurface::K_SUS4)      chord |= CHORD_SUS4;
  else if(ControlSurface.chordKey & CControlSurface::K_MIN)       chord |= CHORD_MIN;
  else if(ControlSurface.chordKey & CControlSurface::K_7TH)       chord |= CHORD_7;  
  else chord |= CHORD_MAJ;  
  
  if(ControlSurface.inversionKey & CControlSurface::K_INV1)       chord |= INV_FIRST;
  else if (ControlSurface.inversionKey & CControlSurface::K_INV2) chord |= INV_SECOND;
  
  chord |= ((unsigned long)octave)<<12;
  return 1;
}

int buildChord(CHORD_TYPE chord, byte *notes)
{
   int rootNote = (chord & ROOT_MASK)-1;
   if(rootNote<0) 
     return 0;
   
   byte octave = chord >> 12;
   if(octave <= 8)
     rootNote += (12 * octave);
   
   int rootOfs = 0;
   int thirdOfs = 0;
   switch(chord & INV_MASK)
   {
     case INV_SECOND:
       thirdOfs = 12;
       // fall thru
     case INV_FIRST:
       rootOfs = 12;
       break;
   }
      
   int len = 0;
   switch(chord & CHORD_MASK)
   {
     case CHORD_MAJ:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 4 + rootNote + thirdOfs;   // major third
       notes[len++] = 7 + rootNote;   // fifth
       break;
     case CHORD_MIN:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 3 + rootNote + thirdOfs;   // minor third
       notes[len++] = 7 + rootNote;   // fifth
       break;
     case CHORD_7:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 4 + rootNote + thirdOfs;   // major third
       notes[len++] = 7 + rootNote;   // fifth
       notes[len++] = 10 + rootNote;  // dominant 7
       break;
     case CHORD_MAJ7:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 4 + rootNote + thirdOfs;   // major third
       notes[len++] = 7 + rootNote;   // fifth
       notes[len++] = 11 + rootNote;  // major 7
       break;
     case CHORD_MIN7:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 3 + rootNote + thirdOfs;   // minor third
       notes[len++] = 7 + rootNote;   // fifth
       notes[len++] = 10 + rootNote;  // dominant 7
       break;
     case CHORD_6:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 4 + rootNote + thirdOfs;   // major third
       notes[len++] = 9 + rootNote;   // sixth
       break;
     case CHORD_MIN6:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 3 + rootNote + thirdOfs;   // minor third
       notes[len++] = 9 + rootNote;   // sixth
       break;
     case CHORD_9:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 4 + rootNote + thirdOfs;   // major third
       notes[len++] = 7 + rootNote;   // fifth
       notes[len++] = 14 + rootNote;  // ninth
       break;
     case CHORD_SUS4:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 5 + rootNote + thirdOfs;   // suspended fourth
       notes[len++] = 7 + rootNote;   // fifth
       break;
     case CHORD_DIM:
       notes[len++] = 0 + rootNote + rootOfs;   // root
       notes[len++] = 3 + rootNote + thirdOfs;   // minor third
       notes[len++] = 6 + rootNote;   // sharpened fifth
       notes[len++] = 10 + rootNote;  // dominant 7
       break;
   }
  return len;   
}

//////////////////////////////////////////////////////////////////
void playChord(byte *notes, int length)
{
  int i;

  // remember which notes were already playing  
  for(i=0; i<128; ++i)
  {
    if(noteHeld[i])
      noteHeld[i] = 2;
  }
  
  // play any new notes
  for(i=0; i<length; ++i)
  {
    byte ch = (notes[i]&0x7F);
    if(!noteHeld[ch])
      midiNote(MIDI_CHAN, ch, 127);
    noteHeld[ch] = 1;
  }
  
  // mute any old notes which are note in the new chord
  for(i=0; i<128; ++i)
  {
    if(noteHeld[i] == 2)
    {
      midiNote(MIDI_CHAN, i, 0);
      noteHeld[i] = 0;
    }
  }
}  

//////////////////////////////////////////////////////////////////
void showNotesHeld()
{
  
  // lookup tables of note names
  char *notes       = "CCDDEFFGGAAB";
  
  // prepare buffer to receive text characters
  char text[30];
  memset(text, ' ', sizeof text);
  
  // prepare buffer to receive graphics characters
  char graphics[30];
  memset(graphics, '.', sizeof graphics);
  
  // find the lowest MIDI note that is held
  int note = 0;
  while(note < 128 && !noteHeld[note]) 
    note++;
    
  // no notes held
  if(note == 128)
    note = 0;
    
  // get the closest "C" note below lowest held note. This 
  // will be the leftmost note shown on the "keyboard"
  note = 12 * (note/12);
  
  int textCount = 0;
  int charCount = 0;
  while(charCount < 16 && note < 128)
  {
    if(textCount < charCount)
      textCount = charCount;
    int n = note%12;
    switch(n)
    {
      //////////////////////////////////////////////////
      // Notes which have a black note to the right
      case 0: // C    
      case 2: // D    
      case 5: // F    
      case 7: // G   
      case 9: // A   
        if(noteHeld[note])  
        {
          // name of the white note
          text[textCount++] = notes[n];
          
          // white note is held. is the following black note held?
          if(note < 127 && noteHeld[note+1]) 
          {
            // name of black note
            text[textCount++] = notes[n];              
            text[textCount++] = CH_SH;
            
            // keyboard graphic 
            graphics[charCount++] = CH_WD_BD;
          }
          else
          {
            // keyboard graphic 
            graphics[charCount++] = CH_WD_BU;
          }
        }
        else
        {
          // white note not held. is the following black note held?
          if(note < 127 && noteHeld[note+1])          
          {
            // name of the black note
            text[textCount++] = notes[n];
            text[textCount++] = CH_SH;

            // keyboard graphic 
            graphics[charCount++] = CH_WU_BD;
          }
          else
          {

            // keyboard graphic 
            graphics[charCount++] = CH_WU_BU;
          }
        }
        
        // we have output info for two notes
        note+=2;
        break;
                
      //////////////////////////////////////////////////
      // Notes which have no black note to the right
      case 4:  // E      
      case 11: // B
        if(noteHeld[note])
        {
          // name of white note
          text[textCount++] = notes[n];
          
          // keyboard graphic           
          graphics[charCount++] = CH_WD;
        }
        else
        {
          graphics[charCount++] = CH_WU;
        }
        ++note;
        break;
      
      //////////////////////////////////////////////////
      // Black notes.. already dealt with
      case 1:  // C#
      case 3:  // D#
      case 6:  // F#
      case 8:  // G#
      case 10: // A#
        ++note;
        break;
    }
  }
  
  Display.show(0, 0, text, 16);
  Display.show(0, 1, graphics, 16);
}


void setup() 
{
  // set up the LCD's number of columns and rows: 
  
  ControlSurface.setup();
  Display.setup();
  delay(1);
  midiSetup();
  memset(noteHeld, 0, sizeof noteHeld);
  showNotesHeld();
}

void loop() {
  unsigned long milliseconds = millis();
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  if(ControlSurface.scan(milliseconds))
  {
    CHORD_TYPE chord;    
    determineChord(4, chord);
    byte notes[20];
    int len = buildChord(chord, notes);
    playChord(notes, len);    
    showNotesHeld();
  }  
}

