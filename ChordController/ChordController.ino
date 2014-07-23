
// include the library code:
#include "Arduino.h"
#include <LiquidCrystal.h>

#include "ChordController.h"
#include "ChordSelection.h"
#include "ControlSurface.h"
#include "Display.h"

#define P_LED 13
byte noteHeld[130];
char notesHeldText[30];
char notesHeldGraphic[30];


enum 
{
  CMD_MODE,
  CMD_FRE,
  CMD_KEY,
  CMD_FAM,
  CMD_HOLD
};


enum 
{ 
  DISP_CALC,        // chord calculator mode
  DISP_LIST,        // chord list mode

  DISP_MODE         // mode screen
};

byte displayMode = DISP_LIST;
byte chordDisplayMode = DISP_LIST;
byte isModePressed = 0;
byte repaint = 0;

CControlSurface ControlSurface;
CDisplay        Display;
CChordSelection ChordSelection;





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

void getNoteName(int note, char*&pos)
{
  switch(note)
  {
  case ROOT_C:       
    *pos++ = 'C'; 
    break;
  case ROOT_CSHARP:  
    *pos++ = 'C'; 
    *pos++ = CH_SH; 
    break;
  case ROOT_D:       
    *pos++ = 'D'; 
    break;
  case ROOT_DSHARP:  
    *pos++ = 'D'; 
    *pos++ = CH_SH; 
    break;
  case ROOT_E:       
    *pos++ = 'E'; 
    break;
  case ROOT_F:       
    *pos++ = 'F'; 
    break;
  case ROOT_FSHARP:  
    *pos++ = 'F'; 
    *pos++ = CH_SH; 
    break;
  case ROOT_G:       
    *pos++ = 'G'; 
    break;
  case ROOT_GSHARP:  
    *pos++ = 'G'; 
    *pos++ = CH_SH; 
    break;
  case ROOT_A:       
    *pos++ = 'A'; 
    break;
  case ROOT_ASHARP:  
    *pos++ = 'A'; 
    *pos++ = CH_SH; 
    break;
  case ROOT_B:       
    *pos++ = 'B'; 
    break;
  default:           
    *pos++ = '?'; 
    break;
  }
}

void getKeyTypeSuffix(int keyType, char*&pos)
{
  switch(keyType)
  {
    case CChordSelection::KEY_MAJOR:
    break;
    case CChordSelection::KEY_MINOR:
    *pos++ = 'm'; 
    break;
  default:
    *pos++ = '?'; 
    break;
  }
}

void getChordName(CHORD_TYPE chord, char *chordName)
{
  *chordName = 0;
  if(CHORD_NONE == chord)
    return;

  int rootNote = (chord & ROOT_MASK);
  char *pos = chordName;
  getNoteName(rootNote, pos);


  //  byte octave = (chord>>12) & 0x0F;
  //  chordName[pos++] = '0' + octave: break;

  switch(chord & CHORD_MASK)
  {
  case CHORD_MAJ:    
    break;
  case CHORD_MAJ7:   
    *pos++ = 'M'; 
    *pos++ = '7'; 
    break;
  case CHORD_MIN7:   
    *pos++ = 'm'; 
    *pos++ = '7'; 
    break;
  case CHORD_6:      
    *pos++ = '6'; 
    break;
  case CHORD_MIN6:   
    *pos++ = 'm'; 
    *pos++ = '6'; 
    break;
  case CHORD_9:      
    *pos++ = '9';
    break;
  case CHORD_DIM:    
    *pos++ =  0b11011111;
    break;
  case CHORD_SUS4:   
    *pos++ = 's'; 
    *pos++ = '4';
    break;
  case CHORD_MIN:    
    *pos++ = 'm'; 
    break;
  case CHORD_7:      
    *pos++ = '7'; 
    break;
  case CHORD_NONE:
    break;
  default:           
    *pos++ = '?'; 
    break;
  }

  int firstInversionRoot = 4;
  int secondInversionRoot = 7;
  switch(chord & CHORD_MASK)
  {
  case CHORD_MAJ:   
    firstInversionRoot = 4; 
    secondInversionRoot = 7; 
    break;
  case CHORD_MAJ7:  
    firstInversionRoot = 4; 
    secondInversionRoot = 7; 
    break;
  case CHORD_6:     
    firstInversionRoot = 4; 
    secondInversionRoot = 9; 
    break;
  case CHORD_9:     
    firstInversionRoot = 4; 
    secondInversionRoot = 7; 
    break;
  case CHORD_7:     
    firstInversionRoot = 4; 
    secondInversionRoot = 7; 
    break;
  case CHORD_MIN7:  
    firstInversionRoot = 3; 
    secondInversionRoot = 7; 
    break; 
  case CHORD_MIN6:  
    firstInversionRoot = 3; 
    secondInversionRoot = 9; 
    break;
  case CHORD_MIN:   
    firstInversionRoot = 3; 
    secondInversionRoot = 7; 
    break;
  case CHORD_DIM:   
    firstInversionRoot = 3; 
    secondInversionRoot = 6; 
    break;
  case CHORD_SUS4:  
    firstInversionRoot = 5; 
    secondInversionRoot = 7; 
    break;            
  }

  switch(chord & INV_MASK)
  {
  case INV_FIRST:
    rootNote += firstInversionRoot;
    if(rootNote > ROOT_B)      
      rootNote -= 12;
    *pos++ = '/';
    getNoteName(rootNote, pos);
    break;        
  case INV_SECOND:
    rootNote += secondInversionRoot;
    if(rootNote > ROOT_B)      
      rootNote -= 12;
    *pos++ = '/';
    getNoteName(rootNote, pos);
    break;        
  }

};


