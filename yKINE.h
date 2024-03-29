/*============================[[    beg-code    ]]============================*/

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
/*===[[ TERMINOLOGY ]]========================================================*/

/*   spider          : walking robot working on 6+ legs
 *   0 . core         : x, y, z position in space
 *   1 . thorax-a     : front-to-back tilt
 *   2 . thorax-b     : side-to-side tilt
 *   3 . thorax-c     : twist around vertical axis
 *   4 . coxa         : attachment angle radially around thorax
 *   5 . trochanter   : n/a for now (maybe rotation later)
 *   6 . femur        : horizontal joint that rotates front to back
 *   7 . patella      : vertical joint that swings up and down
 *   8 . tibia        : vertical joint that brings the leg to ground
 *   9 . metatarsus   : n/a for now (not all spiders)
 *   10. tarsus       : n/a for now
 *   11. claw         : n/a for now (maybe for switching "feet")
 *
 */
/*===[[ PURPOSE ]]============================================================*/

/*   yKINE is a set of functions to provide highly reliable, accurate, and
 *   predictable kinematic calculations for use in the control of a lynxmotion
 *   CHR-3 robotic hexapod with 18 DOF.  because the true location and
 *   orientation of the hexapod is a combination of these 18 DOF, machine
 *   control is very complex and most people resort to sticking with tradition,
 *   excel models, and/or trial and error to develop gaits and motions.
 *
 *   we must learn to master the kinematics involved from the ground up.  there
 *   are a great number of amazing experts out there pointing the way, but we
 *   can't fall into the trap of just becoming consumers and users of their
 *   great work -- build on their foundation and add your own creativity.
 *
 *   the goal of yKINE is to take a painful, tedious, onerous, haphazard, and
 *   error prone process and turn it a easy, flexible, predictable, rapid, and
 *   automated set of functions.
 *
 *   yKINE's core will focus on...
 *      - maintaining kinematic master data on body, legs, and other attachments
 *      - forward leg kinematics predicting x,y,z endpoint based on joint angles
 *      - inverse leg kinematics predicting joint angles based on x,y,z target
 *      - body movements to shift the relative leg positions
 *      - body orientation shifting almost all other calculations
 *      - adjustment relative to the ground to understand actual positioning
 *
 *   arachne will also provide...
 *      - debugging capability to display key information on the process
 *      - string-formatted kinetic data in easily displayable, consumable format
 *      - deep unit testing interfaces to prove computational correctness
 *
 *   as always, there are many, stable, accepted, existing programs and
 *   utilities built by better programmers that are likely superior in speed,
 *   size, capability, and reliability; BUT, i would not have learned nearly as
 *   much using them, so i follow the adage...
 *
 *   TO TRULY LEARN> do not seek to follow in the footsteps of the men of old;
 *   seek what they sought ~ Matsuo Basho
 *
 *   the underlying idea is to use this build process to learn new programming
 *   capabilities and put additional pressure on me to improve my standards,
 *   maintenance, and coding environment.  just using programs and ideas does
 *   not lead to understanding -- so get messy ;)
 *
 *   any one who wishes to become a good writer [programmer] should endeavour,
 *   before he allows himself to be tempted by the more showy qualities, to be
 *   direct, simple, brief, vigourous, and lucid -- henry watson fowler (1908)
 *
 *   simplicity is prerequisite for reliability - edsger dijkstra
 *
 */
/*===[[ DECISION -- TO CODE, MODEL, OR MANUAL ]]==============================*/

