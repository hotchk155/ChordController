
// include the library code:
#include "Arduino.h"
#include <LiquidCrystal.h>

#include "ChordController.h"
#include "ControlSurface.h"
#include "Display.h"

#define P_LED 13
#define MIN_OCTAVE 0
#define MAX_OCTAVE 8

CControlSurface ControlSurface;
CDisplay        Display;
byte noteHeld[130];
byte octave;

char notesHeldText[30];
char notesHeldGraphic[30];

CHORD_TYPE  noteButtons[12];


byte major[12] = { 
  CHORD_MAJ,   // i
  CHORD_MIN, 
  CHORD_MIN,   // ii
  CHORD_MAJ, 
  CHORD_MIN,   // iii
  CHORD_MAJ,   // iv
  CHORD_DIM, 
  CHORD_MAJ,   // v
  CHORD_MAJ, 
  CHORD_MIN,   // vi
  CHORD_MAJ, 
  CHORD_DIM    // vii
};

byte minor[12] = { 
  CHORD_MIN,   // i
  CHORD_MAJ, 
  CHORD_DIM,   // ii
  CHORD_MAJ,   // iii
  CHORD_MIN, 
  CHORD_MIN,   // iv
  CHORD_DIM, 
  CHORD_MIN,   // v
  CHORD_MAJ,   // vi
  CHORD_MIN, 
  CHORD_MAJ,   // vii
  CHORD_MAJ 
};
  

void setChordButtonsMajorTransposed(int scaleRoot)
{  
  for(int i=0; i<12; ++i)
  {
    noteButtons[i] = scaleRoot | major[i];  
    if(++scaleRoot > ROOT_B)
      scaleRoot = ROOT_C;
  }
}

/////////////////////////////////////////////////////////////////
// Set up the chord buttons for a MINOR mode
// The first parameter is the root note applied to the first button
// The second parameter defines the musical key (minor)
void setChordButtonsMinorTransposed(int scaleRoot)
{
  int order[12] = {0,1,2,4,3,5,6,7,9,8,11,10};
  for(int i=0; i<12; ++i)
  {
    noteButtons[order[i]] = scaleRoot | minor[i];  
    if(++scaleRoot > ROOT_B)
      scaleRoot = ROOT_C;
  }
}

//////////////////////////////////////////////////////////////////
// For a given note in a scale, and a given keyType, return the
// appropriate chord type
CHORD_TYPE getChordTypeForNoteButton(int whichButton, byte keyType)
{
  switch(keyType)
  {
    /////////////////////////////////////////////////////////////
    // Constrained to a major key
  case KEYTYPE_MAJOR:
    switch(whichButton)
    {
      // Standard Chords
    case 0:  // I
    case 5:  // IV
    case 7:  // V
      return CHORD_MAJ;
    case 2:  // ii
    case 4:  // iii
    case 9:  // vi
      return CHORD_MIN;
    case 11:  // vii
      return CHORD_DIM;

      // oddness
    case 10:  // viib
    case 3:  //
    case 8:  //
      return CHORD_MAJ;
    case 6:  // 
      return CHORD_DIM;
    case 1:  //
      return CHORD_MIN;
    }

    /////////////////////////////////////////////////////////////
    // Constrained to a minor key
  case KEYTYPE_MINOR:
    switch(whichButton)
    {
    case 0:  // i
    case 4:  // iii
    case 7:  // v
      return CHORD_MIN;
    case 5:  // IV
    case 9:  // VI
    case 11:  // VII
      return CHORD_MAJ;
    case 2:  // ii
      return CHORD_DIM;

      // guesswork
    case 1:  //
    case 3:  //
      return CHORD_MAJ;
    case 6:  // 
      return CHORD_DIM;
    case 8:  //
    case 10:  //        
      return CHORD_MIN;
    }

    /////////////////////////////////////////////////////////////
    // Free play, default to major chords
  case KEYTYPE_NONE:
  default:
    return CHORD_MAJ;
  }
}
void mapNoteButtons(byte rootNote, byte keyType, byte transpose)
{
  int note = rootNote;
  for(int whichButton=0; whichButton<12; ++whichButton)
  {    
    int b = transpose? whichButton : note - 1;
    noteButtons[b] = (note&0x0F) | getChordTypeForNoteButton(b, keyType);
    if(++note > ROOT_B)
      note = ROOT_C;
  }  
}

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

