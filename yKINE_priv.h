/*============================----beg-of-source---============================*/

/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/

/*===[[ ONE_LINERS ]]=========================================================*/

#define     P_FOCUS     "RO (robotics)"
#define     P_NICHE     "hx (hexapoda)"
#define     P_SUBJECT   "wicked spider kinematics"
#define     P_PURPOSE   "shared, consistent, and reliable spider kinematics/scripting"

#define     P_NAMESAKE  "typhoeus-terrigena (earth-born)"
#define     P_HERITAGE  "most fearsome greek monster, last son of gaia and tartarus"
#define     P_IMAGERY   "winged, one-hundred dragon heads, legions of viper coil tentacles"
#define     P_REASON    "king of monsters to synchronize myriad limbs, sensors, and devices"

#define     P_ONELINE   P_NAMESAKE " " P_SUBJECT

#define     P_BASENAME  "libyKINE.so"
#define     P_FULLPATH  "/usr/local/lib64/libyKINE.so"
#define     P_SUFFIX    "arac"
#define     P_CONTENT   "spider movement script"

#define     P_SYSTEM    "gnu/linux   (powerful, ubiquitous, technical, and hackable)"
#define     P_LANGUAGE  "ansi-c      (wicked, limitless, universal, and everlasting)"
#define     P_CODESIZE  "small       (appoximately 10,000 slocl)"
#define     P_DEPENDS   "none"

#define     P_AUTHOR    "heatherlyrobert"
#define     P_CREATED   "2009-07"

#define     P_VERMAJOR  "1.--, working and advancing"
#define     P_VERMINOR  "1.3-, prepare basics for demonstration"
#define     P_VERNUM    "1.3h"
#define     P_VERTXT    "turtle back and working nicely ;)"

#define     P_PRIORITY  "direct, simple, brief, vigorous, and lucid (h.w. fowler)"
#define     P_PRINCIPAL "[grow a set] and build your wings on the way down (r. bradbury)"
#define     P_REMINDER  "there are many better options, but i *own* every byte of this one"

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/
/*===[[ END_HEADER ]]=========================================================*/


/*
 *
 *  metis § ----- § update project report with stats, header, and header fields
 *
 *  metis § ----- § file report with names and statistics
 *
 *  metis § ----- § prototype data collection and updates
 *
 *  metis § ----- § function header update logic
 *
 *  metis § ----- § file scraping c-std and ylib files for functions/macros
 *
 *
 *
 *
 */

/*===[[ SUMMARY ]]============================================================*/

/*   yKINE is a consistent, shared, full-featured, and reliable spider
 *   kinematics library designed around the lynxmotion CH3-R hexapod format
 *   which will free all other application from detailed calculations.
 *
 */



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
#include    <yKEYS.h>        /* CUSTOM  keeps progress data                   */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE_PRIV
#define yKINE_PRIV yes



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


extern const    float    DEG2RAD;
extern const    float    RAD2DEG;

extern char ykine__unit_answer [LEN_RECD];


typedef struct cLOCAL tLOCAL;
struct cLOCAL {
   /*---(overall)-----------*/
   char        unit;
   int         logger;
   float       scrp_len;
   /*---(script)------------*/
   char        s_pass;                      /* file reading pass              */
   char        s_name      [LEN_RECD];      /* script file name               */
   FILE       *s_file;                      /* script file handle             */
   char        s_orig      [LEN_RECD];      /* original line read             */
   char        s_recd      [LEN_RECD];      /* working record                 */
   int         s_len;                       /* original record length         */
   char        s_q         [LEN_LABEL];     /* record parsing delimiter       */
   char       *s_context;                   /* record parsing context         */
   int         s_tline;                     /* source line position in file   */
   int         s_nline;                     /* source lines accepted          */
   int         s_cline;                     /* current line                   */
   /*---(parsing)-----------*/
   char        s_verb      [LEN_LABEL];     /* script line verb               */
   char        s_hidden;                    /* a hidden action/move           */
   int         s_iverb;                     /* index of verb                  */
   char        s_style;                     /* linear vs polar                */
   char        s_servo;                     /* verb needs a servo specifier   */
   char        s_targ;                      /* leg/seg targeting override     */
   char        s_from;                      /* pure vs from                   */
   int         s_count;
   float       s_pace;                      /* global pace                    */
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
   /*---(moves)-------------*/
   char        leg;                         /* current leg for move           */
   char        exact;                       /* current move fully completed   */
   char        label       [LEN_LABEL];     /* current move label             */
   char        part;                        /* current move cell note         */
   char        cell;                        /* current move cell note         */
   char        servos      [LEN_HUND];      /* current servo focus list       */
   char        vb, ov, rcc;
   float       lxz;
   float       pct;
   int         step_s;
   float       step_h;
   char        step        [LEN_LABEL];     /* stepping specification         */
   int         seq;
   float       off;
   float       step_total;
   /*---(done)--------------*/
};
tLOCAL      myKINE;
#define   YKINE_PACE  0.10



