 
// include the library code:
#include "Arduino.h"
#include <LiquidCrystal.h>

#include "Display.h"
#include "ControlSurface.h"
#include "ChordType.h"
#include "ChordBuffer.h"
#include "ChordSelection.h"
#include "ChordPlayer.h"
#include "Heartbeat.h"

CHORD_TYPE currentChord;
#define FIRMWARE_VERSION "0.2"



enum 
{
  CMD_MODE,
  CMD_LAYOUT,
  CMD_KEYTYPE,
  CMD_HOLD,
  CMD_DISPTYPE,
  CMD_RETRIG,
  CMD_STOP,
  
  CMD_PREV,
  CMD_NEXT,
  CMD_RESET,
  CMD_FIRST,
  CMD_INSERT,
  CMD_DELETE
};


enum 
{ 
  DISP_CALC,        // chord calculator mode
  DISP_LIST        // chord list mode
};

byte displayMode = DISP_LIST;
byte isModePressed = 0;
byte repaint = 0;
byte nextButtonPressed = 0;
byte isShiftPressed = 0;

CControlSurface ControlSurface;
CDisplay        Display;
CChordSelection ChordSelection;
CChordBuffer    ChordBuffer;
CChordPlayer    ChordPlayer;
CHeartbeat      Heartbeat;

//////////////////////////////////////////////////////////////////
void copyChars(char * dest, char *src)
{
  while(*src)
    *dest++=*src++;
}

////////////////////////////////////////
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
    CChordType::getNoteName(ChordSelection.getScaleRoot(), pch);
    CChordType::getKeyTypeSuffix(ChordSelection.getKeyType(), pch);
    break;
    case CChordSelection::LAYOUT_CHORD_FAMILY:
    copyChars(&row1[0], "Fam");
    CChordType::getNoteName(ChordSelection.getScaleRoot(), pch);
    CChordType::getKeyTypeSuffix(ChordSelection.getKeyType(), pch);
    break;
  default:
    copyChars(&row1[0], "???");
    break;
  }

  if(ChordSelection.getHoldMode())
    copyChars(&row1[4], "Hold");
  else
    copyChars(&row1[4], "Damp");
  
  if(ChordPlayer.getRetrig())
    copyChars(&row2[4], "RTrg");
  else
    copyChars(&row2[4], "Tied");

  switch(displayMode)
  {
    case DISP_CALC:
      copyChars(&row1[9], "Calc");
      break;
    case DISP_LIST:
      copyChars(&row1[9], "List");
      break;
    default:
      copyChars(&row1[9], "????");
      break;
  }
  
  
  Display.showRow(0, row1);
  Display.showRow(1, row2);
}

//////////////////////////////////////////////////////////////////
void onCommand(int which)
{
  repaint = 1; 
  switch(which)
  {
  case CMD_LAYOUT:
    ChordSelection.toggleChordLayout(); 
    break;
  case CMD_KEYTYPE:
    ChordSelection.toggleKeyType();
    break;
  case CMD_HOLD: 
    ChordSelection.toggleHoldMode(); 
    break;
  case CMD_STOP: 
    ChordSelection.stopChord(); 
    break;
  case CMD_RETRIG: 
    ChordPlayer.toggleRetrig(); 
    break;
  case CMD_DISPTYPE:
    displayMode = (displayMode == DISP_LIST)? DISP_CALC:DISP_LIST;
    break;
  case CMD_FIRST:
    ChordBuffer.moveFirst();
    break;
  case CMD_PREV:
    ChordBuffer.movePrev();
    break;
  case CMD_NEXT:
    ChordBuffer.moveNext();
    break;
  case CMD_RESET:
    ChordBuffer.reset();
    break;
  case CMD_INSERT:
    ChordBuffer.insertItem();
    break;
  case CMD_DELETE:
    ChordBuffer.deleteItem();
    break;
  }
}