void getChordName(CHORD_TYPE chord, char *chordName)
{
  if(CHORD_NONE == chord)
    return;
    
  int pos = 0;
  switch(chord & ROOT_MASK)
  {
  case ROOT_C:          
    chordName[pos++] = 'C'; 
    break;
  case ROOT_CSHARP:     
    chordName[pos++] = 'C'; 
    chordName[pos++] = CH_SH; 
    break;
  case ROOT_D:          
    chordName[pos++] = 'D'; 
    break;
  case ROOT_DSHARP:     
    chordName[pos++] = 'D'; 
    chordName[pos++] = CH_SH; 
    break;
  case ROOT_E:          
    chordName[pos++] = 'E'; 
    break;
  case ROOT_F:          
    chordName[pos++] = 'F'; 
    break;
  case ROOT_FSHARP:     
    chordName[pos++] = 'F'; 
    chordName[pos++] = CH_SH; 
    break;
  case ROOT_G:          
    chordName[pos++] = 'G'; 
    break;
  case ROOT_GSHARP:     
    chordName[pos++] = 'G'; 
    chordName[pos++] = CH_SH; 
    break;
  case ROOT_A:          
    chordName[pos++] = 'A'; 
    break;
  case ROOT_ASHARP:     
    chordName[pos++] = 'A'; 
    chordName[pos++] = CH_SH; 
    break;
  case ROOT_B:          
    chordName[pos++] = 'B'; 
    break;
  default:              
    chordName[pos++] = '?'; 
    break;
  }

  //  byte octave = (chord>>12) & 0x0F;
  //  chordName[pos++] = '0' + octave: break;

  switch(chord & CHORD_MASK)
  {
  case CHORD_MAJ:    
    break;
  case CHORD_MAJ7:   
    chordName[pos++] = 'M'; 
    chordName[pos++] = '7'; 
    break;
  case CHORD_MIN7:   
    chordName[pos++] = 'm'; 
    chordName[pos++] = '7'; 
    break;
  case CHORD_6:      
    chordName[pos++] = '6'; 
    break;
  case CHORD_MIN6:   
    chordName[pos++] = 'm'; 
    chordName[pos++] = '6'; 
    break;
  case CHORD_9:      
    chordName[pos++] = '9';
    break;
  case CHORD_DIM:    
    chordName[pos++] =  0b11011111;
    break;
  case CHORD_SUS4:   
    chordName[pos++] = 's'; 
    chordName[pos++] = '4';
    break;
  case CHORD_MIN:    
    chordName[pos++] = 'm'; 
    break;
  case CHORD_7:      
    chordName[pos++] = '7'; 
    break;
  case CHORD_NONE:
    break;
  default:           
    chordName[pos++] = '?'; 
    break;
  }

};


//////////////////////////////////////////////////////////////////
byte determineChord(byte octave, CHORD_TYPE& chord)
{

  chord = CHORD_NONE;

  if(ControlSurface.rootKey & CControlSurface::K_C)               chord = noteButtons[0];
  else if(ControlSurface.rootKey & CControlSurface::K_CSHARP)     chord = noteButtons[1];
  else if(ControlSurface.rootKey & CControlSurface::K_D)          chord = noteButtons[2];
  else if(ControlSurface.rootKey & CControlSurface::K_DSHARP)     chord = noteButtons[3];
  else if(ControlSurface.rootKey & CControlSurface::K_E)          chord = noteButtons[4];
  else if(ControlSurface.rootKey & CControlSurface::K_F)          chord = noteButtons[5];
  else if(ControlSurface.rootKey & CControlSurface::K_FSHARP)     chord = noteButtons[6];
  else if(ControlSurface.rootKey & CControlSurface::K_G)          chord = noteButtons[7];
  else if(ControlSurface.rootKey & CControlSurface::K_GSHARP)     chord = noteButtons[8];
  else if(ControlSurface.rootKey & CControlSurface::K_A)          chord = noteButtons[9];
  else if(ControlSurface.rootKey & CControlSurface::K_ASHARP)     chord = noteButtons[10];
  else if(ControlSurface.rootKey & CControlSurface::K_B)          chord = noteButtons[11];  
  else return 0;

  byte altChord = CHORD_NONE;
  if(ControlSurface.chordKey & CControlSurface::K_MAJ7)           altChord = CHORD_MAJ7;
  else if(ControlSurface.chordKey & CControlSurface::K_MIN7)      altChord = CHORD_MIN7;
  else if(ControlSurface.chordKey & CControlSurface::K_6TH)       altChord = CHORD_6;
  else if(ControlSurface.chordKey & CControlSurface::K_MIN6)      altChord = CHORD_MIN6;
  else if(ControlSurface.chordKey & CControlSurface::K_9TH)       altChord = CHORD_9;
  else if(ControlSurface.chordKey & CControlSurface::K_DIM)       altChord = CHORD_DIM;
  else if(ControlSurface.chordKey & CControlSurface::K_SUS4)      altChord = CHORD_SUS4;
  else if(ControlSurface.chordKey & CControlSurface::K_7TH)       altChord = CHORD_7;  
  else if(ControlSurface.chordKey & CControlSurface::K_MIN)       altChord = (chord & CHORD_MASK) == CHORD_MAJ ? CHORD_MIN:CHORD_MAJ;  
  


  if(altChord != CHORD_NONE)
  {
    chord &= ~CHORD_MASK;
    chord |= altChord;
  }

  if(ControlSurface.inversionKey & CControlSurface::K_INV1)       chord |= INV_FIRST;
  else if (ControlSurface.inversionKey & CControlSurface::K_INV2) chord |= INV_SECOND;

  chord |= ((unsigned long)octave)<<12;
  return 1;
}

