//#define CHORD_TYPE unsigned int
// 7654321076543210
// oooo--iiccccrrrr
//
// oooo Octave
// ii   Inversion
// cccc Chord type
// rrrr Root note

typedef unsigned int CHORD_TYPE;
#define MK_CHORD_TYPE(root, chord) (unsigned long)((root)&0x0F)| ((unsigned long)((chord)&0x0F)<<4)

enum 
{
  ROOT_MASK    = 0x000F,
  ROOT_NONE    = 0x0000,
  ROOT_C       = 0x0001,
  ROOT_CSHARP  = 0x0002,
  ROOT_D       = 0x0003,
  ROOT_DSHARP  = 0x0004,
  ROOT_E       = 0x0005,
  ROOT_F       = 0x0006,
  ROOT_FSHARP  = 0x0007,
  ROOT_G       = 0x0008,
  ROOT_GSHARP  = 0x0009,
  ROOT_A       = 0x000A,
  ROOT_ASHARP  = 0x000B,
  ROOT_B       = 0x000C
};

enum 
{
  CHORD_MASK   = 0x00F0,
  CHORD_NONE   = 0x0000,
  CHORD_MAJ    = 0x0010,
  CHORD_MAJ7   = 0x0020,
  CHORD_MIN7   = 0x0030,
  CHORD_6      = 0x0040,
  CHORD_MIN6   = 0x0050,
  CHORD_9      = 0x0060,
  CHORD_DIM    = 0x0070,
  CHORD_SUS4   = 0x0080,
  CHORD_MIN    = 0x0090,
  CHORD_7      = 0x00A0
};

enum
{
  INV_MASK     = 0x0300,
  INV_NONE     = 0x0000,
  INV_FIRST    = 0x0100,
  INV_SECOND   = 0x0200
};

enum 
{
  KEY_MAJOR = 0,
  KEY_MINOR,
  KEY_MAX
};

class CChordType
{
public:  
  static void getNoteName(int note, char*&pos)
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
  
  static void getKeyTypeSuffix(int keyType, char*&pos)
  {
    switch(keyType)
    {
      case KEY_MAJOR:
      break;
      case KEY_MINOR:
      *pos++ = 'm'; 
      break;
    default:
      *pos++ = '?'; 
      break;
    }
  }
  
  // does not null terminate
  static void getChordName(CHORD_TYPE chord, char *chordName, byte full)
  {
    if(CHORD_NONE == chord)
      return;
  
    char *pos = chordName;
    int rootNote = (chord & ROOT_MASK);
    
    
    getNoteName(rootNote, pos);
  
    //  byte octave = (chord>>12) & 0x0F;
    //  chordName[pos++] = '0' + octave: break;
  
    switch(chord & CHORD_MASK)
    {
    case CHORD_MAJ:    
      if(full)
      {
        *pos++ = 'm'; 
        *pos++ = 'a'; 
        *pos++ = 'j'; 
      }
      break;
    case CHORD_MAJ7:   
      if(full)
      {
        *pos++ = 'm'; 
        *pos++ = 'a'; 
        *pos++ = 'j'; 
        *pos++ = '7'; 
      }
      else
      {
        *pos++ = 'M'; 
        *pos++ = '7'; 
      }
      break;
    case CHORD_MIN7:   
      if(full)
      {
        *pos++ = 'm'; 
        *pos++ = 'i'; 
        *pos++ = 'n'; 
        *pos++ = '7'; 
      }
      else
      {
        *pos++ = 'm'; 
        *pos++ = '7'; 
      }
      break;
    case CHORD_6:      
      if(full)
      {
        *pos++ = '6'; 
        *pos++ = 't'; 
        *pos++ = 'h'; 
      }
      else
      {
        *pos++ = '6'; 
      }
      break;
    case CHORD_MIN6:   
      if(full)
      {
        *pos++ = 'm'; 
        *pos++ = 'i'; 
        *pos++ = 'n'; 
        *pos++ = '6'; 
      }
      else    
      {
        *pos++ = 'm'; 
        *pos++ = '6'; 
      }
      break;
    case CHORD_9:      
      if(full)
      {
        *pos++ = '9'; 
        *pos++ = 't'; 
        *pos++ = 'h'; 
      }
      else
      {
        *pos++ = '9'; 
      }
      break;
    case CHORD_DIM:    
      if(full)
      {
        *pos++ = 'd'; 
        *pos++ = 'i'; 
        *pos++ = 'm'; 
      }
      else
      {
        *pos++ =  0b11011111;
      }
      break;
    case CHORD_SUS4:   
      if(full)
      {
        *pos++ = 's'; 
        *pos++ = 'u'; 
        *pos++ = 's'; 
        *pos++ = '4'; 
      }
      else
      {
        *pos++ = 's'; 
        *pos++ = '4';
      }
      break;
    case CHORD_MIN:    
      if(full)
      {
        *pos++ = 'm'; 
        *pos++ = 'i'; 
        *pos++ = 'n'; 
      }
      else
      {
        *pos++ =  'm';
      }
      break;
    case CHORD_7:      
      if(full)
      {
        *pos++ = '7'; 
        *pos++ = 't'; 
        *pos++ = 'h'; 
      }
      else
      {
        *pos++ = '7'; 
      }
      break;
    case CHORD_NONE:
      break;
    default:           
      *pos++ = '?'; 
      break;
    }
  
    if(full)
    {
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
      
      int octave = chord >> 12;
      *pos++ = ' ';
      *pos++ = '[';
      *pos++ = '0' + octave;    
      *pos++ = ']';
    }
  }

};