/*   situation (S)
 *
 *      robotics is a very active field, full of invention and discovery.
 *      walking robots provide a unique flexibity and have a tremendous
 *      potential to adapt to many uses.
 *
 *   complication (C)
 *
 *      walking robots require a huge amount of calculation and control in
 *      order to perform nearly any action.  with so many degrees of freedom
 *      involved their use is daunting to say the least.
 *
 *   question (Q)
 *
 *      how to make positioning and control of walking robots less challenging?
 *
 *   answer (A)
 *
 *      extract the kinematics from my existing arachne program and advance them
 *      to a shared service for all my control programs.
 *
 *   benefits (B)
 *      -- design once
 *      -- test once
 *      -- maintain once
 *
 *   risks (R)
 *      -- just to complex to serve that many masters
 *
 *   objectives (o)
 *
 *   alternatives (a)
 *
 *   tradoffs (t)
 *
 *
 */


/*  PROBLEM (pr)
 *     in order to be able to do anything unique with jointed robots such as
 *     hexapods, robotic arms, or even stranger; we will have to develop our
 *     own capability for modeling and control.  if we only wanted to use
 *     existing gaits and positions for show, this is not a problem, but we
 *     view jointed robots as useful to our future, to we must press on.
 *
 *     unfortunately kinematics is quite a complex and detailed field of study
 *     that will require much time and effort to develop sufficient skill to
 *     drive the creation of our own hexapod movements, gaits, and other control
 *
 *     if the future is about delegation of known tasks to automation, both
 *     virtual and physical, then we have to be able to handle custom robotics.
 *     since the world can not be fully controlled and the applications of the
 *     automation with be variable, we have to be able to adapt.
 *
 *     so, we agree that we need to have direct control and knowledge in the
 *     long-term, the current decision is whether to dive in now or wait
 *
 *
 *  OBJECTIVES (o)
 *     - treat the application of our hexapods as vital to our future
 *     - make it do basic and simple things early so we build a foundation
 *     - learn the application of kinematics early in our studies
 *     - have a platform that can be expanded and grown
 *     - learn new and wicked ways to build programming capabilities
 *     - learn to computer-control robots remotely (not a phucking joystick)
 *     - push us hard to be better and more creative developers
 *
 *
 *  ALTERNATIVES and CONSEQUENCES (ac)
 *
 *     1) use the built-in hexapod capabilities...
 *        - focus on what is current, rather than possible
 *        - no work required as it's already done and working
 *        - can only stand, walk under direction, turn around, and sit
 *        - would be nothing more than a curiousity (at bloody best)
 *
 *     2) use the excel sheets that others have developed...
 *        - already built, but need to port to CH3-R hexapod
 *        - can even help develop new gaits
 *        - still only helps look better but not do anything useful
 *        - can not get beyond a cooler factor of curiousity
 *
 *     3) buy an upgraded hexapod with programmed brains...
 *        - already built and running like getting a new puppy
 *        - has the ability to react to movement in spider-like ways
 *        - doesn't do anything useful beyond very cool recognition
 *        - costs a ton of money and is not expandable (dead end)
 *
 *     4) build a programming simulator...
 *        - can do anything i can figure out or dream (mostly)
 *        - will allow me to develop features like hexapod drillpress
 *        - gonna take a ton of time (but i need to do it sooner or later)
 *        - must learn many new tools (but, will learn many new tools)
 *
 *     5) give up for now...
 *        - no additional effort
 *        - no additional cost
 *        - get back into basic learning
 *        - focus on other priorities
 *
 *
 *  JUDGEMENT
 *
 *     i have already benefited from working with the hexapod simulator by
 *     learning a good deal about opengl, xwindows, and simulation.  it could
 *     be argued that i would never learn these things without a great and
 *     grand project to push me, so excellent to date.
 *
 *     going forward, there is great promise to automating physical activities
*      from making to survailance to assistance.  the sooner i start the further
*     further i will get.
*
*
*/


