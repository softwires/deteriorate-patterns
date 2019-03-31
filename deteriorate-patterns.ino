/*
  Deteriorate-Patterns
  Version : 1.0
  Date    : 31-3-2019
  By      : SoftWires

  Variation on the classic repeating pattern / change random bit after looping the pattern.
  The program sends 2 patterns : parent and child. When initialized both 16 steps pattern ar the same. After 1 child cycle a randon position is eiher changed with a random value or the value of the same position of the parent : odds are 50%.
  After <<life>> child cycles the child becomes the parent.<<life>> is default 16, but can be set by D3 (note 62:4),Es3  (note 63:8) and E3 (note 64:16).
  The parent pattern can be frozen : freeze is toggled by key F3 (note 65).

  The output of the parent pulse is at pin 7, the output of the child at pin 8.
  The patterns are clocked by C4 (midi 72) notes. The initial parent pattern is a clear one (silence) (reset also at Midi Start).
  Length of the step is the note length : so this is more a gate pattern than a trigger generator. Timing of the steps depends on the timing of the input (so swing /micro timing are ' out of the box')

  Note ranges C2-C3 will initialize the pattern with different structures: C3 is a random pattern
  Fis3 (note 66) will copy current child pattern to the clipboard
  Cis3 (note 61) pasts the clipboard to parent and child and resets the patterns to start

  G3 (note 67) clears the clipboard
  Gis3 (note 68) adds a trigger to the current clipboard pattern position , and increases the pattern position
  A3 (note6 9) clears rgetrigger at  the current clipboard pattern position , and increases the pattern position
  At position 16 the step resets to position 1

  The functionality allows to create your own patterns (G3, (Gis3/A3) n times, and Cis3 to paste the pattern)

  NOTE: The Arduino output might be too low for some Eurorack Clock inputs of modules: some boosting helps here.
  If there is no clock the outputs are set LOW after <<interval>> milliseconds (default 10 secs)

  For connecting the Arduino to a computer (for using midi) : see http://projectgus.github.io/hairless-midiserial/

*/

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
int midiType;
int parentPin = 7;
int childPin = 8;
const long interval = 10000;
unsigned long currentMillis = millis();
unsigned long parentMillis = 0;
unsigned long childMillis = 0;
word parent;
word child;
word clipboard = 0;
int p_cnt = 0;
int c_cnt = 0;
int t_cnt = 0;
int ranbit = 0;
int ranloc = 0;
int life = 16;
boolean freeze = false;
// Pattern list
word pattern[] =
{
  1, 257, 4369, 20881, 37265, 21845, 62805, 65503, 1157, 16657, 33025, 48059
};

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(parentPin, OUTPUT);
  pinMode(childPin, OUTPUT);
  digitalWrite(parentPin, LOW);
  digitalWrite(childPin, LOW);
  randomSeed(analogRead(0));
  parent = 0;
  child = parent;
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(115200);
}

void loop()
{
  currentMillis = millis();
  // Read incoming messages
  if (MIDI.read())
  {
    if (MIDI.getType() == midi::Start)
    {
      digitalWrite(parentPin, LOW);
      digitalWrite(childPin, LOW);
      // Clear pattern at Midi start
      parent = 0;
      child = parent;
      p_cnt = 0;
      c_cnt = 0;
    }
    if (MIDI.getType() == midi::Stop)
    {
      digitalWrite(parentPin, LOW);
      digitalWrite(childPin, LOW);
    }
    if (MIDI.getType() == midi::NoteOn)
    {
      int note = int(MIDI.getData1());
      int vel = int(MIDI.getData2());
      if (note == 60 && vel > 0)
        // Set Random pattern
      {
        parent = word(random(1, 255));
        child = parent;
        p_cnt = 0;
        c_cnt = 0;
        digitalWrite(parentPin, LOW);
        digitalWrite(childPin, LOW);
      }
      if (note == 72 && vel > 0)
        // Handling of the 'clock'
      {
        if (bitRead(child, c_cnt) == 1)
        {
          digitalWrite(childPin, HIGH);
          childMillis = currentMillis + interval;
        }
        if (bitRead(parent, c_cnt) == 1)
        {
          digitalWrite(parentPin, HIGH);
          parentMillis = currentMillis + interval;
        }
        c_cnt = c_cnt + 1;
        if (c_cnt == 16)
          // After child cycle change a bit (maybe...)
        {
          p_cnt = p_cnt + 1;
          c_cnt = 0;
          ranbit = random(0, 2);
          ranloc = random(0, 16);
          if (random(0, 2) == 1)
            // 50% probability that the bit is from the parent
          {
            ranbit = bitRead(parent, ranloc);
          }
          bitWrite(child, ranloc, ranbit);
        }
        if (p_cnt == life)
          // after life child cycles the child becomes the parent, if not frozen
        {
          p_cnt = 0;
          if (! freeze) {
            parent = child;
          }
        }
      }
      if ((note >= 48 && note < 60) && vel > 0)
      {
        parent = pattern[(note - 48)];
        child = parent;
        p_cnt = 0;
        c_cnt = 0;
        digitalWrite(parentPin, LOW);
        digitalWrite(childPin, LOW);
      }

      if (note == 62 && vel > 0) {
        life = 4;
      }

      if (note == 63 && vel > 0) {
        life = 8;
      }
      if (note == 64 && vel > 0) {
        life = 16;
      }


      if (note == 65 && vel > 0) {
        freeze = !freeze;
      }

      if (note == 66 && vel > 0) {
        clipboard = child;
      }

      if (note == 61 && vel > 0) {
        p_cnt = 0;
        c_cnt = 0;
        parent = clipboard;
        child = clipboard;
      }

      if (note == 67 && vel > 0) {
        t_cnt = 0;
        clipboard = 0;
      }

      if (note == 68 && vel > 0) {
        bitSet(clipboard, t_cnt);
        if (t_cnt == 15) {
          t_cnt = 0;
        } else {
          t_cnt = t_cnt + 1;
        }
      }

      if (note == 69 && vel > 0) {
        bitClear(clipboard, t_cnt);
        if (t_cnt == 15) {
          t_cnt = 0;
        } else {
          t_cnt = t_cnt + 1;
        }
      }


    }


    if (MIDI.getType() == midi::NoteOff)
    {
      if (MIDI.getData1() == 60)
      {
      }
      if (MIDI.getData1() == 72)
      {
        digitalWrite(parentPin, LOW);
        digitalWrite(childPin, LOW);
      }
    }
    if (currentMillis - childMillis > interval)
    {
      digitalWrite(childPin, LOW);
      childMillis = currentMillis + interval;
    }
    if (currentMillis - parentMillis > interval)
    {
      digitalWrite(parentPin, LOW);
      parentMillis = currentMillis + interval;
    }
  }
}