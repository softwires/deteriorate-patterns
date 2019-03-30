# deteriorate-patterns
Pseudo random pattern generator for Arduino/Eurorack

Variation on the classic repeating pattern / change random bit after looping the pattern.
  The program sends 2 patterns : parent and child. When initialized both 16 steps pattern ar the same. After 1 child cycle a randon position is eiher changed with a random value or the value of the same position of the parent : odds are 50%.
  After 16 child cycles the child becomes the parent.
  The output of the parent pulse is at pin 7, the output of the child at pin 8.
  The patterns are clocked by C4 (midi 72) notes. The initial parent pattern is a clear one (silence) (reset also at Midi Start).
  Length of the step is the note length : so this is more a gate pattern than a trigger generator. Timing of the steps depends on the timing of the input (so swing /micro timing are ' out of the box')
  Note ranges C2-C3 will initialize the pattern with different structures: C3 is a random pattern

  NOTE: The Arduino output might be too low for some Eurorack Clock inputs of modules: some boosting helps here.
  If there is no clock the outputs are set LOW after <<interval>> milliseconds (default 10 secs)

  For connecting the Arduino to a computer (for using midi) : see http://projectgus.github.io/hairless-midiserial/