/*
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
 */
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


void copyChars(char * dest, char *src)
{
  while(*src)
    *dest++=*src++;
}
////////////////////////////////////////
//  0123456789012345
// 0FAM    
// 1F#m Oct4
//
void showMode()
{
  char row1[16];
  char row2[16];
  memset(row1, ' ', 16);
  memset(row2, ' ', 16);
  char *pch = row2;
  switch(ChordSelection.getChordLayout())
  {
    case CChordSelection::LAYOUT_FREE:
    copyChars(&row1[0], "Fre");
    copyChars(&row2[0], "---");
    break;
    case CChordSelection::LAYOUT_KEY_SIGNATURE:
    copyChars(&row1[0], "Key");
    getNoteName(ChordSelection.getScaleRoot(), pch);
    getKeyTypeSuffix(ChordSelection.getKeyType(), pch);
    break;
    case CChordSelection::LAYOUT_CHORD_FAMILY:
    copyChars(&row1[0], "Fam");
    getNoteName(ChordSelection.getScaleRoot(), pch);
    getKeyTypeSuffix(ChordSelection.getKeyType(), pch);
    break;
  default:
    copyChars(&row1[0], "???");
    break;
  }
  copyChars(&row2[4], "Oct");
  row2[7] = '0' + ChordSelection.getOctaveSelection();

  if(ChordSelection.getHoldMode())
    copyChars(&row1[4], "Hold");
  
  Display.showRow(0, row1);
  Display.showRow(1, row2);
}

void onCommand(int which, boolean isPress)
{
  repaint = isPress;  
  switch(which)
  {
  case CMD_MODE: 
    isModePressed = isPress; 
    repaint = 1; 
    break;
  case CMD_FRE:
    ChordSelection.setChordLayout(CChordSelection::LAYOUT_FREE); 
    break;
  case CMD_KEY:
    if(ChordSelection.getChordLayout() == CChordSelection::LAYOUT_KEY_SIGNATURE)
      ChordSelection.onKeyTypeButton(which, isPress);
    else
      ChordSelection.setChordLayout(CChordSelection::LAYOUT_KEY_SIGNATURE); 
    break;
  case CMD_FAM:
    if(ChordSelection.getChordLayout() == CChordSelection::LAYOUT_CHORD_FAMILY)
      ChordSelection.onKeyTypeButton(which, isPress);
    else
      ChordSelection.setChordLayout(CChordSelection::LAYOUT_CHORD_FAMILY);
    break;
  case CMD_HOLD: 
    ChordSelection.toggleHoldMode(); 
    break;
  }
}

