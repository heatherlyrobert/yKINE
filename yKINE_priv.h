/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"


/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include  <stdio.h>               /* clibc  standard input/output             */
#include  <stdlib.h>              /* clibc  standard general purpose          */
#include  <string.h>              /* clibc  standard string handling          */
#include  <unistd.h>              /* clibc  standard unix interface           */
#include  <error.h>               /* clibc  standard error handling           */
#include  <fcntl.h>               /* clibc  standard file control             */
#include  <termios.h>             /* clibc  standard terminal control         */
#include  <math.h>                /* clibc  standard math functions           */
#include  <signal.h>              /* clibc  standard signal handling          */
#include  <time.h>                /* clibc  standard time and date handling   */
#include  <ctype.h>               /* clibc  standard character classes        */

/*---(posix standard)--------------------*/
#include  <unistd.h>              /* POSIX  standard operating system API     */
#include  <sys/time.h>            /* POSIX  standard time access              */

/*---(X11 standard)----------------------*/
#include  <X11/X.h>               /* X11    standard overall file             */
#include  <X11/Xlib.h>            /* X11    standard C API                    */


/*---(opengl standard)-------------------*/
#include  <GL/gl.h>               /* opengl standard primary header           */
#include  <GL/glx.h>              /* opengl standard X11 integration          */

/*---(heatherly made)--------------------*/
#include  <yVAR.h>                /* heatherly variable testing               */
#include  <yFONT.h>               /* heatherly texture-mapped fonts           */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE_PRIV
#define yKINE_PRIV yes


#define   MAX_LEGS   20
enum      leg_nums {
   L_RF=0  , L_RM=1  , L_RR=2  ,
   L_LR=3  , L_LM=4  , L_LF=5  ,
};

#define   MAX_SEGS   20
enum      seg_nums {
   /*---(center position)----------------*/
   FOCU  =  0,  CORE  =  1,
   /*---(leg parts)----------------------*/
   THOR  =  2,  COXA  =  3,  TROC  =  4,
   FEMU  =  5,  PATE  =  6,  TIBI  =  7,
   META  =  8,  TARS  =  9,  FOOT  = 10,
   CLAW  = 11,  MAGN  = 12,  HOOK  = 13,
   /*---(working areas)------------------*/
   ORIG  = 14,  TARG  = 15,  LEGN  = 16,
   VERT  = 17,  CALC  = 18,
};



/*---(hexapod leg)-----------------------*/
typedef struct cSEG  tSEG;
struct cSEG {
   /*---(self-knowledge)-----------------*/
   int       leg;
   int       seg;
   /*---(descriptions)-------------------*/
   char      n         [12];      /* short name                               */
   float     l;                   /* joint length                             */
   float     min;                 /* min articulation                         */
   float     max;                 /* max articulation                         */
   float     mass;                /* mass of link                             */
   float     servo;               /* mass of servo                            */
   long      dlist;               /* display list                             */
   /*---(lengths)------------------------*/
   float     fl;                  /* full calculated length from center       */
   float     sl;                  /* full calculated length of segment        */
   /*---(angles)-------------------------*/
   float     d;                   /* joint angle                              */
   float     h, v;                /* leg segment orientation in radians       */
   float     cd;                  /* cum joint angle                          */
   float     ch, cv;              /* cum segment orientation in radians       */
   char      u;                   /* leg underneath body switch               */
   /*---(coordinates)--------------------*/
   float     x , y , z , xz;      /* segment location                         */
   float     cx, cy, cz, cxz;     /* cumulative location                      */
   /*---(flags)--------------------------*/
   char      c;                   /* changed/updated flag                     */
   char      m;                   /* mode (i=IK, f=FK)                        */
   char      p;                   /* planted y/n?                             */
   char      rc;                  /* kinematics status : 0 = no, 1 = yes      */
};
extern    tSEG      gk [MAX_LEGS] [MAX_SEGS];    /* opengl kinematics check   */
extern    tSEG      fk [MAX_LEGS] [MAX_SEGS];    /* forward kinematics        */
extern    tSEG      ik [MAX_LEGS] [MAX_SEGS];    /* inverse kinematics        */

char        yKINO_clear        (tSEG *a_curr, char *a_name, int a_leg, int a_seg);
/*---(shared forward/inverse)------------*/
char        yKINO__thor        (int  a_num);
char        yKINO__coxa        (int  a_num);
char        yKINO__troc        (int  a_num);
/*---(forward kinematics)----------------*/
char        yKINO__FK_femu     (int  a_num, float a_deg);
char        yKINO__FK_pate     (int  a_num, float a_deg);
char        yKINO__FK_tibi     (int  a_num, float a_deg);
/*---(shared forward/inverse)------------*/
char        yKINO__meta        (int  a_num);
char        yKINO__tars        (int  a_num);
char        yKINO__foot        (int  a_num);
/*---(inverse kinematics)----------------*/
char        yKINO__target      (int  a_num, float a_x, float a_z, float a_y);
char        yKINO__IK_femu     (int  a_num);


#endif
/*============================[[    end-code    ]]============================*/