/*
 *
 * all direct motor control is done using joint angles (fpt)
 *    -- fpt means the three joint angles of femur, patella, and tibia
 *    -- signals are sent to each motor to inticate an exact destination angle
 *
 * forward kinematic (FK)
 *    -- set the joint angles (fpt) which results in the location (xzy)
 *    -- simpliest, but simplistic and mostly not useful
 *    -- good for initial setups
 *    -- FK moves follow a direct linear movement, no curving
 *
 * inverse kinematics (IK)
 *    -- set the location (xzy) which results in the joint angles (fpt)
 *    -- complex mathmatics, but the most useful
 *    -- but, requires a lot of prework to designate endpoints
 *    -- can use front-ends of CK, RK, TK, and NK to simplify
 *    -- IK moves follow a direct linear movement, no curving
 *
 * neighborhood kinematics (NK)
 *    -- uses xzy location relative to military stance to drive IK
 *    -- xzy are normal, but relative to military
 *    -- really useful for steps and gaits
 *    -- NK moves follow a direct linear movement, no curving
 *
 * tangent kinematics (TK)
 *    -- uses tangent (oty) to get to xzy then uses IK
 *    -- o is distance inward (neg) or outward (pos) from military stance
 *    -- t is tangent distance clockwise (neg) or counter-clockwise (pos)
 *    -- y is the normal y
 *    -- forgot use-case when in details, think its useful
 *    -- TK moves follow a direct linear movement, no curving
 *
 * center-polar kinematics (CK)
 *    -- uses polar notation (doy) to get to xzy then uses IK
 *    -- d is the angle difference to endpoint relative to thorax (left pos)
 *    -- o is distance inward (neg) or outward (pos) from military stance
 *    -- y is the normal y
 *    -- really useful for turning and spinning
 *    -- CK moves curve on d changes, but are linear on other two
 *
 * relative-polar kinematics (RK)
 *    -- uses polar notation (doy) to get to xzy then uses IK
 *    -- d is the angle difference for femur relative to military (left pos)
 *    -- o is distance inward (neg) or outward (pos) from military stance
 *    -- y is the normal y
 *    -- really useful for setting up stances
 *    -- RK moves curve on d changes, but are linear on other two
 *
 * step-polar kinematics (SK)
 *    -- uses polar notation (doy) to get to xzy then uses IK
 *    -- d is the angle difference from zero for reference (left pos)
 *    -- o is distance inward (neg) or outward (pos) from military stance
 *    -- y is the normal y
 *    -- really useful for steps and gaits
 *    -- SK moves curve on d changes, but are linear on other two
 *
 *
 */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE
#define yKINE yes



/*===[[ CONSTANTS ]]======================================*/



/*---(method/verbs)-------------------*/
#define     YKINE_MAX_METH  10

/*---(general)------------------------*/
#define     YKINE_FK         0
#define     YKINE_IK         1
#define     YKINE_GK         2
#define     YKINE_PK         3
/*---(specific)-----------------------*/
#define     YKINE_TK         3
#define     YKINE_CK         4
#define     YKINE_RK         5
#define     YKINE_SK         6
#define     YKINE_NK         7
#define     YKINE_ZE         8
#define     YKINE_PO         9
#define     YKINE_OR        10
#define     YKINE_TI        11
#define     YKINE_TSP       12
#define     YKINE_TWA       13
#define     YKINE_THM       14
#define     YKINE_TMV       15
#define     YKINE_TGO       16
#define     YKINE_THE       17
#define     YKINE_TTU       18
#define     YKINE_TLO       19
#define     YKINE_TRA       20
#define     YKINE_TDE       21
#define     YKINE_TPU       22
#define     YKINE_TPO       23
#define     YKINE_ST        24
#define     YKINE_SYNC      25
/*---(minor)--------------------------*/
#define     YKINE_SEGN      40
#define     YKINE_REPT      41
#define     YKINE_PHRA      42
#define     YKINE_STZA      43
/*---(major)--------------------------*/
#define     YKINE_PASS      50
#define     YKINE_SECT      51
/*---(external)-----------------------*/
#define     YKINE_SONG      55
/*---(errors)-------------------------*/
#define     YKINE_BODE      66
#define     YKINE_LEGE      67
#define     YKINE_MISS      68
/*---(special)------------------------*/
#define     YKINE_NOOP      99



