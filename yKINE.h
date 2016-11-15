/*============================[[    beg-code    ]]============================*/

/*===[[ HEADER ]]=============================================================*/

/*   focus         : (RO) robotics
 *   niche         : (sp) spiders
 *   application   : yKINE
 *   purpose       : shared, consistent, and reliable spider kinematics
 *
 *   base_system   : gnu/linux   (powerful, ubiquitous, technical, and hackable)
 *   lang_name     : ansi-c      (wicked, limitless, universal, and everlasting)
 *   dependencies  : none
 *   size          : small       (less than 500 slocL)
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
*/



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yKINE
#define yKINE yes



/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define     YKINE_VER_NUM   "0.6i"
#define     YKINE_VER_TXT   "redivided ik and fk data structures to allow better testing"



/*===[[ CONSTANTS ]]======================================*/

/*---(methods)------------------------*/
#define     YKINE_MAX_METH   5
#define     YKINE_FK         0
#define     YKINE_IK         1
#define     YKINE_GK         2

/*---(big legs)-----------------------*/
#define     YKINE_MAX_LEGS  15
#define     YKINE_RR         0
#define     YKINE_RM         1
#define     YKINE_RF         2
#define     YKINE_LF         3
#define     YKINE_LM         4
#define     YKINE_LR         5
/*---(little legs)--------------------*/
#define     YKINE_RP         6
#define     YKINE_RA         7
#define     YKINE_LA         8
#define     YKINE_LA         9

/*---(body)---------------------------*/
#define     YKINE_YAW        0
#define     YKINE_PITCH      1
#define     YKINE_ROLL       2

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
#define     YKINE_LEGN      16
#define     YKINE_LOWR      17
#define     YKINE_CALC      18




char*       yKINE_version      (void);
char        yKINE_debug        (char a_flag);
char        yKINE_init         (char a_type);

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
yKINE_inverse      (int a_num, float a_x, float a_z, float a_y);

char         /*--> drive the leg position from angles ----[ ------ [ ------ ]-*/
yKINE_forward      (int a_num, float a_femu, float a_pate, float a_tibi);



#endif
/*============================[[    end-code    ]]============================*/
