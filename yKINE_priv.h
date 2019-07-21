/*============================----beg-of-source---============================*/


/*===[[ HEADER ]]=============================================================*/

/*   focus         : (RO) robotics
 *   niche         : (sp) spiders
 *   application   : yKINE
 *   purpose       : shared, consistent, and reliable spider kinematics/scipting
 *
 *   base_system   : gnu/linux   (powerful, ubiquitous, technical, and hackable)
 *   lang_name     : ansi-c      (wicked, limitless, universal, and everlasting)
 *   dependencies  : none
 *   size          : small       (less than 2000 slocL)
 * 
 *   author        : rsheatherly
 *   created       : 2009-07     (about when i bought eva and igor)
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable)
 * 
 */
/*===[[ SUMMARY ]]============================================================*/

/*   yKINE is a consistent, shared, full-featured, and reliable spider
 *   kinematics library designed around the lynxmotion CH3-R hexapod format
 *   which will free all other application from detailed calculations.
 *
 */

/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/

/*===[[ ONE_LINERS ]]=========================================================*/

#define     P_FOCUS     "RO (robotics)"
#define     P_NICHE     "hx (hexapoda)"
#define     P_PURPOSE   "shared, consistent, and reliable spider kinematics/scripting"

#define     P_NAMESAKE  "typhoeus-terrigena (earth-born)"
#define     P_HERITAGE  "most fearsome greek monster, last son of gaia and tartarus"
#define     P_IMAGERY   "winged, one-hundred dragon heads, legions of viper coil tentacles"
#define     P_REASON    "king of monsters to synchronize hundreds of limbs, sensors, and devices"

#define     P_SYSTEM    "gnu/linux   (powerful, ubiquitous, technical, and hackable)"
#define     P_LANGUAGE  "ansi-c      (wicked, limitless, universal, and everlasting)"
#define     P_CODESIZE  "small       (appoximately 10,000 slocl)"

#define     P_AUTHOR    "heatherlyrobert"
#define     P_CREATED   "2009-07"
#define     P_DEPENDS   "none"

#define     P_VERMAJOR  "1.--, working and advancing"
#define     P_VERMINOR  "1.1-, implement stances and enabling new leg verbs"
#define     P_VERNUM    "1.0d"
#define     P_VERTXT    "rk_pure, rk_from are reasonably unit tested and ready to rock"

#define     P_PRIORITY  "direct, simple, brief, vigorous, and lucid (h.w. fowler)"
#define     P_PRINCIPAL "[grow a set] and build your wings on the way down (r. bradbury)"
#define     P_REMINDER  "there are many better options, but i *own* every byte of this one"

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/
/*===[[ END_HEADER ]]=========================================================*/


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
#include    <yPARSE.h>       /* CUSTOM  heatherly record parsing/queuing      */
#include    <yLOG.h>         /* CUSTOM  heatherly program logging             */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE_PRIV
#define yKINE_PRIV yes



/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
/*> #define     YKINE_VER_NUM   "1.0s"                                                <*/
/*> #define     YKINE_VER_TXT   "updated to full unit test pass after yPARSE modified (doubles)"   <*/



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


extern const    float    DEG2RAD;
extern const    float    RAD2DEG;

extern char ykine__unit_answer [ LEN_STR  ];


typedef struct cLOCAL tLOCAL;
struct cLOCAL {
   /*---(overall)-----------*/
   char        unit;
   int         logger;
   float       scrp_len;
   /*---(script)------------*/
   char        s_name      [LEN_RECD];      /* script file name               */
   FILE       *s_file;                      /* script file handle             */
   char        s_orig      [LEN_RECD];      /* original line read             */
   char        s_recd      [LEN_RECD];      /* working record                 */
   int         s_len;                       /* original record length         */
   char        s_q         [LEN_LABEL];     /* record parsing delimiter       */
   char       *s_context;                   /* record parsing context         */
   int         s_nline;                     /* source file count of lines     */
   int         s_cline;                     /* source file current line       */
   /*---(parsing)-----------*/
   char        s_verb      [LEN_LABEL];     /* script line verb               */
   char        s_hidden;                    /* a hidden action/move           */
   int         s_iverb;                     /* index of verb                  */
   char        s_servo;                     /* verb needs a servo specifier   */
   char        s_targ;                      /* leg/seg targeting override     */
   char        s_from;                      /* pure vs from                   */
   int         s_count;
   float       s_beats;
   float       s_secs;
   float       s_femu;
   float       s_pate;
   float       s_tibi;
   float       s_xpos;
   float       s_zpos;
   float       s_ypos;
   float       s_1st;
   float       s_2nd;
   /*---(body)--------------*/
   float       s_xcenter;
   float       s_zcenter;
   float       s_ycenter;
   float       s_yaw;
   float       s_roll;
   float       s_pitch;
   /*---(done)--------------*/
};
tLOCAL      myKINE;



/*===[[ TYPEDEFS ]]===========================================================*/
typedef     struct      cMOVE       tMOVE;
typedef     struct      cSERVO      tSERVO;




struct      cMOVE {
   /*---(keys)--------------*/
   int         seq;
   char        type;
   tSERVO     *servo;
   char        label       [LEN_LABEL];
   int         line;
   /*---(timing)------------*/
   float       dur;
   float       secs;
   int         other;
   /*---(angle)-------------*/
   float       degs;
   /*---(position)----------*/
   float       x_pos;
   float       z_pos;
   float       y_pos;
   float       xz_len;
   /*---(linked-lists)------*/
   tMOVE      *m_prev;
   tMOVE      *m_next;
   tMOVE      *s_prev;
   tMOVE      *s_next;
   /*---(done)--------------*/
};
extern      tMOVE      *m_head;
extern      tMOVE      *m_tail;
extern      int         m_count;

