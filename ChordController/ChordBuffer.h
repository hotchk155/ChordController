extern void getChordName(CHORD_TYPE chord, char *chordName, byte full);

#define MAX_CHORD_BUFFER 17

class CChordBuffer
{
  CHORD_TYPE buffer[MAX_CHORD_BUFFER];
  int current;
  int base;
  byte chordRecall;
public:
  void setup()
  {
    reset();
  }
  void reset()
  {
    for(int i=0; i<MAX_CHORD_BUFFER; ++i)
      buffer[i] = CHORD_NONE;
    current = 0;
    base = 0;
    chordRecall = 0;
  }
  void shiftLeft()
  {
    for(int i=0; i<MAX_CHORD_BUFFER-1; ++i)
      buffer[i] = buffer[i+1];
    buffer[MAX_CHORD_BUFFER-1] = CHORD_NONE;
  }
  void moveFirst()
  {
    current = 0;
    base = 0;
    chordRecall = 1;
  }
  void movePrev()
  {
    if(current > 0)
    {
      --current;
      if(current < base)
        base = current;
      chordRecall = 1;
    }
  }
  void moveNext()
  {
    if(buffer[current] == CHORD_NONE)
      return;
      
    if(current == MAX_CHORD_BUFFER-1)
    {
      shiftLeft();
    }    
    else 
    {
      current++;
      if(current > base+2)
        base = current - 2;
    }
    chordRecall = 1;
  }
  void insertItem()
  {
    for(int i=MAX_CHORD_BUFFER-1; i>current; --i)
      buffer[i] = buffer[i-1];
    chordRecall = 1;
  }
  void deleteItem()
  {
    for(int i=current; i<MAX_CHORD_BUFFER-1; ++i)
      buffer[i] = buffer[i+1];
    buffer[MAX_CHORD_BUFFER-1] = CHORD_NONE;
    chordRecall = 1;
  }
  void notify(CHORD_TYPE chord)
  {
    if(CHORD_NONE == chord)
      return;
      
    // insert chord at current position
    if(CHORD_NONE == buffer[current])
    {  
      buffer[current] = chord;    
      if(current == MAX_CHORD_BUFFER - 1) 
      {
        // out of space, shift everything down one
        shiftLeft();
      }
      else
      {
       current++;
       if(current > base+2)
         ++base;
      }
    }
    else
    {
      buffer[current] = chord;    
    }
  }
  // 0123456789012345
  // [A#M7]A#M7 A#M7
  void render(char *buf) // buf must be at least 16 characters
  {
    memset(buf, ' ', 16);
    int index = base;
    int pos = 0;
    for(int i=0; i<3 && index < MAX_CHORD_BUFFER; ++i)
    {
      if(index == current)
        buf[pos] = '[';
      ++pos;
      if(buffer[index] == CHORD_NONE)
      {
        if(index == MAX_CHORD_BUFFER-1)
        {
          buf[pos++] = '$';
          buf[pos++] = '$';
          buf[pos++] = '$';
          buf[pos++] = '$';
        }
        else
        {
          buf[pos++] = '.';
          buf[pos++] = '.';
          buf[pos++] = '.';
          buf[pos++] = '.';
        }
      }
      else
      {
        CChordType::getChordName(buffer[index], &buf[pos], 0);
        pos+=4;
      }
      if(index == current)
        buf[pos] = ']';        
      if(buffer[index] == CHORD_NONE)
        break;
      ++index;
    }    
  }
  byte isChordRecall()
  {
    if(chordRecall)
    {
      chordRecall = 0;
      return 1;
    }
    return 0;
  }
  CHORD_TYPE getChordSelection()
  {
    return buffer[current];
  }
};