////////////////////////////////////////
void onKeyEvent(unsigned long which, boolean isPress)
{
  switch(which)
  {
    case CControlSurface::K_F1: 
      break;
    case CControlSurface::K_F2: 
      isModePressed = isPress; 
      repaint = 1; 
      break;
    case CControlSurface::K_F3: 
      break;
    case CControlSurface::K_F4:     
      isShiftPressed = isPress; 
      break;      
    // FIRST INVERSION
    case CControlSurface::K_INV1:     
      if(isModePressed && isPress) 
        onCommand(CMD_LAYOUT); 
      else 
        ChordSelection.onInversionButton(1, isPress); 
    break;
    // CHORD 0
    case CControlSurface::K_CHORD0:   
      if(isModePressed && isPress) 
        onCommand(CMD_KEYTYPE); 
      else 
        ChordSelection.onChordButton(0, isPress); 
    break;
    // CHORD 1
    case CControlSurface::K_CHORD1:   
      if(isModePressed && isPress) 
        onCommand(CMD_HOLD); 
      else 
        ChordSelection.onChordButton(1, isPress); 
      break;
    // CHORD 2
    case CControlSurface::K_CHORD2:   
      if(isModePressed && isPress) 
        onCommand(CMD_RETRIG); 
      else 
        ChordSelection.onChordButton(2, isPress); 
      break;
    // CHORD 3
    case CControlSurface::K_CHORD3:   
      if(isModePressed && isPress) 
        onCommand(CMD_DISPTYPE); 
      else 
        ChordSelection.onChordButton(3, isPress); 
      break;
    // CHORD 4
    case CControlSurface::K_CHORD4:   
      if(isModePressed && isPress) 
        ; 
      else 
        ChordSelection.onChordButton(4, isPress); 
      break;
    // CHORD 5
    case CControlSurface::K_CHORD5:   
      if(isModePressed && isPress) 
        ; 
      else 
        ChordSelection.onChordButton(5, isPress); 
      break;
    // CHORD 6
    case CControlSurface::K_CHORD6:   
      if(isShiftPressed && isPress) 
        onCommand(CMD_RESET); 
      else 
        ChordSelection.onChordButton(6, isPress); 
      break;
    // CHORD 7
    case CControlSurface::K_CHORD7:   
      if(isShiftPressed && isPress) 
        onCommand(CMD_DELETE); 
      else 
        ChordSelection.onChordButton(7, isPress); 
      break;
    // CHORD 8
    case CControlSurface::K_CHORD8:   
      if(isShiftPressed && isPress) 
        onCommand(CMD_INSERT); 
      else 
        ChordSelection.onChordButton(8, isPress); 
      break;
    // SECOND INVERSION
    case CControlSurface::K_INV2:     
      if(isShiftPressed && isPress) 
        onCommand(CMD_STOP); 
      else 
        ChordSelection.onInversionButton(2, isPress); 
      break;
    // OCTAVE DOWN
    case CControlSurface::K_OCT_DOWN: 
      if(isShiftPressed && isPress) 
      {
        ChordSelection.downOctave(); 
      }
      else if(isPress)
      {
        if(nextButtonPressed)
          onCommand(CMD_FIRST); 
        else
          onCommand(CMD_PREV); 
      }
      break;
    // NOTE 0
    case CControlSurface::K_NOTE0:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_C, isPress);      
    else 
      ChordSelection.onNoteButton(0, isPress); 
    break;
    // NOTE 1
    case CControlSurface::K_NOTE1:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_CSHARP, isPress);
    else
      ChordSelection.onNoteButton(1, isPress); 
    break;
    // NOTE 2
    case CControlSurface::K_NOTE2:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_D, isPress);
    else
      ChordSelection.onNoteButton(2, isPress); 
    break;
    // NOTE 3
    case CControlSurface::K_NOTE3:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_DSHARP, isPress); 
    else
      ChordSelection.onNoteButton(3, isPress); 
    break;
    // NOTE 4
    case CControlSurface::K_NOTE4:    
    if(isModePressed)
      ChordSelection.onKeyModeButton(ROOT_E, isPress); 
    else
      ChordSelection.onNoteButton(4, isPress); 
    break;
    // NOTE 5
    case CControlSurface::K_NOTE5:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_F, isPress); 
    else
      ChordSelection.onNoteButton(5, isPress);
    break;
    // NOTE 6
    case CControlSurface::K_NOTE6:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_FSHARP, isPress); 
    else      
      ChordSelection.onNoteButton(6, isPress); 
    break;
    // NOTE 7
    case CControlSurface::K_NOTE7:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_G, isPress);
    else
      ChordSelection.onNoteButton(7, isPress); 
    break;
    // NOTE 8
    case CControlSurface::K_NOTE8:    
    if(isModePressed)
      ChordSelection.onKeyModeButton(ROOT_GSHARP, isPress); 
    else
      ChordSelection.onNoteButton(8, isPress); 
    break;
    // NOTE 9
    case CControlSurface::K_NOTE9:    
    if(isModePressed) 
      ChordSelection.onKeyModeButton(ROOT_A, isPress);
    else
      ChordSelection.onNoteButton(9, isPress); 
    break;
    // NOTE 10
    case CControlSurface::K_NOTE10:   
    if(isModePressed)
      ChordSelection.onKeyModeButton(ROOT_ASHARP, isPress); 
    else
      ChordSelection.onNoteButton(10, isPress); 
    break;
    // NOTE 11
    case CControlSurface::K_NOTE11:   
    if(isModePressed)
      ChordSelection.onKeyModeButton(ROOT_B, isPress); 
    else
      ChordSelection.onNoteButton(11, isPress); 
    break;
    // OCTAVE UP
    case CControlSurface::K_OCT_UP:   
      if(isPress)
      {
        if(isShiftPressed) 
        {
          ChordSelection.upOctave(); 
        }
        else 
        {
          onCommand(CMD_NEXT); 
          nextButtonPressed = 1;
        }
      }
      else
      {
        nextButtonPressed = 0;         
      }
      break;
  }
}

