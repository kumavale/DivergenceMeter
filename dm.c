#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>

#include <math.h>

#include <sys/ioctl.h>

#include "aa.c"


int row = -1;
int col = -1;
int terminal_width  = 72;
int terminal_height = 14;


void finish() {
	printf("\033[?25h\033[0m");
	exit(0);
}

void SIGINT_handler(int sig){
	(void)sig;
	finish();
}

void SIGWINCH_handler(int sig) {
	(void)sig;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	terminal_width = w.ws_col;
	terminal_height = w.ws_row;

  col = (terminal_width - FRAME_WIDTH*8) / 2;
  row = (terminal_height - FRAME_HEIGHT) / 2;

	signal(SIGWINCH, SIGWINCH_handler);
}

void usage(char * argv[]) {
	printf(
			"Terminal DivergenceMeter\n"
			"\n"
			"usage: %s [-dhn]\n"
			"\n"
			" -d --divergence \033[3mDivergence Meter\033[0m\n"
			" -n --no-title   \033[3mDo not set the titlebar text\033[0m\n"
			//" -t --timer      \033[3mKitchen timer\033[0m\n"
			//" -s --stopwatch  \033[3mStopwatch\033[0m\n"
			" -h --help       \033[3mShow this help message.\033[0m\n",
			argv[0]);
}

int main(int argc, char **argv)
{
  time_t t, to;
  struct tm *n;
  char f[16] = {'\0'};
  const char *colors[128] = {NULL};
  int i, j;
  int c, r;
  const char *a;
  int set_title = 1;
  int dMeter = 0;
  int clock = 1;

	static struct option longopts[] = {
		{"divergence", no_argument, 0, 'd'},
		{"no-title",   no_argument, 0, 'n'},
		{"help",       no_argument, 0, 'h'},
		{0,0,0,0}
	};

  int o, idx;
  while( (o=getopt_long(argc, argv, "dhn", longopts, &idx)) != -1 )
  {
    if( !o ) if( longopts[idx].flag == 0) o=longopts[idx].val;
    switch(o)
    {
      case 'd':
        clock = 0;
        dMeter = 1;
        break;
      case 'h':
        usage(argv);
        exit(0);
        break;
      case 'n':
        set_title = 0;
        break;
    }
  }

	signal(SIGINT, SIGINT_handler);
  signal(SIGWINCH, SIGWINCH_handler);

  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  terminal_width = w.ws_col;
  terminal_height = w.ws_row;
  if(col < 0 || row < 0)
  {
    col = (terminal_width - FRAME_WIDTH*8) / 2;
    row = (terminal_height - FRAME_HEIGHT) / 2;
  }

  colors[','] = "\033[38;5;16;48;5;16m ";  /* Black background */
  colors['|'] = "\033[38;5;252;48;5;16m|";
  colors['-'] = "\033[38;5;252;48;5;16m-";
  colors['/'] = "\033[38;5;252;48;5;16m/";
  colors['='] = "\033[38;5;252;48;5;16m=";
  colors['^'] = "\033[38;5;130;48;5;16m|";
  colors['@'] = "\033[38;5;202;48;5;16m@";
  colors['+'] = "\033[38;5;226;48;5;16m+";
  colors['*'] = "\033[38;5;208;48;5;16m#";
  colors['1'] = "\033[38;5;215;48;5;208m#";
  colors['#'] = "\033[38;5;130;48;5;16m#";


  if( dMeter )
  {
    float r, s;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand(((unsigned int)time(NULL)+(unsigned int)ts.tv_nsec)/2);
    //printf("Calculating...\r\033[?25l");
    //fflush(stdout);
    //for(int _i=0; _i<65536; _i++)
    //  (void)rand(); // Do nothing
    r = (float)rand();
    s = (1.123581*r)/RAND_MAX;
    snprintf(f, 15, "%08.6f", s);
    if(s >= 1.0) printf("%c", 0x07);
  }

  if( set_title )
  {
    const char h[] = "( ﾟ∀゜)ﾌﾊﾊ八八ﾉヽﾉヽﾉヽﾉ ＼ / ＼/ ＼";
	  printf("\033k%s\033\134", h);
	  printf("\033]1;%s\007", h);
	  printf("\033]2;%s\007", h);
  }

  printf("\033[H\033[2J\033[?25l");
  setvbuf(stdout, NULL, _IOFBF, 32768);

  while(1)
  {
    t = time(NULL);
    if( t==to ) continue;
    to = t;

    if( clock )
    {
      t = time(NULL);
      n = localtime(&t);
      sprintf(f, "%02d.%02d.%02d", n->tm_hour, n->tm_min, n->tm_sec);
    }

    for(r=0; r<row; r++)
    {
      for(c=0; c<terminal_width; c++)
        printf("%s", colors[',']);
      printf("%c%c", 0x0d, 0x0a);
    }

    for(i=0; i<FRAME_HEIGHT; i++)
    {
      for(c=0; c<col; c++)
        printf("%s", colors[',']);
      for(j=0; j<(int)strlen(f); j++)
      {
        switch(f[j])
        {
          case '0': a=num0[i]; break;
          case '1': a=num1[i]; break;
          case '2': a=num2[i]; break;
          case '3': a=num3[i]; break;
          case '4': a=num4[i]; break;
          case '5': a=num5[i]; break;
          case '6': a=num6[i]; break;
          case '7': a=num7[i]; break;
          case '8': a=num8[i]; break;
          case '9': a=num9[i]; break;
          case '.': a=dot[i];  break;
          default:  continue;  break;
        }
        while(*a)printf("%s",colors[(int)*a++]);
      }
      for(c=0; c<col; c++)
        printf("%s", colors[',']);
      printf("%c%c", 0x0d, 0x0a);
    }

    for(r=0; r<row-1; r++)
    {
      for(c=0; c<terminal_width; c++)
        printf("%s", colors[',']);
      printf("%c%c", 0x0d, 0x0a);
    }
    printf("\033[%d;1H", terminal_height);
    fflush(stdout);

    if( dMeter ) finish();
    printf("\033[H\033[?25l");

    usleep(500000);
  }

  return 0;
}
