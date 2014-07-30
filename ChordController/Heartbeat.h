#define P_LED 13
class CHeartbeat
{
   unsigned long nextEvent;
   byte state;
  public:
    void setup()
    {
      pinMode(P_LED, OUTPUT);
      state = 0;
      nextEvent = 0;
    }
    void run(unsigned long milliseconds)
    {
      if(milliseconds > nextEvent)
      {
        state = !state;
        digitalWrite(P_LED, state);
        nextEvent = milliseconds + 500;
      }
    }
};
