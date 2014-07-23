//////////////////////////////////////////////////////////////////////////////
// This class defines the selection of chord made by the user at the keyboard
#ifndef __CHORDSELECTION_H__
#define __CHORDSELECTION_H__

#define NUM_NOTE_BUTTONS    12
#define NUM_CHORD_BUTTONS   9
#define MIN_OCTAVE 0
#define MAX_OCTAVE 8
#define DEFAULT_OCTAVE 4

class CChordSelection
{
  ///////////////////////////////////////////////////////
  // Helper method to lay out major key chord family
  int getChordTypeMajorKey(int which)
  {
    switch(which)
    {
    case 0: 
      return CHORD_MAJ;   // i
    case 1: 
      return CHORD_MIN; 
    case 2: 
      return CHORD_MIN;   // ii
    case 3: 
      return CHORD_MAJ; 
    case 4: 
      return CHORD_MIN;   // iii
    case 5: 
      return CHORD_MAJ;   // iv
    case 6: 
      return CHORD_DIM; 
    case 7: 
      return CHORD_MAJ;   // v
    case 8: 
      return CHORD_MAJ; 
    case 9: 
      return CHORD_MIN;   // vi
    case 10: 
      return CHORD_MAJ; 
    case 11: 
      return CHORD_DIM;    // vi
    }
    return CHORD_NONE;
  }

  ///////////////////////////////////////////////////////
  // Helper method to lay out minor key chord family
  int getChordTypeMinorKey(int which)
  {
    switch(which)
    {
    case 0: 
      return CHORD_MIN;   // i
    case 1: 
      return CHORD_MAJ; 
    case 2: 
      return CHORD_DIM;   // ii
    case 3: 
      return CHORD_MAJ;   // iii
    case 4: 
      return CHORD_MIN; 
    case 5: 
      return CHORD_MIN;   // iv
    case 6: 
      return CHORD_DIM; 
    case 7: 
      return CHORD_MIN;   // v
    case 8: 
      return CHORD_MAJ;   // vi
    case 9: 
      return CHORD_MIN; 
    case 10: 
      return CHORD_MAJ;   // vii
    case 11: 
      return CHORD_MAJ;
    }
  }

  CHORD_TYPE  chordSelection;
  CHORD_TYPE  altChordPending;
  CHORD_TYPE  inversionType;
  CHORD_TYPE  noteButtons[NUM_NOTE_BUTTONS];
  CHORD_TYPE  chordButtons[NUM_CHORD_BUTTONS];
  byte isHoldMode;  
  byte changeOfChord;
  char currentNoteButton;
  char currentChordButton;
  char octaveSelection;
  byte scaleRoot;
  byte keyType;
  byte chordLayout;
  
public:  

  //////////////////////////////////////////////////////////////////////////////
  // Possible chord button layouts
  enum 
  {
    LAYOUT_FREE,
    LAYOUT_KEY_SIGNATURE,
    LAYOUT_CHORD_FAMILY
  };

  //////////////////////////////////////////////////////////////////////////////
  // Possible key types
  enum 
  {
    KEY_MAJOR = 0,
    KEY_MINOR,
    KEY_MAX
  };


  ///////////////////////////////////////////////////////
  CChordSelection()
  {
    chordSelection = CHORD_NONE;
    altChordPending = CHORD_NONE;
    inversionType = INV_NONE;
    memset(noteButtons,0,sizeof noteButtons);
    memset(chordButtons,0,sizeof chordButtons);
    isHoldMode = 1;  
    changeOfChord = 0;
    currentNoteButton = -1;
    currentChordButton = -1;
    octaveSelection = DEFAULT_OCTAVE;
    scaleRoot = ROOT_C;
    keyType = KEY_MAJOR;
    chordLayout = LAYOUT_FREE;

  }
  

  /////////////////////////////////////////////////////////////////////
  // Return the selected chord
  CHORD_TYPE getChordSelection()
  {
    return (((CHORD_TYPE)octaveSelection)<<12) | inversionType | chordSelection;
  }

