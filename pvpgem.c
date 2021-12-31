#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <conio.h> /* console i/o */
#include <graph.h> /* graphics mode */

#include "msg.h"
#include "sound.h"
#include "vga.h" /* XRES and YRES */

/* the target score; first player to get this score wins */
#define MAXSCORE 25

/* the SIZE of the board is always a square */
#define SIZE 10

/* bit mask GEM (7=0111) only returns values 0=0000 to 7=0111 */
#define GEM 7

/* binary flag (1000) to indicate gem should be cleared */
#define CLR 8

/* row,col coordinate */
typedef struct {
  int row;
  int col;
} loc_t;

int
rand_gem(void)
{
  /* wrapper function to return a single random gem */
  /* make this a wrapper in case we change the number of gems later */
  return (rand() & GEM);
}

int
find_matches(int **board)
{
  int row, col;
  int matches;

  /* find horiz matches */

  for (row = 0; row < SIZE; row++) {
    for (col = 1; col < SIZE-1; col++) {
      if ( ((board[row][col-1] & GEM) == (board[row][col] & GEM)) &&
	   ((board[row][col+1] & GEM) == (board[row][col] & GEM)) ) {
	/* this is a match */
	/* set them all equal to each other */
	board[row][col] = board[row][col] | CLR;
	board[row][col-1] = board[row][col];
	board[row][col+1] = board[row][col];
      }
    }
  }

  /* find vert matches */

  for (row = 1; row < SIZE-1; row++) {
    for (col = 0; col < SIZE; col++) {
      if ( ((board[row-1][col] & GEM) == (board[row][col] & GEM)) &&
	   ((board[row+1][col] & GEM) == (board[row][col] & GEM)) ) {
	/* this is a match */
	/* set them all equal to each other */
	board[row][col] = board[row][col] | CLR;
	board[row-1][col] = board[row][col];
	board[row+1][col] = board[row][col];
      }
    }
  }

  /* count the matched gems */

  matches = 0;

  for (row = 0; row < SIZE; row++) {
    for (col = 0; col < SIZE; col++) {
      if (board[row][col] & CLR) {
	matches++;
      }
    }
  }

  /* done */

  return matches;
}

int
shuffle_down(int **board, int init_row, int col)
{
  int row;

  /* go "up" from this row,col and shuffle gems down by one row */
  for (row = init_row+1; row < SIZE; row++) {
    board[row-1][col] = board[row][col];
  }

  /* insert a new gem at the top */
  board[SIZE-1][col] = rand_gem();

  return (row - init_row); /* number of shuffles */
}

int
clear_matches(int **board)
{
  int row, col;
  int cleared = 0;

  /* move from "bottom" (row 0) to top and remove matched gems, and
     shuffle down from the top */

  for (row = 0; row < SIZE; row++) {
    for (col = 0; col < SIZE; col++) {
      while (board[row][col] & CLR) {
	/* this gem should be cleared .. shuffle down from the top */

	shuffle_down(board, row, col);
	cleared++;
      }
    } /* for col */
  } /* for row */

  return cleared;
}

void
show_score(int p1, int p2)
{
  char score[3];

  /* use graphics mode to update a special scoreboard */

  _setcolor(3); /* cyan */
  _rectangle(_GFILLINTERIOR, 0,10, XRES-1,20);

  /* player 1 score */

  sprintf(score, "%d", p1);

  _moveto(XRES/3, 12);
  _setcolor(1); /* blue */
  _outgtext(score);

  /* player 2 score */

  sprintf(score, "%d", p2);

  _moveto((XRES/3)*2, 11);
  _outgtext(score);
}

void
show_board(int **board)
{
  int row, col;
  char colors[8] = { 9, 10, 11, 12, 13, 14, 15, 6 };

  /* upper-left of board */
#define BOARDLEFT ( (XRES/2)-(BOARDRES/2) )
#define BOARDTOP 50
#define GEMSIZE (BOARDRES/SIZE)

  /* use graphics mode to print the gems on the screen */

  _setcolor(8); /* br black */

  /* 400px in 640x480, centered */
  _rectangle(_GFILLINTERIOR, BOARDLEFT,BOARDTOP,
      BOARDLEFT+BOARDRES,BOARDTOP+BOARDRES);

  for (row = 0; row < SIZE; row++) {
    for (col = 0; col < SIZE; col++) {
      /* "bottom" is row 0 = board[0][...] */
      /* "left" is col 0 = board[...][0] */

      _setcolor(colors[ board[row][col] & GEM ]);

      _ellipse(_GFILLINTERIOR,
	  BOARDLEFT+(GEMSIZE*col)+1,BOARDTOP+(GEMSIZE*(SIZE-1-row))+1,
	  BOARDLEFT+GEMSIZE+(GEMSIZE*col)-1,BOARDTOP+GEMSIZE+(GEMSIZE*(SIZE-1-row))-1 );
    }
  }
}

