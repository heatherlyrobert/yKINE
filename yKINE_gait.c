/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                        gait verbs                            ----===*/
/*====================------------------------------------====================*/
static void      o___GAITS___________________o (void) {;}

tMOVE      *s_gait_begin  [YKINE_MAX_SERVO];

char         /*--> prepare to read a gait ----------------[ ------ [ ------ ]-*/
ykine_gait__begin       (char a_scrp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i, j        = 0;
   char       *p           = NULL;
   int         x_len       = 0;
   int         x_servo     = 0;
   char        x_list      [LEN_HUND];
   double      x_deg       = 0.0;
   double      x_xpos      = 0.0;
   double      x_zpos      = 0.0;
   double      x_ypos      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_char    ("a_scrp"    , a_scrp);
   /*---(mark servers)----------------*/
   if (a_scrp == 'y') {
      DEBUG_YKINE_SCRP   yLOG_note    ("script based verb");
      rc  = ykine_scrp_popservo ();
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("internal/complex verb");
      rc = ykine_servos ("++");
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(create servo headers)-----------*/
   for (i = 0; i < g_nservo; ++i) {
      if (x_list [i] == '_')  continue;
      for (j = 0; j <= 2; ++j) {
         /*---(filter uninvolved servos)----*/
         x_servo = (i - 2) + j;
         s_gait_begin [x_servo] = NULL;
         /*---(get last position)-----------*/
         DEBUG_YKINE_SCRP   yLOG_value    ("servo"     , x_servo);
         DEBUG_YKINE_SCRP   yLOG_info     ("name"      , g_servo_info [x_servo].label);
         yKINE_move_last_servo  (x_servo, NULL, &x_deg);
         yKINE_move_curdata     (&x_xpos, &x_zpos, &x_ypos);
         DEBUG_YKINE_SCRP   yLOG_double   ("last deg"  , x_deg);
         /*---(write header note)-----------*/
         ykine_move_create (YKINE_MOVE_NOTE , g_servo_info + x_servo, "incipio"      , myKINE.s_nline, 0.0  , 0.0);
         /*---(write header placeholders)---*/
         ykine_move_addloc (g_servo_info + x_servo, x_xpos, x_zpos, x_ypos);
         ykine_move_create (YKINE_MOVE_WAIT , g_servo_info + x_servo, "wait for turn", myKINE.s_nline, x_deg, 3.0);
         ykine_move_addloc (g_servo_info + x_servo, x_xpos, x_zpos, x_ypos);
         ykine_move_create (YKINE_MOVE_SERVO, g_servo_info + x_servo, "up to neutral", myKINE.s_nline, 0.0  , 0.5);
         ykine_move_create (YKINE_MOVE_SERVO, g_servo_info + x_servo, "step to first", myKINE.s_nline, 0.0  , 0.5);
         ykine_move_create (YKINE_MOVE_WAIT , g_servo_info + x_servo, "wait for legs", myKINE.s_nline, 0.0  , 3.0);
         /*---(save end of header)----------*/
         DEBUG_YKINE_SCRP   yLOG_point    ("saved"     , g_servo_info [x_servo].tail);
         s_gait_begin [x_servo] = g_servo_info [x_servo].tail;
         /*---(done)------------------------*/
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static double  s_neux  = 0.0;
static double  s_neuz  = 0.0;
static double  s_neuy  = 0.0;
static double  s_neud  = 0.0;

static char
yKINE__gait_save_neutral  (int a_servo)
{  /*---(design notes)-------------------*/
   /*  the very first move in the gait is to the neutral position.  this
    *  means it is one (next) after the header.
    */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   /*> DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);                                <*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_servo"   , a_servo);
   DEBUG_YKINE_SCRP   yLOG_point   ("gait_begin", s_gait_begin [a_servo]);
   DEBUG_YKINE_SCRP   yLOG_point   ("->s_next"  , s_gait_begin [a_servo]->s_next);
   /*---(get to neutral move)------------*/
   x_curr = s_gait_begin [a_servo]->s_next;  /* right after header   */
   /*> DEBUG_YKINE_SCRP  yLOG_spoint  (x_curr);                                       <*/
   DEBUG_YKINE_SCRP   yLOG_point   ("x_curr"    , x_curr);
   /*---(save neutral data)--------------*/
   s_neux  = x_curr->x_pos;
   s_neuz  = x_curr->z_pos;
   s_neuy  = x_curr->y_pos;
   s_neud  = x_curr->degs;
   /*---(complete)-----------------------*/
   /*> DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);                                <*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static double  s_1stx  = 0.0;
static double  s_1stz  = 0.0;
static double  s_1sty  = 0.0;
static double  s_1std  = 0.0;

static char
yKINE__gait_roll     (int a_servo, int a_order, int a_count)
{  /*---(design notes)-------------------*/
   /*  gait order determines how many moves to roll.  first leg rolls none
    *  and starts in neutral position.
    */
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_times     = 0;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_next      = NULL;
   tSERVO     *x_servo     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prep)---------------------*/
   switch (a_count) {
   case  6 : x_times = a_order    ;  break;
   case 12 : x_times = a_order * 2;  break;
   case 18 : x_times = a_order * 3;  break;
   default : return -1;              break;
   }
   /*---(roll moves to end)--------*/
   x_servo = s_gait_begin [a_servo]->servo;;
   x_curr  = s_gait_begin [a_servo]->s_next;
   for (i = 0; i < x_times; ++i) {
      /*---(prepare for delete)----*/
      x_next  = x_curr->s_next;
      /*---(add dup to end)--------*/
      ykine_move_create (YKINE_MOVE_SERVO, x_servo , "extended", x_curr->line, x_curr->degs, x_curr->dur);
      ykine_move_addloc (x_servo, x_curr->x_pos, x_curr->z_pos, x_curr->y_pos);
      /*---(delete current)--------*/
      ykine_move_delete (x_curr);
      /*---(go to next)------------*/
      x_curr = x_next;
      DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
      /*---(done)------------------*/
   }
   /*---(save first pos data)------------*/
   s_1stx  = x_curr->x_pos;
   s_1stz  = x_curr->z_pos;
   s_1sty  = x_curr->y_pos;
   s_1std  = x_curr->degs;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static double  s_inix  = 0.0;
static double  s_iniz  = 0.0;
static double  s_iniy  = 0.0;
static double  s_inid  = 0.0;

static char
yKINE__gait_1st_wait      (int a_servo, int a_order)
{  /*---(design notes)-------------------*/
   /*  the first wait is three places before the header end.  the wait is the
    *  same for both 6 and 12 step gaits.
    */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get to neutral move)------------*/
   x_curr = s_gait_begin [a_servo]->s_prev->s_prev->s_prev;
   DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
   /*---(save values)--------------------*/
   s_inix  = x_curr->x_pos;
   s_iniz  = x_curr->z_pos;
   s_iniy  = x_curr->y_pos;
   s_inid  = x_curr->degs;
   /*---(update values)------------------*/
   x_curr->dur = (double) (5 - a_order) * 0.500;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char
yKINE__gait_fix_neutral   (int a_servo)
{  /*---(design notes)-------------------*/
   /*  update the headers neutral entry which is two places back in the header.
   */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get to neutral move)------------*/
   x_curr = s_gait_begin [a_servo]->s_prev->s_prev;
   DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
   /*---(update values)------------------*/
   x_curr->x_pos           = s_neux;
   x_curr->z_pos           = s_neuz;
   x_curr->y_pos           = s_neuy;
   x_curr->degs            = s_neud;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char
yKINE__gait_fix_first     (int a_servo)
{  /*---(design notes)-------------------*/
   /*  update the headers neutral entry which is two places back in the header.
   */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get to neutral move)------------*/
   x_curr = s_gait_begin [a_servo]->s_prev;
   DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
   /*---(update values)------------------*/
   x_curr->x_pos           = s_1stx;
   x_curr->z_pos           = s_1stz;
   x_curr->y_pos           = s_1sty;
   x_curr->degs         = s_1std;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char
yKINE__gait_2nd_wait      (int a_servo, int a_order)
{  /*---(design notes)-------------------*/
   /*  the first wait is three places before the header end.  the wait is the
    *  same for both 6 and 12 step gaits.
    */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get to neutral move)------------*/
   x_curr = s_gait_begin [a_servo];
   DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
   /*---(update values)------------------*/
   x_curr->x_pos           = s_1stx;
   x_curr->z_pos           = s_1stz;
   x_curr->y_pos           = s_1sty;
   x_curr->degs         = s_1std;
   x_curr->dur = (double) a_order * 0.500;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char
yKINE__gait_fix_timings   (int a_servo)
{  /*---(design notes)-------------------*/
   /*  all the timings are off after rolling forward and updating
   */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_curr      = NULL;
   float       d           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get to neutral move)------------*/
   x_curr = s_gait_begin [a_servo]->s_prev->s_prev->s_prev;
   DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
   /*---(fix waiting)--------------*/
   while (x_curr != NULL) {
      if (x_curr->s_prev != NULL)  d = x_curr->s_prev->secs;
      else                         d = 0.0;
      x_curr->secs = d + x_curr->dur;
      x_curr = x_curr->s_next;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char
yKINE__gait_add_tail      (int a_servo, int a_order)
{  /*---(design notes)-------------------*/
   /*  all the timings are off after rolling forward and updating
   */
   /*---(locals)-----------+-----------+-*/
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get to neutral move)------------*/
   x_curr  = s_gait_begin [a_servo]->servo->tail;
   DEBUG_YKINE_SCRP  yLOG_point   ("x_curr"    , x_curr);
   x_servo = s_gait_begin [a_servo]->servo;;
   /*---(add tail)-----------------------*/
   ykine_move_create (YKINE_MOVE_WAIT , x_servo , "wait for turn", -1     , x_curr->degs  , a_order * 0.500);
   ykine_move_addloc (x_servo, x_curr->x_pos, x_curr->z_pos, x_curr->y_pos);
   ykine_move_create (YKINE_MOVE_SERVO, x_servo , "up to neutral", -1           , s_neud          , 0.500           );
   ykine_move_addloc (x_servo, s_neux, s_neuz, s_neuy);
   ykine_move_create (YKINE_MOVE_SERVO, x_servo , "back down"    , -1           , s_inid          , 0.500           );
   ykine_move_addloc (x_servo, s_inix, s_iniz, s_iniy);
   ykine_move_create (YKINE_MOVE_WAIT , x_servo , "wait all legs", -1       , s_inid          , (5 - a_order) * 0.500);
   ykine_move_addloc (x_servo, s_inix, s_iniz, s_iniy);
   ykine_move_create (YKINE_MOVE_NOTE , x_servo , "compleo"      , myKINE.s_nline, 0.0, 0.0);
   ykine_move_addloc (x_servo, s_inix, s_iniz, s_iniy);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> parse a high level repeat -------------[ ------ [ ------ ]-*/
ykine_gait__update      (char a_scrp, char a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           = 0;
   int         j           = 0;
   char       *p           = NULL;
   int         x_len       = 0;
   char        x_order     [LEN_LABEL];
   int         x_leg       = 0;
   int         x_snum      = 0;
   double      x_count     = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   --rce;  if (a_scrp == 'y') {
      DEBUG_YKINE_SCRP   yLOG_note    ("script based verb");
      rc  = ykine_scrp_popservo ();
      DEBUG_YKINE_SCRP   yLOG_value   ("popservo"  , rc);
      --rce;  if (rc < 0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("script based count");
      rc = yPARSE_popval    (0.0, &x_count);
      DEBUG_YKINE_SCRP   yLOG_value   ("popval"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("script based order");
      rc = yPARSE_popstr    (x_order);
      DEBUG_YKINE_SCRP   yLOG_value   ("popstr"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("internal/complex verb");
      rc = ykine_servos ("++");
      DEBUG_YKINE_SCRP   yLOG_value   ("servos"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      strlcpy (x_order, "315240", LEN_LABEL);
      x_count = 12;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("x_count"   , x_count);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_order"   , x_order);
   x_len = strlen (x_order);
   DEBUG_YKINE_SCRP   yLOG_value   ("x_len"     , x_len);
   /*---(walk through leg order)---------*/
   for (i = 0; i < x_len; ++i) {
      x_leg = (int) (x_order [i] - '0');
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(handle three servos each)----*/
      for (j = 0; j < 3; ++j) {
         /*---(figure starting move)-----*/
         x_snum = ykine_servo_find (x_leg + 1, j + YKINE_FEMU);
         DEBUG_YKINE_SCRP  yLOG_value   ("x_snum"    , x_snum);
         /*---(get neutral data)---------*/
         yKINE__gait_save_neutral (x_snum);
         /*---(delete/copy to first)-----*/
         yKINE__gait_roll         (x_snum, i, x_count);
         /*---(fix first wait)-----------*/
         yKINE__gait_1st_wait     (x_snum, i);
         /*---(fix neutral)--------------*/
         yKINE__gait_fix_neutral  (x_snum);
         /*---(fix first)----------------*/
         yKINE__gait_fix_first    (x_snum);
         /*---(fix second wait)----------*/
         yKINE__gait_2nd_wait     (x_snum, i);
         /*---(fix waiting)--------------*/
         yKINE__gait_fix_timings  (x_snum);
         /*---(add to end)---------------*/
         yKINE__gait_add_tail     (x_snum, i);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       complex movements                      ----===*/
/*====================------------------------------------====================*/
static void      o___COMPLEX_________________o (void) {;}

char ykine_gait_beg      (void) { ykine_gait__begin    ('y');    }
char ykine_gait_end      (void) { ykine_gait__update   ('y', 0); }


/*> char      /+--> walking ---------------------------[--------[--------]-+/                                 <* 
 *> ykine_scrp_walk        (int a_repeats)                                                                    <* 
 *> {                                                                                                         <* 
 *>    /+---(locals)-----------+-----------+-+/                                                               <* 
 *>    char        rce         = -10;                /+ return code for errors    +/                          <* 
 *>    char        rc          = 0;                                                                           <* 
 *>    char       *p           = NULL;                                                                        <* 
 *>    int         x_len       = 0;                                                                           <* 
 *>    int         x_cycle     = 0;                                                                           <* 
 *>    int         x_step      = 0;                                                                           <* 
 *>    char        x_request   [LEN_LABEL];                                                                   <* 
 *>    char        x_verb      [LEN_LABEL] = "ik_from";                                                       <* 
 *>    char        x_style     [LEN_LABEL];                                                                   <* 
 *>    /+---(header)-------------------------+/                                                               <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                                        <* 
 *>    /+---(parse style)--------------------+/                                                               <* 
 *>    DEBUG_YKINE_SCRP   yLOG_note    ("walking style field");                                               <* 
 *>    p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);                                                  <* 
 *>    --rce;  if (p == NULL) {                                                                               <* 
 *>       DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");                                         <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                                     <* 
 *>    }                                                                                                      <* 
 *>    strltrim (p, ySTR_BOTH, LEN_RECD);                                                                     <* 
 *>    strlcpy  (x_style, p, LEN_LABEL);                                                                      <* 
 *>    x_len = strlen (x_style);                                                                              <* 
 *>    DEBUG_YKINE_SCRP  yLOG_info    ("x_style"   , x_style);                                                <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);                                                  <* 
 *>    /+---(prepare)------------------------+/                                                               <* 
 *>    rc = ykine__scrp_prep      ();                                                                         <* 
 *>    ykine_gait__begin    (0);                                                                               <* 
 *>    /+---(loop repeats)-------------------+/                                                               <* 
 *>    for (x_cycle = 0; x_cycle < a_repeats; ++x_cycle) {                                                    <* 
 *>       for (x_step = 0; x_step < 12; ++x_step) {                                                           <* 
 *>          /+---(prepare)------------------------+/                                                         <* 
 *>          rc = ykine__scrp_prep      ();                                                                   <* 
 *>          rc = ykine_parse_prep      (x_verb);                                                             <* 
 *>          if (rc < 0) {                                                                                    <* 
 *>             DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                                                <* 
 *>             return rc;                                                                                    <* 
 *>          }                                                                                                <* 
 *>          /+---(identify servos)----------------+/                                                         <* 
 *>          strlcpy   (x_request, "++.femu", LEN_LABEL);                                                     <* 
 *>          myKINE.s_count = ykine_servos (x_request);                                                       <* 
 *>          DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);                                   <* 
 *>          /+---(fill common fields)-------------+/                                                         <* 
 *>          myKINE.s_secs = 0.150;                                                                           <* 
 *>          myKINE.s_zpos = 0.00;                                                                            <* 
 *>          /+> switch (x_step) {                                                        <*                  <* 
 *>           *> case  0 :  myKINE.s_xpos =   0.00;  myKINE.s_ypos =   25.00; break;                    <*    <* 
 *>           *> case  1 :  myKINE.s_xpos = -45.00;  myKINE.s_ypos =   10.00; break;                    <*    <* 
 *>           *> case  2 :  myKINE.s_xpos = -40.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  3 :  myKINE.s_xpos = -30.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  4 :  myKINE.s_xpos = -20.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  5 :  myKINE.s_xpos = -10.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  6 :  myKINE.s_xpos =   0.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  7 :  myKINE.s_xpos =  10.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  8 :  myKINE.s_xpos =  20.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case  9 :  myKINE.s_xpos =  30.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case 10 :  myKINE.s_xpos =  40.00;  myKINE.s_ypos =    0.00; break;                    <*    <* 
 *>           *> case 11 :  myKINE.s_xpos =  45.00;  myKINE.s_ypos =   10.00; break;                    <*    <* 
 *>           *> }                                                                        <+/                 <* 
 *>          switch (x_step) {                                                                                <* 
 *>          case  0 :  myKINE.s_xpos =   0.00;  myKINE.s_ypos =   25.00; break;                              <* 
 *>          case  1 :  myKINE.s_xpos = -32.00;  myKINE.s_ypos =   10.00; break;                              <* 
 *>          case  2 :  myKINE.s_xpos = -28.00;  myKINE.s_ypos =    0.00; break;                              <* 
 *>          case  3 :  myKINE.s_xpos = -21.00;  myKINE.s_ypos =    0.00; break;                              <* 
 *>          case  4 :  myKINE.s_xpos = -14.00;  myKINE.s_ypos =    0.00; break;                              <* 
 *>          case  5 :  myKINE.s_xpos =  -7.00;  myKINE.s_ypos =    0.00; break;                              <* 
 *>          case  6 :  myKINE.s_xpos =   0.00;  myKINE.s_ypos =    0.00; break;                              <* 
 *>          case  7 :  myKINE.s_xpos =   7.00;  myKINE.s_ypos =    0.00; break;                              <* 
 *>          case  8 :  myKINE.s_xpos =  14.00;  myKINE.s_ypos =    0.00; break;                              <* 
*>          case  9 :  myKINE.s_xpos =  21.00;  myKINE.s_ypos =    0.00; break;                              <* 
*>          case 10 :  myKINE.s_xpos =  28.00;  myKINE.s_ypos =    0.00; break;                              <* 
*>          case 11 :  myKINE.s_xpos =  32.00;  myKINE.s_ypos =   10.00; break;                              <* 
*>          }                                                                                                <* 
*>          /+---(check)--------------------------+/                                                         <* 
*>          /+> rc = ykine_parse_check   ();                                             <*                  <* 
*>           *> if (rc < 0) {                                                            <*                  <* 
   *>           *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                        <*                  <* 
      *>           *>    return rc;                                                            <*                  <* 
      *>           *> }                                                                        <+/                 <* 
      *>          /+---(handle IK)----------------------+/                                                         <* 
      *>          rc = ykine_scrp_ik_from   ();                                                                    <* 
      *>       }                                                                                                   <* 
      *>    }                                                                                                      <* 
      *>    rc = ykine__scrp_prep      ();                                                                         <* 
      *>    ykine_gait_update   (0);                                                                               <* 
      *>    /+---(complete)-----------------------+/                                                               <* 
      *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                                        <* 
      *>    return 0;                                                                                              <* 
      *> }                                                                                                         <*/

      /*> char      /+--> turning ---------------------------[--------[--------]-+/                   <* 
       *> ykine_scrp_turn        (int a_repeats)                                                      <* 
       *> {                                                                                           <* 
       *>    /+---(locals)-----------+-----------+-+/                                                 <* 
       *>    char        rce         = -10;                /+ return code for errors    +/            <* 
       *>    char        rc          = 0;                                                             <* 
       *>    char       *p           = NULL;                                                          <* 
       *>    int         x_len       = 0;                                                             <* 
       *>    int         x_cycle     = 0;                                                             <* 
       *>    int         x_step      = 0;                                                             <* 
       *>    int         x_leg       = 0;                                                             <* 
       *>    char        x_request   [LEN_LABEL];                                                     <* 
       *>    char        x_verb      [LEN_LABEL] = "ik_from";                                         <* 
       *>    char        x_style     [LEN_LABEL];                                                     <* 
       *>    double      x_xcur      = 0.0;      /+ base x position                     +/            <* 
       *>    double      x_zcur      = 0.0;      /+ base z position                     +/            <* 
       *>    double      x_deg       = 0.0;      /+ relative degrees around circle      +/            <* 
       *>    double      x_acur      = 0.0;      /+ current leg angle                   +/            <* 
       *>    double      x_rcur      = 0.0;      /+ current leg radius from turn center +/            <* 
       *>    double      x_xorig     =   0.0;                                                         <* 
       *>    double      x_zorig     =   0.0;                                                         <* 
       *>    double      x_xnew      = 0.0;      /+ new  x position                     +/            <* 
       *>    double      x_znew      = 0.0;      /+ new  z position                     +/            <* 
       *>    /+---(header)-------------------------+/                                                 <* 
       *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                          <* 
       *>    /+---(prepare)------------------------+/                                                 <* 
       *>    rc = ykine__scrp_prep      ();                                                           <* 
       *>    ykine_gait__begin    (0);                                                                 <* 
       *>    /+---(loop repeats)-------------------+/                                                 <* 
       *>    for (x_cycle = 0; x_cycle < a_repeats; ++x_cycle) {                                      <* 
       *>       for (x_step = 0; x_step < 12; ++x_step) {                                             <* 
       *>          for (x_leg = 0; x_leg < 6; ++x_leg) {                                              <* 
       *>             /+---(prepare)---------------+/                                                 <* 
       *>             rc = ykine__scrp_prep      ();                                                  <* 
       *>             rc = ykine_parse_prep     (x_verb);                                             <* 
       *>             if (rc < 0) {                                                                   <* 
       *>                DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                               <* 
       *>                return rc;                                                                   <* 
       *>             }                                                                               <* 
       *>             /+---(identify servos)-------+/                                                 <* 
       *>             switch (x_leg) {                                                                <* 
       *>             case 0 : strlcpy   (x_request, "RR.femu", LEN_LABEL);  break;                   <* 
       *>             case 1 : strlcpy   (x_request, "RM.femu", LEN_LABEL);  break;                   <* 
       *>             case 2 : strlcpy   (x_request, "RF.femu", LEN_LABEL);  break;                   <* 
       *>             case 3 : strlcpy   (x_request, "LF.femu", LEN_LABEL);  break;                   <* 
       *>             case 4 : strlcpy   (x_request, "LM.femu", LEN_LABEL);  break;                   <* 
       *>             case 5 : strlcpy   (x_request, "LR.femu", LEN_LABEL);  break;                   <* 
       *>             }                                                                               <* 
       *>             myKINE.s_count = ykine_servos (x_request);                                      <* 
       *>             DEBUG_YKINE_SCRP  yLOG_info    ("x_request" , x_request);                       <* 
       *>             DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);                  <* 
       *>             /+---(set degree)---------------+/                                              <* 
       *>             switch (x_step) {                                                               <* 
       *>             case  0 :  x_deg  =   0.00;  myKINE.s_ypos = 25.0;  break;                      <* 
       *>             case  1 :  x_deg  =  -4.25;  myKINE.s_ypos = 15.0;  break;                      <* 
       *>             case  2 :  x_deg  =  -4.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  3 :  x_deg  =  -3.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  4 :  x_deg  =  -2.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  5 :  x_deg  =  -1.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  6 :  x_deg  =   0.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  7 :  x_deg  =   1.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  8 :  x_deg  =   2.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case  9 :  x_deg  =   3.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case 10 :  x_deg  =   4.00;  myKINE.s_ypos =  0.0;  break;                      <* 
       *>             case 11 :  x_deg  =   4.25;  myKINE.s_ypos = 15.0;  break;                      <* 
       *>             }                                                                               <* 
       *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_deg"     , x_deg);                           <* 
       *>             /+> x_deg /= 1.5;                                                         <+/   <* 
       *>             x_deg *= -1.5;                                                                  <* 
       *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_deg"     , x_deg);                           <* 
       *>             /+---(get current)--------------------+/                                        <* 
      *>             /+> x_xcur  = g_servo_info [(x_leg * 3) + 2].xsave;                       <*    <* 
      *>              *> x_zcur  = g_servo_info [(x_leg * 3) + 2].zsave;                       <+/   <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_xcur"    , x_xcur);                          <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_zcur"    , x_zcur);                          <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_xorig"   , x_xorig);                         <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_zorig"   , x_zorig);                         <* 
      *>             x_xcur += x_xorig;                                                              <* 
      *>             x_zcur += x_zorig;                                                              <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_xcur"    , x_xcur);                          <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_zcur"    , x_zcur);                          <* 
      *>             /+---(calc new)-----------------------+/                                        <* 
      *>             x_rcur  = sqrt ((x_xcur * x_xcur) + (x_zcur * x_zcur));                         <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_rcur"    , x_rcur);                          <* 
      *>             x_acur  = atan2 (x_xcur , x_zcur);                                              <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_acur"    , x_acur);                          <* 
      *>             /+> x_acur  = asin  (x_xcur / x_rcur);                                    <+/   <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_acur"    , x_acur);                          <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_acur (d)", x_acur * RAD2DEG);                <* 
      *>             x_deg  += x_acur * RAD2DEG;                                                     <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_deg"     , x_deg);                           <* 
      *>             x_xnew  = x_rcur * sin (x_deg * DEG2RAD);                                       <* 
      *>             x_znew  = x_rcur * cos (x_deg * DEG2RAD);                                       <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_xnew"    , x_xnew);                          <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("x_znew"    , x_znew);                          <* 
      *>             myKINE.s_xpos  = x_xcur - x_xnew;                                               <* 
      *>             myKINE.s_zpos  = x_zcur - x_znew;                                               <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_xpos"    , myKINE.s_xpos);            <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_zpos"    , myKINE.s_zpos);            <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_ypos"    , myKINE.s_ypos);            <* 
      *>             /+---(fill common fields)-------------+/                                        <* 
      *>             myKINE.s_secs = 0.100;                                                          <* 
      *>             DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_secs"    , myKINE.s_secs);            <* 
      *>             /+---(check)--------------------------+/                                        <* 
      *>             /+> rc = ykine_parse_check   ();                                          <*    <* 
      *>              *> if (rc < 0) {                                                         <*    <* 
         *>              *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                     <*    <* 
            *>              *>    return rc;                                                         <*    <* 
            *>              *> }                                                                     <+/   <* 
            *>             /+---(handle IK)----------------------+/                                        <* 
            *>             rc = ykine_scrp_ik_from   ();                                                   <* 
            *>          }                                                                                  <* 
            *>       }                                                                                     <* 
            *>    }                                                                                        <* 
            *>    rc = ykine__scrp_prep      ();                                                           <* 
            *>    ykine_gait_update   (0);                                                                 <* 
            *>    /+---(complete)-----------------------+/                                                 <* 
            *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                          <* 
            *>    return 0;                                                                                <* 
            *> }                                                                                           <*/