/*===[[ CALCULATION STRUCTURES ]]=============================================*/
/*---(body zero-point/orient)---------*/
typedef struct cCENTER tCENTER;
struct   cCENTER {
   float       height;
   float       xpos , zpos , ypos;
   float       yaw  , pitch, roll;
};
tCENTER g_center;

/*---(move beg, cur,and end)----------*/
typedef struct cWORK tWORK;
struct   cWORK {
   /*---(common)------------*/
   float       sec;
   char        rc;
   /*---(segments)----------*/
   float       cd;                     /* coxa                                */
   float       fd, fx, fz, fy;         /* femu                                */
   float       pd, px, pz, py;         /* pate                                */
   float       td, tx, tz, ty;         /* tibi                                */
   float       ed, ex, ez, ey;         /* end/foot                            */
   /*---(working)-----------*/
   float       deg, out, len, xz;
   /*---(done)--------------*/
};
tWORK   g_beg;           /* move before                                       */
tWORK   g_end;           /* expected endpoint                                 */
tWORK   g_cur;           /* partial move calculation between g_beg/g_end      */
tWORK   g_sav;           /* helper for steps and gaits                        */
tWORK   g_pure;          /* result of kinematics calculation                  */
tWORK   g_adapt;         /* result of kinematics WITH body zero and orient    */



/*===[[ TYPEDEFS ]]===========================================================*/
typedef     struct      cMOVE       tMOVE;
typedef     struct      cSERVO      tSERVO;




