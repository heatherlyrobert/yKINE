/*============================----beg-of-source---============================*/

/*===[[ HEADERS ]]========================================*/
/*---(program)---------------------------*/
#include    "yKINE.h"

/*---(ansi-c standard)-------------------*/
#include    <stdio.h>        /* CLIBC   standard input/output                 */
#include    <stdlib.h>       /* CLIBC   standard general purpose              */
#include    <string.h>       /* CLIBC   standard string handling              */
#include    <math.h>         /* CLIBC   standard math functions               */

/*---(heatherly made)--------------------*/
#include    <yURG.h>         /* CUSTOM  heatherly urgent processing           */
#include    <ySTR.h>         /* CUSTOM  heatherly string handling             */
#include    <yLOG.h>         /* CUSTOM  heatherly program logging             */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE_PRIV
#define yKINE_PRIV yes



/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define     YKINE_VER_NUM   "0.9l"
#define     YKINE_VER_TXT   "reading zero body verbs in scripting working and unit tested"



/*===[[ UNIT TEST ]]======================================*/
#ifndef DEBUG_TOPS
#define     DEBUG_TOPS     if (myKINE.unit  == 'y')
#endif



/*===[[ RATIONAL LIMITS ]]====================================================*/
/*   LEN_ is a length or size of something
 *   MIN_ is a minimum count
 *   DEF_ is a default count
 *   MAX_ is a maximum count
 *
 */
/*---(string length)------------------*/
#define     LEN_LABEL   20
#define     LEN_STR     200
#define     LEN_RECD    2000


extern const    double   DEG2RAD;
extern const    double   RAD2DEG;

extern char ykine__unit_answer [ LEN_STR  ];


typedef struct cLOCAL tLOCAL;
struct cLOCAL {
   /*---(overall)-----------*/
   char        unit;
   int         logger;
   double      scrp_len;
   /*---(script)------------*/
   char        s_name      [LEN_RECD];      /* script file name               */
   FILE       *s_file;                      /* script file handle             */
   char        s_recd      [LEN_RECD];      /* record                         */
   int         s_len;                       /* original record length         */
   char        s_q         [LEN_LABEL];     /* record parsing delimiter       */
   char       *s_context;                   /* record parsing context         */
   int         s_lines;                     /* source file line               */
   char        s_verb      [LEN_LABEL];     /* script line verb               */
   int         s_iverb;                     /* index of verb                  */
   char        s_targ;                      /* leg/seg targeting override     */
   char        s_type;                      /* value intrepretation           */
   char        s_from;                      /* pure vs from                   */
   char        s_vers;                      /* script line version            */
   int         s_count;
   double      s_secs;
   double      s_femu;
   double      s_pate;
   double      s_tibi;
   double      s_xpos;
   double      s_zpos;
   double      s_ypos;
   /*---(body)--------------*/
   float       s_xcenter;
   float       s_zcenter;
   float       s_ycenter;
   float       s_yaw;
   float       s_rotate;
   float       s_pitch;
   /*---(done)--------------*/
};
tLOCAL      myKINE;



/*===[[ TYPEDEFS ]]===========================================================*/
typedef     struct      cMOVE       tMOVE;
typedef     struct      cSERVO      tSERVO;



struct      cMOVE {
   int         seq;
   char        type;
   tSERVO     *servo;
   char        label       [LEN_LABEL];
   int         line;
   double      sec_dur;
   double      deg_beg;
   double      deg_end;
   double      sec_beg;
   double      sec_end;
   double      x_pos;
   double      z_pos;
   double      y_pos;
   char        status;
   tMOVE      *m_prev;
   tMOVE      *m_next;
   tMOVE      *s_prev;
   tMOVE      *s_next;
};
extern      tMOVE      *m_head;
extern      tMOVE      *m_tail;
extern      int         m_count;

struct cSERVO {
   /*---(overall)------------------------*/
   char        label       [20];
   int         leg;                    /* specific identifier of leg          */
   int         seg;                    /* specific identifier of segment      */
   /*---(current)------------------------*/
   char        exact;                  /* servo move starts                   */
   tMOVE      *curr;                   /* current move pointer                */
   double      deg;                    /* servo degree                        */
   double      xexp;                   /* expected x-pos                      */
   double      zexp;                   /* expected z-pos                      */
   double      yexp;                   /* expected y-pos                      */
   /*---(reapeats)-----------------------*/
   char        segno_flag;
   tMOVE      *segno;
   char        coda_flag;
   tMOVE      *coda;
   char        scrp;
   /*---(saved)--------------------------*/
   char        saved;                  /* saved flag (y/n)                    */
   double      xsave;                  /* saved x-pos                         */
   double      zsave;                  /* saved z-pos                         */
   double      ysave;                  /* saved y-pos                         */
   double      dsave;                  /* saved degree                        */
   /*---(moves)--------------------------*/
   int         count;
   tMOVE      *head;
   tMOVE      *tail;
   /*---(done)---------------------------*/
};
extern      tSERVO      g_servo_info    [YKINE_MAX_SERVO];
extern      int         g_nservo;


typedef struct cLEGDATA tLEGDATA;
struct cLEGDATA
{
   char        leg;
   char        full        [25];
   char        two         [ 5];
   char        caps        [ 5];
   double      deg;
};
extern tLEGDATA    g_leg_data [YKINE_MAX_LEGS];