struct cSERVO {
   /*---(overall)------------------------*/
   const char  label       [20];
   const char  leg;                    /* specific identifier of leg          */
   const char  seg;                    /* specific identifier of segment      */
   /*---(current)------------------------*/
   float       pace;                   /* secs per beat                       */
   char        exact;                  /* servo move starts                   */
   tMOVE      *curr;                   /* current move pointer                */
   float       deg;                    /* servo degree                        */
   float       xexp;                   /* expected x-pos                      */
   float       zexp;                   /* expected z-pos                      */
   float       yexp;                   /* expected y-pos                      */
   /*---(repeats)------------------------*/
   char        nsegno;                 /* number of segnos                    */
   tMOVE      *segni       [10];       /* saved segnos in stack               */
   /*---(scripting)----------------------*/
   char        scrp;
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
   char        abbr;
   char        full        [25];
   char        two         [ 5];
   char        caps        [ 5];
   float       deg;
};
extern tLEGDATA    g_leg_data [YKINE_MAX_LEGS];


typedef struct cSEGDATA tSEGDATA;
struct cSEGDATA {
   char        seg;
   char        abbr;
   char        full        [25];            /* descriptive name               */
   char        four        [ 5];            /* abbreviated name               */
   char        caps        [ 5];            /* capitalized name for titles    */
   char        move;                        /* have a servo/moveable (y/n)    */
   float       len;                         /* length                         */
   float       min;                         /* min degrees for segment        */
   float       attn;                        /* attn degress for segment       */
   float       max;                         /* max degrees for segment        */
   float       test1;                       /* test length (1) original       */
   float       test2;                       /* test length (2) newer          */
   float       test3;                       /* test length (3)                */
   char        neg         [LEN_LABEL];     /* direction of negative degrees  */
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
float       yKINE__femu_path   (int  a_leg, float a_deg, int a_meth);
char        yKINE__femu        (int  a_num, float a_deg, int a_meth);
char        yKINE__pate        (int  a_num, float a_deg, int a_meth);
char        yKINE__tibi        (int  a_num, float a_deg, int a_meth);
char        yKINE__lowr        (int  a_num, int    a_meth);
char        yKINE__FK_targ     (int  a_num, int    a_meth);
/*---(inverse kinematics)----------------*/
char        yKINE__IK_targ     (int  a_num, float a_x, float a_z, float a_y);
char        yKINE__IK_femu     (int  a_num);
char        yKINE__IK_pate     (int  a_num);
char        yKINE__IK_tibi     (int  a_num);
/*---(shared forward/inverse)------------*/
char        yKINE__meta        (int  a_num);
char        yKINE__tars        (int  a_num);
char        yKINE__foot        (int  a_num, int    a_meth);
/*---(unit testing)----------------------*/
char        yKINE__setter      (char *a_request , int a_leg, int a_seg, float a_value);
char*       yKINE__getter      (char *a_question, int a_leg, int a_seg);
char        yKINE__unit_quiet  (void);
char        yKINE__unit_loud   (void);
char        yKINE__unit_end    (void);


char        ykine_verb_init         (void);

char        ykine_scrp_begin        (void);
char        ykine__scrp_prep        (void);
char*       ykine__unit_scrp        (char *a_question, int a_num);
char        ykine_scrp_verb         (char *a_char);

char        ykine_scrp_zero         (void);
char        ykine_scrp_zpolar       (void);
char        ykine_scrp_orient       (void);
char        ykine_scrp_opolar       (void);

char        ykine_legs_fk           (void);
char        ykine_legs_ik           (void);
char        ykine_legs_tk           (void);
char        ykine_legs_rk           (void);
char        ykine_legs_ck           (void);
char        ykine_scrp_segno        (void);
char        ykine_scrp_repeat       (void);
char        ykine_scrp_exec         (void);


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


char        ykine_move_create       (char a_type, tSERVO *a_servo, char *a_label, int a_line, float a_deg, float a_sec);
char        ykine_move_addloc       (tSERVO *a_servo, float a_xpos, float a_zpos, float a_ypos);
char        ykine_move_repeat       (tSERVO *a_servo, int a_times);
char        ykine_move_delete       (tMOVE *a_move);
char        ykine_move_clear_servo  (tSERVO *a_servo);
char        ykine_move_savedloc     (tSERVO *a_servo, float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz);
char        ykine_move_savedprev    (tMOVE  *a_move , float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz);
char*       ykine__unit_move        (char *a_question, int a_leg, int a_seg, int a_move);

char        ykine__exact_find       (tSERVO *a_servo, float a_sec);
char        ykine__exact_data       (tSERVO *a_servo, float a_sec);

char        ykine_gait_beg          (void);
char        ykine_gait_end          (void);

char        ykine_scrp_walk         (int   a_repeats);
char        ykine_scrp_turn         (int   a_repeats);



/*===[[ YKINE_queue.c ]]======================================================*/
/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
char        ykine_scrp_popverb      (void);
char        ykine_scrp_popservo     (void);
char        ykine_servo_list        (char *a_which);



/*===[[ YKINE_turtle.c ]]=====================================================*/
/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
char        ykine__turtle_last      (void);
char        ykine_turtle_speed      (void);
char        ykine_turtle_wait       (void);
char        ykine_turtle_home       (void);
char        ykine_turtle_move       (void);
char        ykine_turtle_goto       (void);
char        ykine_turtle_head       (void);
char        ykine_turtle_turn       (void);
char        ykine_turtle_raise      (void);
char        ykine_turtle_lower      (void);
char        ykine_turtle_depth      (void);
char*       ykine__unit_turtle      (char *a_question, int a_num);



#endif
/*============================[[    end-code    ]]============================*/
