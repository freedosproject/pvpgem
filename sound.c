#include <i86.h> /* sound */

void
sound_play(int soundlvl)
{
#define FACTOR 1193180
#define OCTAVE 4

  int lvl;
  int notes[10] = {9121, 9664, 10238, 10847, 11492,
      12175, 12899, 13666, 14479, 15340};

  /* keep the sound level in the array bounds */

  lvl = (soundlvl < 0 ? 0 : soundlvl);
  lvl = (lvl < 10 ? lvl : 10);

  /* on DOS, this will make different beep sounds */

  sound( FACTOR / (notes[lvl] / (1<<OCTAVE)) );
  delay(200);
  nosound();
}

void
sound_win(void)
{
  /* play a winner tone */

  sound(700);
  delay(200);

  sound(900);
  delay(500);

  sound(800);
  delay(200);

  sound(1000);
  delay(1000);

  nosound();
}

void
sound_err(void)
{
  /* play a double beep */

  sound(100);
  delay(10);
  nosound();

  delay(10);

  sound(100);
  delay(10);
  nosound();
}