//////////////////////////////////////////////////////////////////
byte heartbeat = 0;
unsigned long nextHeartbeat;
void setup() 
{
  nextHeartbeat = 0;
  // set up the LCD's number of columns and rows: 

  Heartbeat.setup();
  ControlSurface.setup(onKeyEvent);
  Display.setup();
  ChordBuffer.setup();
  ChordPlayer.setup();
  delay(1);
  Display.showRow(0, "--== MIDIOT ==--");
  Display.showRow(1, "F/W version " FIRMWARE_VERSION);
  ControlSurface.setLED(0, 1);
  ControlSurface.setLED(1, 1);
  ControlSurface.setLED(2, 1);
  delay(2000);
  ControlSurface.setLED(0, 0);
  ControlSurface.setLED(1, 0);
  ControlSurface.setLED(2, 0);

  ChordSelection.layoutChordButtons();
  ChordSelection.layoutNoteButtons(ROOT_C, KEY_MAJOR, CChordSelection::LAYOUT_FREE);

  isModePressed = 0;
  isShiftPressed = 0;
  repaint = 1;
  nextButtonPressed = 0;
}


void loop() {

  unsigned long milliseconds = millis();
  Heartbeat.run(milliseconds);
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  ControlSurface.run(milliseconds);
  byte newChordSelection = ChordSelection.isChanged();
  byte chordRecall = ChordBuffer.isChordRecall();

  if(repaint || newChordSelection || chordRecall)
  {
    if(chordRecall)
    {
      currentChord = ChordBuffer.getChordSelection();
      ChordPlayer.playChord(currentChord);
    }
    else if(newChordSelection)
    {
      currentChord = ChordSelection.getChordSelection();
      ChordBuffer.notify(currentChord);
      ChordPlayer.playChord(currentChord);
    }

    if(isModePressed)
    {
      showMode();
    }    
    else if(displayMode == DISP_CALC)
    {
      char notesHeldText[16];
      char notesHeldGraphic[16];
      ChordPlayer.renderNotesHeld(notesHeldText, notesHeldGraphic);
      Display.showRow(0, notesHeldText, 16);
      Display.showRow(1, notesHeldGraphic, 16);            
    }
    else
    {
      char buf[16];
      memset(buf,' ',16);
      if(CHORD_NONE == currentChord)
        copyChars(buf, "....");
      else
        CChordType::getChordName(currentChord, buf, 1);       
      buf[13] = '[';
      buf[14] = '0' + ChordSelection.getOctaveSelection();
      buf[15] = ']';
      Display.showRow(0, buf, 16);
      ChordBuffer.render(buf);
      Display.showRow(1, buf, 16);
    }
    repaint=0;
  }
}

