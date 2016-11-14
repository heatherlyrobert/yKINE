/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    <yLOG.h>                    /* heatherly program logger            */


/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>             /* clibc  standard input/output             */
#include    <stdlib.h>            /* clibc  standard general purpose          */
#include    <string.h>            /* clibc  standard string handling          */
#include    <math.h>              /* clibc  standard math functions           */




/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE_PRIV
#define yKINE_PRIV yes



/*===[[ UNIT TEST ]]======================================*/
#ifndef DEBUG_TOPS
#define DEBUG_TOPS   if (1 == 0)
#endif



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
   LOWR  = 17,  CALC  = 18,
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
   /*---(flags)--------------------------*/
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



/*---(setup)-----------------------------*/
char        yKINE__clear       (tSEG *a_curr, char *a_name, int a_leg, int a_seg, char a_type);
/*---(shared forward/inverse)------------*/
char        yKINE__thor        (int  a_num);
char        yKINE__coxa        (int  a_num);
char        yKINE__troc        (int  a_num);
/*---(forward kinematics)----------------*/
char        yKINE__FK_femu     (int  a_num, float a_deg);
char        yKINE__FK_pate     (int  a_num, float a_deg);
char        yKINE__FK_tibi     (int  a_num, float a_deg);
/*---(inverse kinematics)----------------*/
char        yKINE__target      (int  a_num, float a_x, float a_z, float a_y);
char        yKINE__IK_femu     (int  a_num);
char        yKINE__IK_pate     (int  a_num);
char        yKINE__IK_tibi     (int  a_num);
/*---(shared forward/inverse)------------*/
char        yKINE__meta        (int  a_num);
char        yKINE__tars        (int  a_num);
char        yKINE__foot        (int  a_num);
/*---(unit testing)----------------------*/
char        yKINE__setter      (char *a_request, int a_leg, int a_seg, double a_value);
char*       yKINE__getter      (char *a_question, int a_leg,  int a_seg);
char        yKINE__testquiet   (void);
char        yKINE__testloud    (void);
char        yKINE__testend     (void);



#endif
/*============================[[    end-code    ]]============================*/