  /////////////////////////////////////////////////////////////////////
  // Determine if a new chord selection has been made
  byte isChanged()
  {
    if(changeOfChord)
    {
      changeOfChord = 0;
      return 1;
    }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////
  // Lay out the note buttons according to a root, key and layout type
  void layoutNoteButtons(int root, int key, int layout)
  {
    scaleRoot = root;
    keyType = key;
    chordLayout = layout;
    
    int order[12] = {
      0,1,2,4,3,5,6,7,9,8,11,10    };
    for(int i=0; i<12; ++i)
    {    
      if(root > ROOT_B)
        root = ROOT_C;
      if(LAYOUT_KEY_SIGNATURE == layout)
      {
        if(KEY_MINOR == keyType)
          noteButtons[i] = (ROOT_C + i) | getChordTypeMinorKey(root - 1);  
        else
          noteButtons[i] = (ROOT_C + i) | getChordTypeMajorKey(root - 1);  
      }
      else if(LAYOUT_CHORD_FAMILY == layout)
      {
        if(KEY_MINOR == keyType)
          noteButtons[noteButtons[order[i]]] = root | getChordTypeMinorKey(i);  
        else
          noteButtons[i] = root | getChordTypeMajorKey(i);  
      }
      else
      {
        noteButtons[i] = root | CHORD_MAJ;  
      }
      ++root;
    }
  }

  /////////////////////////////////////////////////////////////////////
  // Lay out the chord type buttons
  void layoutChordButtons()
  {
    int seq = 0;
    chordButtons[seq++] = CHORD_MAJ7;
    chordButtons[seq++] = CHORD_MIN7;
    chordButtons[seq++] = CHORD_6;
    chordButtons[seq++] = CHORD_MIN6;
    chordButtons[seq++] = CHORD_9;
    chordButtons[seq++] = CHORD_DIM;
    chordButtons[seq++] = CHORD_SUS4;
    chordButtons[seq++] = CHORD_MIN;
    chordButtons[seq++] = CHORD_7;
  }

  /////////////////////////////////////////////////////////////////////
  // CALLED WHEN A NOTE BUTTON IS PRESSED OR RELEASED
  void onNoteButton(int which, int isPress)  
  {
    if(which < 0 || which >= NUM_NOTE_BUTTONS)
      return;

    if(isHoldMode)
    {
      // Hold mode   
      if(isPress)
      {
        // New note button pressed
        chordSelection = noteButtons[which];
        if(altChordPending != CHORD_NONE)
        {
          chordSelection &= ~CHORD_MASK;
          chordSelection |= altChordPending;
          altChordPending = CHORD_NONE;
        }
        currentNoteButton = which;        
        changeOfChord = 1;
      }
      else if(which == currentNoteButton)
      {
        // current note button released
        currentNoteButton = -1;
      }
    }
    else
    {   
      // Standard mode   
      if(isPress)
      {        
        // start with the basic chord type for the note button
        currentNoteButton = which;        
        chordSelection = noteButtons[which];
        if(currentChordButton >= 0)
        {
          // A chord button is also pressed, so apply that chord type
          chordSelection &= ~CHORD_MASK;
          chordSelection |= chordButtons[currentChordButton];
        }
        changeOfChord = 1;
      }
      // check if the note button being released is the active one
      // (it could be a previously held one which got superseded by
      // a new one)
      else if(which == currentNoteButton)
      {
        // Stop playing chord
        currentNoteButton = -1;
        chordSelection = CHORD_NONE;        
        changeOfChord = 1;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////
  // CALLED WHEN A CHORD BUTTON IS PRESSED OR RELEASED
  void onChordButton(int which, int isPress)
  {
    if(which < 0 || which >= NUM_CHORD_BUTTONS)
      return;

    if(isHoldMode)  // Hold mode
    {
      if(isPress)
      {
        // New chord button pressed
        if(currentNoteButton < 0)
        {
          // Theere is not currently any note button pressed, so this alternative 
          // chord will be applied to the next pressed chord button
          altChordPending = chordButtons[which];
        }
        else
        {          
          // Apply this chord variation to the currenty held note
          chordSelection = noteButtons[currentNoteButton];
          chordSelection &= ~CHORD_MASK;
          chordSelection |= chordButtons[which];
          changeOfChord = 1;
        }
        currentChordButton = which;  
      }
    }
    else 
    {
      if(isPress)
      {
        currentChordButton = which;  
        // In this mode the chord buttons only have an effect when
        // a note button is held at the same time
        if(currentNoteButton>=0)  
        {    
          // Play the chord defined by the note and chord button together
          chordSelection = noteButtons[currentNoteButton];
          chordSelection &= ~CHORD_MASK;
          chordSelection |= chordButtons[which];
          changeOfChord = 1;
        }
      }
      // check if the chord button being released is the active one
      // (it could be a previously held one which got superseded by
      // a new chord)
      else if(currentChordButton == which)
      {
        currentChordButton = -1;
        if(currentNoteButton>=0)  
        {
          // There is a note button held, so revert back to the basic
          // chord for that note button
          chordSelection = noteButtons[currentNoteButton];
          changeOfChord = 1;
        }
      }
    }        
  }     
  
  /////////////////////////////////////////////////////////////////////
  // Handler for when an inversion is selected
  void onInversionButton(int which, int isPress)  
  {
    if(isPress)
    {
      if(which > 1)      
        inversionType = INV_SECOND;
      else
        inversionType = INV_FIRST;
    }
    else
    {
      inversionType = INV_NONE;
    }
    changeOfChord = 1;        
  }

  /////////////////////////////////////////////////////////////////////
  // Handler for when an inversion is selected
  void onOctaveButton(int which, int isPress)  
  {
    if(isPress)
    {
      if(which>0 && octaveSelection < MAX_OCTAVE)
      {
        ++octaveSelection;
        changeOfChord = 1;        
      }
      else if(which<0 && octaveSelection > MIN_OCTAVE)
      {
        --octaveSelection;
        changeOfChord = 1;        
      }
    }
  }

  /////////////////////////////////////////////////////////////////////
  void onKeyModeButton(byte which, byte isPress)
  {
    if(isPress)
      setScaleRoot(which);
  }

  /////////////////////////////////////////////////////////////////////
  void onKeyTypeButton(byte which, byte isPress)
  {
    if(isPress)
    {
      int k = keyType + 1;
      if(k>=KEY_MAX)
        k = 0;
      setKeyType(k);
    }
  }

  void toggleHoldMode()  
  {
    isHoldMode = !isHoldMode;
  }
  byte getHoldMode()
  {
    return isHoldMode;
  }
  char  getOctaveSelection()
  {
    return octaveSelection;
  }
  void setScaleRoot(byte r)
  {
    if(r != scaleRoot)
      layoutNoteButtons(r, keyType, chordLayout);
  }
  byte getScaleRoot()
  {
    return scaleRoot;
  }
  void setKeyType(byte k)
  {
    if(k != keyType)
      layoutNoteButtons(scaleRoot, k, chordLayout);
  }
  byte getKeyType()
  {
    return keyType;
  }
  void setChordLayout(byte l)
  {
    if(l != chordLayout)
      layoutNoteButtons(scaleRoot, keyType, l);
  }
  byte getChordLayout()
  {
    return chordLayout;
  }

};


#endif // __CHORDSELECTION_H__

