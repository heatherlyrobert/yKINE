/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


/*===[[ DESIGN NOTES ]]=======================================================*/
/*
 * turtle movements are always accelerated with "[¢]" where ¢ is in tsmfx.
 *   later, we should likely allow more complex specification, but not now.
 *
 *
 *
 *
 *
 *
 *
 */


/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
#define     MAX_TURTLE  10
typedef     struct      cTURTLE     tTURTLE;
struct      cTURTLE {
   float       deg;
   float       xpos;
   float       zpos;
   float       ypos;
};

static      tTURTLE     s_turtles   [MAX_TURTLE];
static      int         s_nturtle   =    0;
static      int         s_cturtle   =    0;


static      tSERVO     *s_servo     = NULL;
static      char        s_speed     =  'm';
static      char        s_accel     =  '[';
static      char        s_decel     =  ']';
static      float       s_head      =  0.0;
static      float       s_depth     =  0.0;



static      float       s_x         =  0.0;
static      float       s_z         =  0.0;
static      float       s_y         =  0.0;
static      float       s_xz        =  0.0;




/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char
ykine_turtle__reset     (void)
{
   s_accel = '[';
   s_decel = ']';
   sprintf (myKINE.accel, "%c%c%c", s_accel, s_speed, s_decel);
   myKINE.b = -1.0;
   return 0;
}

char
ykine_turtle_init       (void)
{
   s_speed = 'm';
   ykine_turtle__reset ();
   s_head  = 0.0;
   s_depth = 0.0;
   return 0;
}

