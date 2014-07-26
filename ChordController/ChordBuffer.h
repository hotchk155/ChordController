extern void getChordName(CHORD_TYPE chord, char *chordName, byte full);

#define MAX_CHORD_BUFFER 20

class CChordBuffer
{
  CHORD_TYPE buffer[MAX_CHORD_BUFFER];
  int current;
  int count;
  int base;
  byte isSelectionChanged;
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
    count = 0;
    base = 0;
    isSelectionChanged = 0;
  }
  void movePrev()
  {
    if(current > 0)
    {
      --current;
      if(current < base)
        base = current;
      isSelectionChanged = 1;
    }
  }
  void moveNext()
  {
    if(current < count)
    {
      current++;
      if(current > base+2)
        base = current - 2;
    }
    isSelectionChanged = 1;
  }
  void insertItem()
  {
  }
  void deleteItem()
  {
  }
  void notify(CHORD_TYPE chord)
  {
    if(CHORD_NONE == chord)
      return;
      
    // insert chord at current position
    buffer[current] = chord;
    if(current == count)
    {  
      if(count == MAX_CHORD_BUFFER - 1) 
      {
        // out of space, shift everything down one
        for(int i = 1; i < MAX_CHORD_BUFFER-1; ++i)
          buffer[i-1] = buffer[i];
      }
      else
      {
       current++;
       count++;
       if(current > base+2)
         ++base;
      }
    }
  }
  // 0123456789012345
  // [A#M7]A#M7 A#M7
  void render(char *buf) // buf must be at least 16 characters
  {
    memset(buf, ' ', 16);
    int index = base;
    int pos = 0;
    for(int i=0; i<3 && index <= count; ++i)
    {
      if(index == current)
        buf[pos] = '[';
      ++pos;
      if(index == count)
      {
        buf[pos++] = '.';
        buf[pos++] = '.';
        buf[pos++] = '.';
        buf[pos++] = '.';
      }
      else
      {
        getChordName(buffer[index], &buf[pos], 0);
        pos+=4;
      }
      if(index == current)
        buf[pos] = ']';        
      ++index;
    }    
    if(index == MAX_CHORD_BUFFER-1)
      buf[15]='$';
  }
  byte isChanged()
  {
    if(isSelectionChanged)
    {
      isSelectionChanged = 0;
      return 1;
    }
    return 0;
  }
  CHORD_TYPE getChordSelection()
  {
    if(count == 0)
      return CHORD_NONE;
    return buffer[current];
  }
};