////////////////////////////////////////
void onKeyEvent(unsigned long which, boolean isPress)
{
  switch(which)
  {
    case CControlSurface::K_UP: 
    break;
    case CControlSurface::K_DOWN: 
    break;
    case CControlSurface::K_SPARE: 
    break;
    case CControlSurface::K_MODE:     
    onCommand(CMD_MODE, isPress); 
    break;      
    case CControlSurface::K_OCT_DOWN: 
    ChordSelection.onOctaveButton(-1, isPress); 
    break;
    case CControlSurface::K_OCT_UP:   
    ChordSelection.onOctaveButton(+1, isPress); 
    break;
    case CControlSurface::K_INV1:     
    if(isModePressed) onCommand(CMD_FRE, isPress); 
    else ChordSelection.onInversionButton(1, isPress); 
    break;
    case CControlSurface::K_CHORD0:   
    if(isModePressed) onCommand(CMD_KEY, isPress); 
    else ChordSelection.onChordButton(0, isPress); 
    break;
    case CControlSurface::K_CHORD1:   
    if(isModePressed) onCommand(CMD_FAM, isPress); 
    else ChordSelection.onChordButton(1, isPress); 
    break;
    case CControlSurface::K_CHORD2:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(2, isPress); 
    break;
    case CControlSurface::K_CHORD3:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(3, isPress); 
    break;
    case CControlSurface::K_CHORD4:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(4, isPress); 
    break;
    case CControlSurface::K_CHORD5:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(5, isPress); 
    break;
    case CControlSurface::K_CHORD6:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(6, isPress); 
    break;
    case CControlSurface::K_CHORD7:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(7, isPress); 
    break;
    case CControlSurface::K_CHORD8:   
    if(isModePressed) onCommand(CMD_HOLD, isPress); 
    else ChordSelection.onChordButton(8, isPress); 
    break;
    case CControlSurface::K_INV2:     
    if(isModePressed) ; 
    else ChordSelection.onInversionButton(2, isPress); 
    break;
    case CControlSurface::K_NOTE0:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_C, isPress); repaint=1; }
    else ChordSelection.onNoteButton(0, isPress); 
    break;
    case CControlSurface::K_NOTE1:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_CSHARP, isPress); repaint=1; }
    ChordSelection.onNoteButton(1, isPress); 
    break;
    case CControlSurface::K_NOTE2:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_D, isPress); repaint=1; }
    ChordSelection.onNoteButton(2, isPress); 
    break;
    case CControlSurface::K_NOTE3:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_DSHARP, isPress); repaint=1; }
    ChordSelection.onNoteButton(3, isPress); 
    break;
    case CControlSurface::K_NOTE4:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_E, isPress); repaint=1; }
    ChordSelection.onNoteButton(4, isPress); 
    break;
    case CControlSurface::K_NOTE5:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_F, isPress); repaint=1; }
    ChordSelection.onNoteButton(5, isPress);
    break;
    case CControlSurface::K_NOTE6:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_FSHARP, isPress); repaint=1; }
    ChordSelection.onNoteButton(6, isPress); 
    break;
    case CControlSurface::K_NOTE7:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_G, isPress); repaint=1; }
    ChordSelection.onNoteButton(7, isPress); 
    break;
    case CControlSurface::K_NOTE8:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_GSHARP, isPress); repaint=1; }
    ChordSelection.onNoteButton(8, isPress); 
    break;
    case CControlSurface::K_NOTE9:    
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_A, isPress); repaint=1; }
    ChordSelection.onNoteButton(9, isPress); 
    break;
    case CControlSurface::K_NOTE10:   
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_ASHARP, isPress); repaint=1; }
    ChordSelection.onNoteButton(10, isPress); 
    break;
    case CControlSurface::K_NOTE11:   
    if(isModePressed) { ChordSelection.onKeyModeButton(ROOT_B, isPress); repaint=1; }
    ChordSelection.onNoteButton(11, isPress); 
    break;
  }
}


byte heartbeat = 0;
unsigned long nextHeartbeat;
void setup() 
{
  pinMode(P_LED,OUTPUT);
  nextHeartbeat = 0;
  // set up the LCD's number of columns and rows: 

  ControlSurface.setup(onKeyEvent);
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

  ChordSelection.layoutChordButtons();
  ChordSelection.layoutNoteButtons(ROOT_C, CChordSelection::KEY_MAJOR, CChordSelection::LAYOUT_FREE);

  isModePressed = 0;
  repaint = 0;


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
  ControlSurface.run(milliseconds);

  if(isModePressed)
  {
    if(repaint)
    {
      showMode();
    }
  }  
  else if(repaint || ChordSelection.isChanged())
  {
    CHORD_TYPE chord = ChordSelection.getChordSelection();
    char chordName[16] = {
      0    };
    getChordName(chord, chordName);
    Display.showRow(0, chordName);
    Display.showRow(1, "");

    /*    byte notes[20];
     int len = buildChord(chord, notes);
     playChord(notes, len);    
     renderNotesHeld();
     Display.showRow(0, notesHeldText, 16);
     Display.showRow(1, notesHeldGraphic);            */
  }

  /*
  if(repaint)
   {
   renderNotesHeld();
   switch(displayMode)
   {
   case DISP_CALC:
   {
   Display.showRow(0, notesHeldText, 16);
   Display.showRow(1, notesHeldGraphic);        
   }
   break;
   case DISP_LIST:
   {
   char chordName[17] = {
   0                };
   getChordName(chord, chordName);
   Display.showRow(0, notesHeldText, 16);
   Display.showRow(1, chordName);        
   }
   break;
   case DISP_MODE:
   showMode();      
   break;
   }
   }*/
}


/*

 0123456789012345
 0123456789012345
 
 octave
 
 
 */