/*---(overall)------------------------*/
#define     YKINE_MAX_LEGS  15       /* all possible legs      */
#define     YKINE_CENTER    -1
#define     YKINE_BODY       0
/*---(big legs)-----------------------*/
#define     YKINE_RR         1
#define     YKINE_RM         2
#define     YKINE_RF         3
#define     YKINE_LF         4
#define     YKINE_LM         5
#define     YKINE_LR         6
/*---(little legs)--------------------*/
#define     YKINE_RP         7
#define     YKINE_RA         8
#define     YKINE_LA         9
#define     YKINE_LP        10
/*---(special)------------------------*/
#define     YKINE_BEAK      11
#define     YKINE_CAMOF     12
#define     YKINE_CAMOR     13



/*---(segments)-----------------------*/
#define     YKINE_MAX_SEGS  25
#define     YKINE_FOCU       0
#define     YKINE_CORE       1
#define     YKINE_THOR       2
#define     YKINE_COXA       3
#define     YKINE_TROC       4
#define     YKINE_FEMU       5
#define     YKINE_PATE       6
#define     YKINE_TIBI       7
#define     YKINE_META       8
#define     YKINE_TARS       9
#define     YKINE_FOOT      10
#define     YKINE_CLAW      11
#define     YKINE_MAGN      12
#define     YKINE_HOOK      13
#define     YKINE_ORIG      14
#define     YKINE_TARG      15
#define     YKINE_LOWR      16
#define     YKINE_CALC      17

/*---(body)---------------------------*/
#define     YKINE_YAW        5
#define     YKINE_PITCH      6
#define     YKINE_ROLL       7

/*---(servos)-------------------------*/
#define     YKINE_MAX_SERVO   64

/*---(move searches)------------------*/
#define     YKINE_HEAD      '['
#define     YKINE_NEXT      '>'
#define     YKINE_SAME      '`'
#define     YKINE_PREV      '<'
#define     YKINE_TAIL      ']'
#define     YKINE_POS       'p'
#define     YKINE_DEG       'd'


#define     YKINE_NULL      '-'
#define     YKINE_SERVO     's'
#define     YKINE_WAIT      'w'
#define     YKINE_INIT      'i'
#define     YKINE_NOTE      'n'
#define     YKINE_DEL       'x'
#define     YKINE_MARK      '#'
#define     YKINE_TEMPO     '>'


#define     YKINE_NONE        '-'
#define     YKINE_FROM        'f'
#define     YKINE_PURE        'p'
#define     YKINE_DOWN        'd'

#define     YKINE_FORWARD     'f'
#define     YKINE_INVERSE     'i'
#define     YKINE_CONTROL     'c'
#define     YKINE_ORIENT      'o'
#define     YKINE_ZERO        'z'


#define     YKINE_LINEAR      'l'
#define     YKINE_POLAR       'p'
#define     YKINE_ZPOLAR      'z'
#define     YKINE_OPOLAR      'z'
#define     YKINE_MAX         'm'

char*       yKINE_version           (void);
char        yKINE_init              (void);
char        yKINE_reinit            (void);
char        yKINE_sizing            (char a_type);

char        yKINE_zero              (float a_x, float a_z, float a_y);
char        yKINE_orient            (float a_yaw, float a_pitch, float a_roll);
char        yKINE_forward           (char  a_leg, float a_femu, float a_pate, float a_tibi);
char        yKINE_inverse           (char  a_leg, float a_x, float a_z, float a_y);
char        yKINE_adapt             (char  a_leg, float a_x, float a_z, float a_y);
char        yKINE_opengl            (char  a_leg, char  a_seg, float a_deg, float a_x, float a_z, float a_y, float a_len);
char        yKINE_pure              (char  a_leg, char  a_seg, float a_deg, float a_x, float a_z, float a_y, float a_len);