void
init_board(int **board)
{
  int row, col;
  int n;
  int prev;

  for (row = 0; row < SIZE; row++) {
    prev = -1; /* init to something not 0..7 */

    for (col = 0; col < SIZE; col++) {
      do {
	n = rand_gem();
      } while (n == prev);

      board[row][col] = n;
      prev = n;
    }
  }

  /* ensure there are no matches on the board */

  do {
    n = find_matches(board);

    if (n > 0) {
      clear_matches(board);
    }
  } while (n > 0);
}

loc_t
select_gem(void)
{
  int key;
  char rtarr[1] = { 0xc3 };
  char dnarr[1] = { 0xc2 };
  loc_t coord;

  /* use graphics mode to visually select a gem */

  /* print_message("Select a gem ..."); */

  coord.row = SIZE/2;
  coord.col = SIZE/2;

  /* loop while the user selects a gem */

  do {
    /* draw new selector wireframe */

    _setcolor(7); /* white */

    _rectangle(_GBORDER,
	BOARDLEFT+(GEMSIZE*coord.col),BOARDTOP+(GEMSIZE*(SIZE-1-coord.row)),
	BOARDLEFT+GEMSIZE+(GEMSIZE*coord.col),BOARDTOP+GEMSIZE+(GEMSIZE*(SIZE-1-coord.row)) );

    /* get up/down/left/right */

    key = getch();

    if (key == 0) {
      /* erase current selector wireframe */

      _setcolor(8); /* br black */

      _rectangle(_GBORDER,
	  BOARDLEFT+(GEMSIZE*coord.col),BOARDTOP+(GEMSIZE*(SIZE-1-coord.row)),
	  BOARDLEFT+GEMSIZE+(GEMSIZE*coord.col),BOARDTOP+GEMSIZE+(GEMSIZE*(SIZE-1-coord.row)) );

      /* read ext key */

      switch( getch() ) {
      case 72: /* up */
	coord.row++;
	break;
      case 80: /* down */
	coord.row--;
	break;
      case 75: /* left */
	coord.col--;
	break;
      case 77: /* right */
	coord.col++;
	break;
      }
    } /* if key */

    /* don't go outside the bounds */

    if (coord.row < 0) {
	sound_err();
	coord.row = 0;
    }
    if (coord.row >= SIZE) {
	sound_err();
	coord.row = SIZE-1;
    }
    if (coord.col < 0) {
	sound_err();
	coord.col = 0;
    }
    if (coord.col >= SIZE) {
	sound_err();
	coord.col = SIZE-1;
    }
  } while (key != 13); /* enter */

  /* return the coordinate */

  return coord;
}

int
play_game(int **board)
{
  int player;
  int score[2];
  int cleared;
  int sndlevel;
  loc_t gem;
  char prompt[25];

  /* alternate between 0 and 1 */

  player = 0;

  score[0] = 0;
  score[1] = 0;

  /* refresh screen */

  show_board(board);
  show_score(score[0], score[1]);

  do {
    sprintf(prompt, "Player %d: your move", player);
    print_message(prompt);

    /* select gem to kill */

    gem = select_gem();

    /* mark that gem to be cleared */

    board[gem.row][gem.col] = board[gem.row][gem.col] | CLR;

    /* update the board with find_matches and clear_matches */

    sndlevel = 0;

    do {
      find_matches(board);

      cleared = clear_matches(board);
      score[player] += cleared;

      if (cleared) {
	sound_play(sndlevel++);

	/* update screen again */

	show_board(board);
	show_score(score[0], score[1]);
      }
    } while (cleared);

    /* swap players 0 and 1 */

    player = (player ? 0 : 1);
  } while ((score[0] < MAXSCORE) && (score[1] < MAXSCORE));

  /* done */

  sound_win();

  /* oops, this comes back as the wrong player - swap them back */

  return (player ? 0 : 1);
}

int
main()
{
  int **board;
  int row;
  int winner;
  char winmsg[20];

  /* DOS does not have a built-in RNG, so we need to use srandom */
  srand((unsigned int) time(NULL));

  /* allocate the board */

  /* rows */
  board = (int **) malloc(sizeof(int **) * SIZE);

  if (board) {
    for (row = 0; row < SIZE; row++) {
      board[row] = (int *) malloc(sizeof (int) * SIZE);

      if (board[row] == NULL) {
	/* error! de-allocate previous memory and quit */
	fputs("Error allocating memory - abort\n", stderr);

	fputs("Done\n", stderr);
	return 1;
      }
    } /* for */
  } /* if (board) */

  /* initialize the board */

  _setvideomode(_VRES16COLOR); /* 640x480 @ 16 colors */

  print_title("PvP GEM");
  print_message("Generating a random board . . .");
  init_board(board);

  /* play game */

  winner = play_game(board);

  sprintf(winmsg, "Player %d wins!", winner);
  print_title(winmsg);

  /* not really an error, but print it in a diff color*/

  print_error("Press any key to end");
  if (getch() == 0) {
    getch(); /* clear the extended key */
  }

  _setvideomode(_DEFAULTMODE);

  /* done */

  for (row = 0; row < SIZE; row++) {
    free(board[row]);
  }

  puts("Ok");
  return 0;
}