//////////////////////////////////////////////////////////////////
int buildChord(CHORD_TYPE chord, byte *notes)
{
  int rootNote = (chord & ROOT_MASK)-1;
  if(rootNote<0) 
    return 0;

  byte octave = chord >> 12;
  if(octave <= 8)
    rootNote += (12 * (1+octave));

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
void renderNotesHeld()
{

  // lookup tables of note names
  char *notes       = "CCDDEFFGGAAB";

  memset(notesHeldText, ' ', sizeof notesHeldText);
  memset(notesHeldGraphic, ' ', sizeof notesHeldGraphic);  

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
        notesHeldText[textCount++] = notes[n];

        // white note is held. is the following black note held?
        if(note < 127 && noteHeld[note+1]) 
        {
          // name of black note
          notesHeldText[textCount++] = notes[n];              
          notesHeldText[textCount++] = CH_SH;

          // keyboard graphic 
          notesHeldGraphic[charCount++] = CH_WD_BD;
        }
        else
        {
          // keyboard graphic 
          notesHeldGraphic[charCount++] = CH_WD_BU;
        }
      }
      else
      {
        // white note not held. is the following black note held?
        if(note < 127 && noteHeld[note+1])          
        {
          // name of the black note
          notesHeldText[textCount++] = notes[n];
          notesHeldText[textCount++] = CH_SH;

          // keyboard graphic 
          notesHeldGraphic[charCount++] = CH_WU_BD;
        }
        else
        {

          // keyboard graphic 
          notesHeldGraphic[charCount++] = CH_WU_BU;
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
        notesHeldText[textCount++] = notes[n];

        // keyboard graphic           
        notesHeldGraphic[charCount++] = CH_WD;
      }
      else
      {
        notesHeldGraphic[charCount++] = CH_WU;
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

}


byte heartbeat = 0;
unsigned long nextHeartbeat;
void setup() 
{
  pinMode(P_LED,OUTPUT);
  nextHeartbeat = 0;
  // set up the LCD's number of columns and rows: 
  octave = 3;
  ControlSurface.setup();
  Display.setup();
  delay(1);
  midiSetup();
  memset(noteHeld, 0, sizeof noteHeld);
  renderNotesHeld();
  Display.showRow(0, "--== MIDIOT ==--");
  Display.showRow(1, notesHeldGraphic, 16);
  ControlSurface.setLED(0, 1);
  ControlSurface.setLED(1, 1);
  ControlSurface.setLED(2, 1);
  delay(500);
  ControlSurface.setLED(0, 0);
  ControlSurface.setLED(1, 0);
  ControlSurface.setLED(2, 0);

  setChordButtonsMajorTransposed(ROOT_D);

}

void loop() {
  unsigned long milliseconds = millis();
  if(milliseconds > nextHeartbeat)
  {
    heartbeat = !heartbeat;
    digitalWrite(P_LED, heartbeat);
    nextHeartbeat = milliseconds + 500;
  }

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  if(ControlSurface.scan(milliseconds))
  {

    byte bChangeOfOctave = 0;
    switch(ControlSurface.commandKey)
    {
      case CControlSurface::K_OCT_DOWN:
      if(octave > MIN_OCTAVE)
        --octave;
      bChangeOfOctave = 1;                                                                         
      break;
      case CControlSurface::K_OCT_UP:
      if(octave < MAX_OCTAVE)
        ++octave;
      bChangeOfOctave = 1;
      break;
    }

    CHORD_TYPE chord;    
    determineChord(octave, chord);
    byte notes[20];
    int len = buildChord(chord, notes);
    playChord(notes, len);    

    renderNotesHeld();
    if(bChangeOfOctave)
    {
      char msg[8] = { "Oct X" };
      msg[4] = '0' + octave;
      Display.showRow(0, msg);
    }
    else
    {
      Display.showRow(0, notesHeldText, 16);
    }
    char chordName[17] = {0};
    getChordName(chord, chordName);
      Display.showRow(1, chordName);
    //Display.showRow(1, notesHeldGraphic, 16);
  }  
}


/*

  0123456789012345
  0123456789012345
  
  octave
  
  
*/