float       yKINE_radius            (char *a_entry);
float       yKINE_height            (char *a_entry);
float       yKINE_scale             (char *a_entry);

/*---(static data accessors)----------*/
char*       yKINE_legtwo            (char a_leg);
char*       yKINE_legcaps           (char a_leg);
char*       yKINE_legfull           (char a_leg);
float       yKINE_legdeg            (char a_leg);
char*       yKINE_segfour           (char a_seg);
char*       yKINE_segcaps           (char a_seg);
char*       yKINE_segfull           (char a_seg);
float       yKINE_seglen            (char a_seg);
float       yKINE_segmin            (char a_seg);
float       yKINE_segattn           (char a_seg);
float       yKINE_segmax            (char a_seg);

/*---(dynamic datae accessors)--------*/
char        yKINE_endpoint          (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y, float *a_xz);
char        yKINE_segment           (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y, float *a_xz);
char        yKINE_angle             (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_cd, float *a_len, float *a_cv, float *a_ch, float *a_fv, float *a_fh);
char        yKINE_enddiff           (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y);
char        yKINE_segdiff           (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y);
char        yKINE_angles            (char a_leg, char a_meth, float *a_coxa, float *a_femu, float *a_pate, float *a_tibi);


/*---rc---- -----function---------- -----parameters---------------------------*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yKINE_servo_index       (char a_leg, char a_seg);
char        yKINE_servo_cursor      (char a_dir);
/*> char        yKINE_servo_which       (int a_seq, int *a_leg, int *a_seg);          <*/
/*> char        yKINE_servo             (char *a_source);                             <*/
char        yKINE_sect_cursor       (char a_dir, char *a_major, char *a_minor, int *a_line, char *a_label, float *a_sec);

char        yKINE_exact_all         (float  a_time);
char        yKINE_exact_leg         (char a_leg, float a_margin, char *a_exact, char *a_label, char *a_cell, char *pure, float *f, float *p, float *t, float *x, float *z, float *y, char *adapt, float *fr, float *pr, float *tr, float *xr, float *zr, float *yr);

char        yKINE_move_cursor       (char a_dir, char a_leg, char a_seg, float *s, float *d);
char        yKINE_zero_cursor       (char a_dir, float *s, float *x, float *z, float *y);
char        yKINE_move_tail        (int a_servo, float *a_sec, float *a_deg);

char        yKINE_move_rpt          (void);
char        yKINE_sect_rpt          (void);


char        yKINE_neighbors         (float *a_center, float *a_s, float *a_se, float *a_ne, float *a_n, float *a_nw, float *a_sw);
char        yKINE_xz2hexdo          (float x, float z, int *c, int *r, float *d, float *o);
char        yKINE_xz2hex            (float x, float z, int *c, int *r);
char        yKINE_hex2xz            (int c, int r, float *x, float *z);
char        yKINE_hex2adj           (int c, int r, int rc, int *ac, int *ar);
char        yKINE_verify_rc         (int a_row, int a_col);



char        yKINE_handlers          (void);
char        yKINE_scrp_prepper      (char a_pass);
char        yKINE_scrp_finisher     (char a_pass);

char        yKINE_ticker            (int a_row, int a_col, char a_type, float *b, float *e, char *r);
char        yKINE_tick_deg          (int a_leg, int a_tick, float *a_femu, float *a_pate, float *a_tibi);
char        yKINE_tick_end          (int a_leg, int a_tick, float *x, float *z, float *y);
char        yKINE_tick_exp          (int a_leg, int a_tick, float *x, float *z, float *y);
char        yKINE_tick_act          (int a_leg, int a_tick, float *x, float *z, float *y);
char        yKINE_tick_opengl       (int a_leg, int a_tick, float x, float z, float y);

/*> char        yKINE_phys_flat    (char a_meth, float a_sec, float *a_lowest, int *a_count);   <*/



#endif
/*============================[[    end-code    ]]============================*/