struct      cMOVE {
   /*---(keys)--------------*/
   int         seq;                         /* natural order of creation      */
   char        type;                        /* type of move -- servo, note, ..*/
   char        verb;                        /* input verb                     */
   tSERVO     *servo;                       /* servo pointer for move         */
   int         line;                        /* line from input file           */
   char        label       [LEN_LABEL];     /* label from input line          */
   char        part;                        /* for grouping into accels       */
   char        cell;                        /* for grouping into cells        */
   /*---(timing)------------*/
   float       dur;                         /* duration of move               */
   float       secs;                        /* end time of move               */
   int         other;                       /* ¢ identify repeat moves ?      */
   /*---(pure move)---------*/
   char        pure_rc;                     /* kinematics return code         */
   float       pure_d;                      /* end servo degrees              */
   float       pure_x;                      /* end x position                 */
   float       pure_z;                      /* end z position                 */
   float       pure_y;                      /* end y position                 */
   float       pure_xz;                     /* end xz length from center      */
   /*---(adapted)-----------*/
   char        adapt_rc;                    /* kinematics return code         */
   float       adapt_d;                     /* end servo degrees              */
   float       adapt_x;                     /* end x position                 */
   float       adapt_z;                     /* end z position                 */
   float       adapt_y;                     /* end y position                 */
   float       adapt_xz;                    /* end xz length from center      */
   /*---(linked-lists)------*/
   tMOVE      *m_prev;                      /* prev in all moves list         */
   tMOVE      *m_next;                      /* next in all moves list         */
   tMOVE      *s_prev;                      /* prev move for servo            */
   tMOVE      *s_next;                      /* next move for servo            */
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
   float     fh, fv;              /* full orientations in radians             */
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
extern    tSEG      fk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* forward kinematics        */
extern    tSEG      ik [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* inverse kinematics        */
extern    tSEG      gk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* opengl kinematics check   */
extern    tSEG      pk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* pure endpoint             */


typedef struct cTIMING tTIMING;
struct cTIMING {
   /*---(original)----------*/
   char        request     [LEN_LABEL];
   /*---(as parsed)---------*/
   char        accel;
   char        decel;
   char        speed;
   float       step;
   /*---(indexes)-----------*/
   char        acceln;
   char        deceln;
   char        speedn;
   /*---(global)------------*/
   double      beats;
   float       exact;
   char        cpart;
   /*---(working)-----------*/
   char        a_acceln;
   char        a_deceln;
   char        a_speedn;
   float       a_exact;
   char        a_raise     [LEN_LABEL];     /* raise acceleration             */
   char        a_middle    [LEN_LABEL];     /* middle of move acceleration    */
   char        a_plant     [LEN_LABEL];     /* plant acceleration             */
   char        a_body      [LEN_LABEL];     /* body acceleration              */
   /*---(done)--------------*/
};
extern tTIMING  g_timing;

#define     MAX_ACCEL         10
#define     MAX_PARTS         10

#define     ACCEL_TURTLE       0
#define     ACCEL_SLOW         1
#define     ACCEL_MOD          2
#define     ACCEL_FAST         3
#define     ACCEL_EXTRA        4
#define     DECEL_FAST         5
#define     DECEL_MOD          6
#define     DECEL_SLOW         7
#define     DECEL_TURTLE       8
#define     DECEL_NOOP         9

typedef struct cENDS tENDS;
struct cENDS {
   char        verb;
   char        part;
   float       sb, se;
   float       xb, xe;
   float       zb, ze;
   float       yb, ye;
   float       xzlen, len;
   float       fb, fe;
   float       pb, pe;
   float       tb, te;
};
extern tENDS  g_accel_ends [MAX_PARTS];

typedef struct cACCEL tACCEL;
struct cACCEL {
   char        abbr;
   char        label    [LEN_LABEL];
   float       persec;
   float       dist     [MAX_PARTS];         /* distance at speed/level       */
   float       dur      [MAX_PARTS];         /* duration at speed/level       */
   float       pct      [MAX_PARTS];         /* lvl pct by distance           */
   float       cum      [MAX_PARTS];         /* cum pct by distance           */
   float       adj      [MAX_PARTS];         /* adjusted duration after fixes */
};
extern tACCEL g_accel_info [MAX_ACCEL];



#define    INCH2MM     25.400
#define    MM2COL      25.400
#define    MM2ROW      29.328




/*---(shared setup)-----+-----------+-----------+-----------+-----------+-----*/
char        ykine__setleg           (int a_num, int a_meth);
char        ykine__unsetleg         (void);
char        ykine__clear            (tSEG *a_curr, char *a_name, int a_leg, int a_seg, char a_type);
/*---(shared forward/inverse)-------+-----------+-----------+-----------+-----*/
char        ykine__thor             (void);
char        ykine__coxa             (void);
char        ykine__troc             (void);
/*---(forward kinematics)-----------+-----------+-----------+-----------+-----*/
char        ykine__femu             (float a_deg);
char        ykine__pate             (float a_deg);
char        ykine__tibi             (float a_deg);
char        ykine__lowr             (void);
char        ykine__FK_targ          (void);
/*---(inverse kinematics)-----------+-----------+-----------+-----------+-----*/
char        ykine__IK_targ          (float a_x, float a_z, float a_y);
char        ykine__IK_femu          (void);
char        ykine__IK_pate          (void);
char        ykine__IK_tibi          (void);
/*---(shared forward/inverse)-------+-----------+-----------+-----------+-----*/
char        ykine__meta             (void);
char        ykine__tars             (void);
char        ykine__foot             (void);
char*       ykine_calc__unit        (char *a_question, int a_num);



/*---(unit testing)-----------------+-----------+-----------+-----------+-----*/
char        ykine__setter           (char *a_request , int a_leg, int a_seg, float a_value);
char*       ykine__getter           (char *a_question, int a_leg, int a_seg);
char        ykine__unit_quiet       (void);
char        ykine__unit_loud        (void);
char        ykine__unit_end         (void);
/*---(unit testing)-----------------+-----------+-----------+-----------+-----*/



char        ykine_exact_calc        (char a_type, float a_beg, float a_end, float a_pct, float *a_cur);

float       ykine_exact_dist_xzy    (void);
float       ykine_exact_dist_doy    (char a_verb);
float       ykine_exact_dist_ypr    (void);
char        ykine_exact_dist_route  (char a_verb);

char        ykine_exact_pct_xzy     (float a_pct);
char        ykine_exact_pct_doy     (char a_verb, float a_pct);
char        ykine_exact_pct_ypr     (float a_pct);
char        ykine_exact_pct_route   (char a_verb, float a_pct);

char        ykine_exact_calc_length (float xp, float xc, float zp, float zc, float a_pct, float *l);
char        ykine_exact_calc_polar  (float l, float d, float *x, float *z);
char        ykine__exact_check      (tMOVE *a_curr, float a_sec);
char        ykine__exact_find       (tSERVO *a_servo, float a_sec);
char        ykine_exact_clearall    (void);
char        ykine_exact_context     (char a_leg, float a_margin);
char        ykine_exact_copy2pure   (float a_sec, char a_leg, char a_meth, char a_rc);
char        ykine_exact_copy2adapt  (float a_sec, char a_leg, char a_meth, char a_rc);
char        ykine_exact_fail2pure   (float a_sec, char a_leg, char a_meth, char a_rc);
char        ykine_exact_fail2adapt  (float a_sec, char a_leg, char a_meth, char a_rc);
char        ykine_exact_setbody     (float a_sec);

char        ykine_exact_fake_beg    (float db, float sb, float xb, float zb, float yb, float ob);
char        ykine_exact_fake_end    (float de, float se, float xe, float ze, float ye, float oe);
char        ykine_exact_fake_abeg   (float sb, float cb, float fb, float pb, float tb);
char        ykine_exact_fake_aend   (float se, float ce, float fe, float pe, float te);
char*       ykine__exact_unit       (char *a_question, int a_cnt);

char        ykine__exact_data       (tSERVO *a_servo, float a_sec);




char        ykine_verb_init         (void);

char        ykine_scrp_begin        (void);
char        ykine__scrp_prep        (void);
char*       ykine__unit_scrp        (char *a_question, int a_num);
char        ykine_scrp_prev         (tMOVE *a_move, float *d, float *x, float *z, float *y);
char        ykine_scrp_verb         (char *a_char);
char        ykine_hint_init         (void);
char        ykine_hint_final        (float a_sec);
char        ykine_hint_reset        (void);
char        ykine_hint_wrap         (void);
char        ykine_hint__new         (char *a_verb, int a_line, char *a_label, float a_sec);
char        ykine_scrp_section      (int n, uchar *a_verb);

char        ykine_body_ze_getter    (char *x_str, char *z_str, char *y_str, float *x, float *z, float *y);
char        ykine_body_xz2po        (float a_xpos, float a_zpos, float *a_deg, float *a_len);
char        ykine_body_po2xz        (float a_deg, float a_len, float *a_xpos, float *a_zpos);
char        ykine_body_po_getter    (char *d_str, char *o_str, char *y_str, float *x, float *z, float *y);
char        ykine_body_zero         (int n, uchar *a_verb);
char        ykine_body_polar        (int n, uchar *a_verb);
char        ykine_body_orient_valid (float y, float p, float r);
char        ykine_body_ypr2xzy      (float a_yaw, float a_pitch, float a_roll, float *a_x, float *a_z, float *a_y, float *a_l);
char        ykine_body_orient       (int n, uchar *a_verb);
char        ykine_body_pr2ti        (float p, float r, float *d, float *t);
char        ykine_body_ti2pr        (float d, float t, float *p, float *r);
char        ykine_body_tilt         (int n, uchar *a_verb);

/*---(forward)------------------------*/
char        ykine_legs_fk           (int n, uchar *a_verb);
/*---(inverse)------------------------*/
char        ykine_legs_ik           (int n, uchar *a_verb);
/*---(relative)-----------------------*/
char        ykine_legs_ik2rk        (float a_coxa, float x, float z, float *d, float *o);
char        ykine_legs_rk2ik        (float a_coxa, float d, float o, float *x, float *z);
char        ykine_legs_rk           (int n, uchar *a_verb);
/*---(center)-------------------------*/
char        ykine_legs_ik2ck        (float a_coxa, float x, float z, float *d, float *o);
char        ykine_legs_ck2ik        (float a_coxa, float d, float o, float *x, float *z);
char        ykine_legs_ck           (int n, uchar *a_verb);
/*---(tangent)------------------------*/
char        ykine_legs_tk_dist      (float *l);
char        ykine_legs_tk_exact     (float p);
char        ykine_legs_ik2tk        (float a_coxa, float x, float z, float *o, float *t);
char        ykine_legs_tk2ik        (float a_coxa, float o, float t, float *x, float *z);
char        ykine_legs_tk           (int n, uchar *a_verb);
/*---(neighborhood)-------------------*/
char        ykine_legs_ik2nk        (float a_coxa, float x, float z, float *nx, float *nz);
char        ykine_legs_nk2ik        (float a_coxa, float nx, float nz, float *x, float *z);
char        ykine_legs_nk           (int n, uchar *a_verb);
/*---(step)---------------------------*/
char        ykine_legs_ik2sk        (float a_coxa, float x, float z, float *d, float *o);
char        ykine_legs_sk2ik        (float a_coxa, float d, float o, float *x, float *z);
char        ykine_legs_sk           (int n, uchar *a_verb);
/*---(step)---------------------------*/


char        ykine_scrp_segno        (int n, uchar *a_verb);
char        ykine_scrp_repeat       (int n, uchar *a_verb);
char        ykine_scrp_exec         (void);
char        ykine_scrp_miss         (int n, uchar *a_verb);


char        ykine_servo_purge       (void);
char        ykine_servo_init        (void);
char        ykine_servo_prep        (void);
tSERVO*     ykine_servo_pointer     (int   a_leg, int a_seg);
/*> char        ykine_servo_unfocused   (int   a_leg, int a_seg);                     <*/
char        ykine_servo_side        (char a_char);
char        ykine_servo_rank        (char a_char);
char        ykine_servo_segment     (char *a_char);
char        ykine_servo_one         (char *a_source);
char        ykine_servos            (char *a_source);
char*       ykine__unit_servo       (char *a_question);


char        ykine_accel__clear      (void);
char        ykine_accel__level      (char a_part, char a_max , char a_level, char a_accel, char a_decel, float a_step, float *a_rem);
char        ykine_accel__alloc      (char a_part, float a_dist);
char        ykine_accel__dist       (float *a_dist, float *a_dur);
char        ykine_accel__pcts       (float a_dist);
int         ykine_accel__adjust     (float a_dur);
char        ykine_accel__calc       (char a_part, char a_meth, char a_action);
char        ykine_accel__dur        (cchar *a_dur);
char        ykine_accel__timing_save(void);
char        ykine_accel_timing      (void);
char        ykine_accel_make        (char a_acceln, float a_exact, char a_speedn, char a_deceln, char *a_out);
char        ykine_accel_body_adj    (float a_pct, char *a_out);
char        ykine_accel__servo      (char a_verb, char a_rc, char a_leg, int a_seg, float a_deg, float a_beat, char *a_label, char a_part, char a_cell);
char        ykine_accel__zero       (char a_verb, char a_rc, float x, float z, float y, float a_beat, char *a_label, char a_cell);
char        ykine_accel__single     (char a_verb, char a_rc, char a_leg, float f, float p, float t, float b, char *a_label, char a_part, char a_cell);
char        ykine_accel_reset       (char a_leg);
char        ykine_accel_append      (char a_verb, char a_part, char *a_accel);
char        ykine_accel_execute     (char *a_label);
char        ykine_accel_immediate   (char a_verb, char a_rc, char a_leg, float b, char *a_label);
char        ykine_exec_wait         (char a_leg, float a_wait);
char        ykine_exec_fill         (char a_leg, float a_wait);
char*       ykine_accel__unit       (char *a_question, int a_num);



/*===[[ YKINE_move.c ]]=======================================================*/
char        ykine_move_init         (void);
/*---(malloc)-------------------------*/
char        ykine__move_new         (tMOVE **a_move);
char        ykine__move_free        (tMOVE **a_move);

char        ykine_move_create       (tSERVO *a_servo, char a_type, char a_verb, int a_line, char *a_label, char a_part, char a_cell, float a_sec);
char        ykine_move_add_pure     (tSERVO *a_servo, char a_rc, float a_deg, float a_xpos, float a_zpos, float a_ypos);
char        ykine_move_add_adapt    (tSERVO *a_servo, char a_rc, float a_deg, float a_xpos, float a_zpos, float a_ypos);
char        ykine_move_add_zero     (tSERVO *a_servo, char a_rc, float a_xpos, float a_zpos, float a_ypos);
char        ykine_move_add_orient   (tSERVO *a_servo, char a_rc, float a_deg);
char        ykine_move_repeat       (tSERVO *a_servo, int a_times);
char        ykine_move_delete       (tMOVE **a_move);
char        ykine_move_clear_servo  (tSERVO *a_servo);
char        ykine_move_savedtail     (tSERVO *a_servo, float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz);
char        ykine_move_savedcurr    (tMOVE  *a_move , float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz);
char        ykine_move_savedprev    (tMOVE  *a_move , float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz);
char        ykine_move_curdata      (double *a_x, double *a_z, double *a_y);
char*       ykine__unit_move        (char *a_question, int a_leg, int a_seg, int a_move);

char        ykine_gait_beg          (int n, uchar *a_verb);
char        ykine_gait_end          (int n, uchar *a_verb);

char        ykine_scrp_walk         (int   a_repeats);
char        ykine_scrp_turn         (int   a_repeats);



/*===[[ YKINE_queue.c ]]======================================================*/
/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
char        ykine_scrp_by_code      (char a_code, char *a_terse, char *a_name, char *a_desc);
char        ykine_scrp_by_terse     (char *a_terse, char *a_code, char *a_name, char *a_desc);
char        ykine_scrp_popverb      (int n, uchar *a_verb, char a_exist, void *a_handler);
char        ykine_scrp_popservo     (void);
char        ykine_servo_list        (char *a_which);



/*===[[ YKINE_turtle.c ]]=====================================================*/
/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
char        ykine_turtle_init       (void);
char        ykine_turtle__last      (void);
char        ykine_turtle_speed      (int n, uchar *a_verb);
char        ykine_turtle_wait       (int n, uchar *a_verb);
char        ykine_turtle_home       (int n, uchar *a_verb);
char        ykine_turtle_move       (int n, uchar *a_verb);
char        ykine_turtle_goto       (int n, uchar *a_verb);
char        ykine_turtle_head       (int n, uchar *a_verb);
char        ykine_turtle_turn       (int n, uchar *a_verb);
char        ykine_turtle_raise      (int n, uchar *a_verb);
char        ykine_turtle_lower      (int n, uchar *a_verb);
char        ykine_turtle_depth      (int n, uchar *a_verb);
char*       ykine_turtle__unit      (char *a_question, int a_num);



/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        ykine_stance_spread     (char a_leg, double a_start, char *a_entry, double *a_result);
char        ykine_stance_radius     (double a_start, char *a_entry, double *a_result);
char        ykine_stance_radius_head (int *n, char *a_short);
char        ykine_stance_radius_next (int *n, char *a_short);
char        ykine_stance_height     (char a_verb, double a_start, char *a_entry, double *a_result);
char        ykine_stance_height_head (int *n, char *a_short);
char        ykine_stance_height_next (int *n, char *a_short);
char        ykine_stance_scale      (double a_start, char *a_entry, double *a_result);
char        ykine_stance_scale_head  (int *n, char *a_short);
char        ykine_stance_scale_next  (int *n, char *a_short);
char        ykine_stance            (int n, uchar *a_verb);
char        ykine_stance_verify     (void);
char        ykine__neighbors        (float a_x, float a_z, int a_col, int a_row);


char        ykine_step_init         (void);
char        ykine_step__defaults    (void);
char        ykine_step_shape        (char *a_step);
char        ykine_step_seq          (char *a_seq);
char        ykine_stepping          (char *a_mods);
char        ykine_step_accels       (void);
char        ykine_step_raise        (char a_verb);
char        ykine_step_plant        (char a_verb);
char        ykine_step__rounded     (float a_xzlen, float a_pct, float *y);
char        ykine_step_yshaper      (float a_xzlen, float a_pct, float *y);
char        ykine_step_show         (char *a_label);
char        ykine_step_copy         (tWORK *a, tWORK *b);
char        ykine_step_wait         (char a_verb, char a_leg);
char        ykine_step_fill         (char a_verb, char a_leg);
char*       ykine_step__unit        (char *a_question, int a_num);



/*---(malloc)---------------*/
char        ykine_tick_init         (void);
char        ykine_tick__new         (void);
char        ykine_tick__free        (void);
/*---(accessor)-------------*/
char        ykine_tick_setbody      (float a_sec);
char        ykine_tick_fill         (char a_leg, float a_beg, float a_end);
char        yKINE_tick_load         (void);
/*---(unitest)--------------*/
char*       ykine_tick__unit        (char *a_question, char a_leg, int a_tick);


/*---(shared)-------------------------*/
char        ykine_exec_servos       (char a_verb);
char        ykine_exec_get_prev     (int a_leg);
char        ykine_exec_prepare      (int a_line, char *a_one, char *a_two, char *a_thr, char *a_label, char *a_mods, char a_code, char *a_seg);
char        ykine_exec_driver       (int a_line, uchar *a_verb, char a_code);
char        ykine_exec_partial      (char a_verb, char a_leg, char a_ik);



#endif
/*============================[[    end-code    ]]============================*/