typedef struct cSEGDATA tSEGDATA;
struct cSEGDATA {
   char        seg;
   char        full        [25];            /* descriptive name               */
   char        four        [ 5];            /* abbreviated name               */
   char        caps        [ 5];            /* capitalized name for titles    */
   char        move;                        /* have a servo/moveable (y/n)    */
   double      len;                         /* length                         */
   double      min;                         /* min degrees for segment        */
   double      attn;                        /* attn degress for segment       */
   double      max;                         /* max degrees for segment        */
   double      test1;                       /* test length (1) original       */
   double      test2;                       /* test length (2) newer          */
   double      test3;                       /* test length (3)                */
};
extern tSEGDATA    g_seg_data [YKINE_MAX_SEGS];


/*---(hexapod leg)-----------------------*/
typedef struct cSEG  tSEG;
struct cSEG {
   /*---(self-knowledge)-----------------*/
   int       leg;
   int       seg;
   /*---(descriptions)-------------------*/
   char      n         [12];      /* short name                               */
   double    l;                   /* joint length                             */
   double    min;                 /* min articulation                         */
   double    max;                 /* max articulation                         */
   double    mass;                /* mass of link                             */
   double    servo;               /* mass of servo                            */
   long      dlist;               /* display list                             */
   /*---(lengths)------------------------*/
   double    fl;                  /* full calculated length from center       */
   double    sl;                  /* full calculated length of segment        */
   /*---(angles)-------------------------*/
   double    d;                   /* joint angle                              */
   double    h, v;                /* leg segment orientation in radians       */
   double    cd;                  /* cum joint angle                          */
   double    ch, cv;              /* cum segment orientation in radians       */
   /*---(flags)--------------------------*/
   char      u;                   /* leg underneath body switch               */
   /*---(coordinates)--------------------*/
   double    x , y , z , xz;      /* segment location                         */
   double    cx, cy, cz, cxz;     /* cumulative location                      */
   /*---(flags)--------------------------*/
   char      c;                   /* changed/updated flag                     */
   char      m;                   /* mode (i=IK, f=FK)                        */
   char      p;                   /* planted y/n?                             */
   char      rc;                  /* kinematics status : r = no, 1 = yes      */
};
extern    tSEG      gk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* opengl kinematics check   */
extern    tSEG      fk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* forward kinematics        */
extern    tSEG      ik [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* inverse kinematics        */



/*---(setup)-----------------------------*/
char        yKINE__clear       (tSEG *a_curr, char *a_name, int a_leg, int a_seg, char a_type);
/*---(shared forward/inverse)------------*/
char        yKINE__thor        (int  a_num);
char        yKINE__coxa        (int  a_num);
char        yKINE__troc        (int  a_num);
/*---(forward kinematics)----------------*/
double      yKINE__femu_path   (int  a_leg, double a_deg, int a_meth);
char        yKINE__femu        (int  a_num, double a_deg, int a_meth);
char        yKINE__pate        (int  a_num, double a_deg, int a_meth);
char        yKINE__tibi        (int  a_num, double a_deg, int a_meth);
char        yKINE__lowr        (int  a_num, int    a_meth);
char        yKINE__FK_targ     (int  a_num, int    a_meth);
/*---(inverse kinematics)----------------*/
char        yKINE__IK_targ     (int  a_num, double a_x, double a_z, double a_y);
char        yKINE__IK_femu     (int  a_num);
char        yKINE__IK_pate     (int  a_num);
char        yKINE__IK_tibi     (int  a_num);
/*---(shared forward/inverse)------------*/
char        yKINE__meta        (int  a_num);
char        yKINE__tars        (int  a_num);
char        yKINE__foot        (int  a_num);
/*---(unit testing)----------------------*/
char        yKINE__setter      (char *a_request , int a_leg, int a_seg, double a_value);
char*       yKINE__getter      (char *a_question, int a_leg, int a_seg);
char        yKINE__unit_quiet  (void);
char        yKINE__unit_loud   (void);
char        yKINE__unit_end    (void);



char        ykine_scrp_init         (void);
char        ykine__scrp_prep        (void);
char*       ykine__unit_scrp        (char *a_question, int a_num);

char        ykine_scrp_zero         (void);
char        ykine_scrp_orient       (void);
char        ykine_scrp_ik_pure      (void);
char        ykine_scrp_ik_from      (void);

char        ykine_scrp_ik           (void);
char        ykine_scrp_fk           (void);
char        ykine_scrp_repeat       (void);


char        ykine_servo_purge       (void);
char        ykine_servo_init        (void);
char        ykine_servo_prep        (void);
int         ykine_servo_find        (int   a_leg, int a_seg);
tSERVO*     ykine_servo_pointer     (int   a_leg, int a_seg);
char        ykine_servo_unfocused   (int   a_leg, int a_seg);
char        ykine_servo_side        (char  a_char);
char        ykine_servo_rank        (char  a_char);
char        ykine_servo_segment     (char *a_char);
char        ykine_servo_one         (char *a_source);
char        ykine_servos            (char *a_source);
char*       ykine__unit_servo       (char *a_question);


char        ykine_move_delete       (tMOVE *a_move);
char        ykine_move_clear_servo  (tSERVO *a_servo);
char*       ykine__unit_move        (char *a_question, int a_leg, int a_seg, int a_move);



char        ykine_parse_read        (void);
char        ykine_parse_prep        (char *a_verb);
char        ykine_parse_fields      (void);
char        ykine_parse_check       (void);
char        ykine_parse             (void);

char        ykine_gait_begin        (char  a_count);
char        ykine_gait_update       (char  a_count);

char        ykine_gait_06_beg       (void);
char        ykine_gait_06_end       (void);
char        ykine_gait_12_beg       (void);
char        ykine_gait_12_end       (void);

char        ykine_scrp_walk         (int   a_repeats);
char        ykine_scrp_turn         (int   a_repeats);



#endif
/*============================[[    end-code    ]]============================*/