char
ykine_turtle__last      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   s_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("s_servo"   , s_servo);
   --rce;  if (s_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get distance)----------------*/
   rc = ykine_move_savedtail  (s_servo, NULL, NULL, &s_x, &s_z, &s_y, &s_xz);
   DEBUG_YKINE_SCRP  yLOG_value   ("saved"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     min stack for saves                      ----===*/
/*====================------------------------------------====================*/
static void      o___STACK___________________o (void) {;}

char
ykine_turtle_push       (void)
{
}

char
ykine_turtle_pop        (void)
{
}



/*====================------------------------------------====================*/
/*===----                    configuration/settings                    ----===*/
/*====================------------------------------------====================*/
static void      o___CONFIG__________________o (void) {;}

char
ykine_turtle_speed      (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char        s           =  'm';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   rc = ykine_turtle__reset ();
   /*---(get duration)----------------*/
   rc  = yPARSE_popchar  (&s);
   DEBUG_YKINE_SCRP  yLOG_char    ("s"         , s);
   /*---(defense)---------------------*/
   --rce;  if (s == 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("speed null, error out");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr ("tsmfxTSMFX", s) == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("unknown speed, error out");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)------------------------*/
   s_speed = s;
   DEBUG_YKINE_SCRP  yLOG_char    ("s_speed"   , s_speed);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_head       (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      d           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(beg and end points)-------------*/
   rc = ykine_turtle__last  ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   DEBUG_YKINE_SCRP  yLOG_value   ("s_head"    , s_head);
   rc  = yPARSE_popval   (s_head, &d);
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   if (d <    0.0)   d  = 360.0 + d;
   if (d >= 360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   /*---(update turtle)---------------*/
   s_head  = d;
   /*> rc = ykine_move_create (s_servo, YKINE_SERVO, YKINE_NONE, myKINE.s_tline, myKINE.s_verb, '-', YKINE_NONE, 0.0, 0.0);   <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_turn       (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      d           =  0.0;
   float       xp, zp, yp;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine_turtle__last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   DEBUG_YKINE_SCRP  yLOG_value   ("s_head"    , s_head);
   rc  = yPARSE_popfrom  (s_head, &d);
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   if (d <    0.0)   d  = 360.0 + d;
   if (d >= 360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   /*---(update turtle)---------------*/
   s_head  = d;
   /*> rc = ykine_move_create (s_servo, YKINE_SERVO, YKINE_NONE, myKINE.s_tline, myKINE.s_verb, '-', YKINE_NONE, 0.0, 0.0);   <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       turtle movements                       ----===*/
/*====================------------------------------------====================*/
static void      o___MOVES___________________o (void) {;}

char
ykine_turtle__zero      (int n, uchar *v, char a_verb, float x, float z, float y)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(create record)------------------*/
   DEBUG_YKINE_SCRP   yLOG_value   ("a_verb"    , a_verb);
   switch (a_verb) {
   case YKINE_TMV :
      sprintf (x_recd, "zero    (%s, %6.1fr, %6.1fr, =)", myKINE.accel, x, z);
      break;
   case YKINE_THM :
      sprintf (x_recd, "zero    (%s, 0.0, 0.0, =)"      , myKINE.accel);
      break;
   case YKINE_TGO :
      sprintf (x_recd, "zero    (%s, %6.1f, %6.1f, =)"  , myKINE.accel, x, z);
      break;
   case YKINE_TRA : case YKINE_TLO :
      sprintf (x_recd, "zero    (%s, 0.0r, 0.0r, %6.1f)"  , myKINE.accel, y);
      break;
   case YKINE_TWA :
      sprintf (x_recd, "zero    (%6.1f, =, =, =)"  , x);
      break;
   }
   DEBUG_YKINE_SCRP   yLOG_info    ("x_recd"    , x_recd);
   /*---(parse)--------------------------*/
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP   yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_body_zero (n, v);
   DEBUG_YKINE_SCRP  yLOG_point   ("zero"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_move       (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      l           =  0.0;
   float       x, z;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   ykine_turtle__reset ();
   /*---(calc relative)---------------*/
   rc  = yPARSE_popval   (0.0, &l);
   DEBUG_YKINE_SCRP  yLOG_value   ("l"         , l);
   x = l * cos ((s_head - 90.0) * DEG2RAD);
   z = l * sin ((s_head - 90.0) * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("change"    , "%6.1fl, %6.1fx, %6.1fz", l, x, z);
   /*---(call move)-------------------*/
   rc = ykine_turtle__zero (n, v, YKINE_TMV, x, z, 0.0);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_home       (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   rc = ykine_turtle__reset ();
   /*---(call move)-------------------*/
   rc = ykine_turtle__zero (n, v, YKINE_THM, 0.0, 0.0, 0.0);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_goto       (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b, s        =  0.0;
   double      x, z;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   rc = ykine_turtle__reset ();
   /*---(get location)----------------*/
   rc  = yPARSE_popval   (0.0, &x);
   rc  = yPARSE_popval   (0.0, &z);
   /*---(call move)-------------------*/
   rc = ykine_turtle__zero (n, v, YKINE_TGO, x, -z, 0.0);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_beg        (void)
{
   s_accel = '[';
   s_decel = '>';
   return 0;
}

char
ykine_turtle_con        (void)
{
   s_accel = '<';
   s_decel = '>';
   return 0;
}

char
ykine_turtle_end        (void)
{
   s_accel = '<';
   s_decel = ']';


   /*---(reset acceleration)----------*/
   ykine_turtle__reset ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       non-move moves                         ----===*/
/*====================------------------------------------====================*/
static void      o___NON_MOVE________________o (void) {;}

char
ykine_turtle_wait       (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      b           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   rc = ykine_turtle__reset ();
   /*---(get timing)------------------*/
   rc  = yPARSE_popval   (0.0, &b);
   /*---(call move)-------------------*/
   rc = ykine_turtle__zero (n, v, YKINE_TWA, b, 0.0, 0.0);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      router/beak moves                       ----===*/
/*====================------------------------------------====================*/
static void      o___BEAK____________________o (void) {;}


char
ykine_turtle_raise      (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   rc = ykine_turtle__reset ();
   /*---(call move)-------------------*/
   rc = ykine_turtle__zero (n, v, YKINE_TRA, 0.0, 0.0, 10.0);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_lower      (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(reset acceleration)----------*/
   rc = ykine_turtle__reset ();
   /*---(call move)-------------------*/
   rc = ykine_turtle__zero (n, v, YKINE_TLO, 0.0, 0.0, s_depth);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_depth      (int n, char *v)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      d           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get duration)----------------*/
   rc  = yPARSE_popval   (s_depth, &d);
   DEBUG_YKINE_SCRP  yLOG_double  ("d"         , d);
   /*---(defense)---------------------*/
   --rce;  if (d >   5.0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("depth too high");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (d <  -20.0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("depth too low");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)------------------------*/
   s_depth = d;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_depth"   , s_depth);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine_turtle__unit      (char *a_question, int a_num)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_RECD];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "TURTLE unit      : question not understood", LEN_RECD);
   /*---(answer)------------------------------------------*/
   if (strcmp (a_question, "global"  ) == 0) {
      sprintf (ykine__unit_answer, "TURTLE global  : speed [%c], heading %8.2f deg", s_speed, s_head);
   }
   else if (strcmp (a_question, "stack"   ) == 0) {
      sprintf (ykine__unit_answer, "TURTLE stack   : total %3d, curr %3d", s_nturtle, s_cturtle);
   }
   else if (strcmp (a_question, "position") == 0) {
      sprintf (ykine__unit_answer, "TURTLE pos     : %8.2fx, %8.2fz, %8.2fy", s_x, s_z, s_y);
   }
   else if (strcmp (a_question, "speed"   ) == 0) {
      sprintf (ykine__unit_answer, "TURTLE speed   : [%c]", s_speed);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}


