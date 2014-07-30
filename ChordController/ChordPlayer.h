class CChordPlayer
{
  byte noteHeld[130];
  byte retrig;
  byte midiChan;

  //////////////////////////////////////////////////////////////////
  void midiNote(byte chan, byte note, byte vel)
  {
    Serial.write(0x90|chan);
    Serial.write(note&0x7F); 
    Serial.write(vel&0x7f);
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
      if(!noteHeld[ch] || retrig)
        midiNote(midiChan, ch, 127);
      noteHeld[ch] = 1;
    }
  
    // mute any old notes which are note in the new chord
    for(i=0; i<128; ++i)
    {
      if(noteHeld[i] == 2)
      {
        midiNote(midiChan, i, 0);
        noteHeld[i] = 0;
      }
    }
  }  
  
public:  
  void setup()
  {
    Serial.begin(31250);
    memset(noteHeld,0,sizeof noteHeld);
    retrig = 0;
    midiChan = 0;
  }
  void toggleRetrig()
  {
    retrig = !retrig;
  }
  byte getRetrig()
  {
    return retrig;
  }

  void playChord(CHORD_TYPE chord)
  {
    byte notes[20];    
    int len = buildChord(chord, notes);
    playChord(notes, len);
  }
  
  //////////////////////////////////////////////////////////////////
  void renderNotesHeld(char *notesHeldText, char *notesHeldGraphic) // buffers are 16 chars
  {
  
    // lookup tables of note names
    char *notes       = "CCDDEFFGGAAB";
  
    memset(notesHeldText, ' ', 16);
    memset(notesHeldGraphic, ' ', 16);  
  
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

};
