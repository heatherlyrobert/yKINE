/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



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
static      float       s_speed     = 10.0;
static      float       s_head      =  0.0;



static      float       s_x         =  0.0;
static      float       s_z         =  0.0;
static      float       s_y         =  0.0;
static      float       s_xz        =  0.0;




/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char
ykine__turtle_last      (void)
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
   rc = ykine_move_savedloc  (s_servo, NULL, NULL, &s_x, &s_z, &s_y, &s_xz);
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
/*===----                    run-time configuration                    ----===*/
/*====================------------------------------------====================*/
static void      o___CONFIG__________________o (void) {;}

char
ykine_turtle_speed      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       s           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get duration)----------------*/
   rc  = yPARSE_popval   (s_speed, &s);
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(check range)-----------------*/
   if (s <   5.0)  s =   5.0;
   if (s > 100.0)  s = 100.0;
   /*---(save)------------------------*/
   s_speed = s;
   DEBUG_YKINE_SCRP  yLOG_value   ("s_speed"   , s_speed);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_wait       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b, s        =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get timing)------------------*/
   rc  = yPARSE_popval   (0.0, &b);
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       turtle movements                       ----===*/
/*====================------------------------------------====================*/
static void      o___MOVES___________________o (void) {;}

char
ykine_turtle_home       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b, s        =  0.0;
   char        x_recd      [LEN_RECD];
   float       xz          =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get timing)------------------*/
   xz = sqrt ((s_x * s_x) + (s_z * s_z));
   b  = xz / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%3.1f, 0.0, 0.0, =)", b);
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP  yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_scrp_zero ();
   DEBUG_YKINE_SCRP  yLOG_point   ("run"       , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_goto       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b, s        =  0.0;
   char        x_recd      [LEN_RECD];
   float       x, z, xz;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get location)----------------*/
   rc  = yPARSE_popval   (s_x , &x);
   rc  = yPARSE_popval   (-s_z, &z);
   z *= -1;
   /*---(get timing)------------------*/
   xz = sqrt (((x - s_x) * (x - s_x)) + ((z - s_z) * (z - s_z)));
   b  = xz / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%3.1f, %6.1f, %6.1f, =)", b, x, z);
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP  yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_scrp_zero ();
   DEBUG_YKINE_SCRP  yLOG_point   ("run"       , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_raise      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b, s        =  0.0;
   char        x_recd      [LEN_RECD];
   float       y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get timing)------------------*/
   y  = 10.0;
   b  = (y - s_y) / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%3.1f, =, =, %6.1f)", b, y);
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP  yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_scrp_zero ();
   DEBUG_YKINE_SCRP  yLOG_point   ("run"       , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_lower      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b, s        =  0.0;
   char        x_recd      [LEN_RECD];
   float       y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get timing)------------------*/
   y  = 0.0;
   b  = (s_y - y) / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%3.1f, =, =, %6.1f)", b, y);
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP  yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_scrp_zero ();
   DEBUG_YKINE_SCRP  yLOG_point   ("run"       , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_head       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   float       d           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = yPARSE_popval   (s_head, &d);
   if (d <    0.0)   d  = 360.0 + d;
   if (d >  360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   /*---(update turtle)---------------*/
   s_head  = d;
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, 0.0);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_turn       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   float       d           =  0.0;
   float       xp, zp, yp;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = yPARSE_popfrom  (s_head, &d);
   if (d <    0.0)   d  = 360.0 + d;
   if (d >  360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   /*---(update turtle)---------------*/
   s_head  = d;
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, 0.0);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_turtle_move       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   float       l           =  0.0;
   float       x, z, xz;
   float       b, s;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine__turtle_last ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get distance)----------------*/
   rc  = yPARSE_popval   (0.0, &l);
   DEBUG_YKINE_SCRP  yLOG_value   ("l"         , l);
   /*---(calc coordinates)------------*/
   x =  l * cos ((s_head - 90.0) * DEG2RAD);
   z =  l * sin ((s_head - 90.0) * DEG2RAD);
   /*---(get timing)------------------*/
   xz = sqrt ((x * x) + (z * z));
   b  = xz / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_cline, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%6.1f, %6.1f, %6.1f, =)", b, s_x + x, s_z + z);
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP  yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_scrp_zero ();
   DEBUG_YKINE_SCRP  yLOG_point   ("run"       , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_turtle      (char *a_question, int a_num)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "TURTLE unit      : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if (strcmp (a_question, "global"  ) == 0) {
      sprintf (ykine__unit_answer, "TURTLE global  : speed %8.2f mmps, heading %8.2f deg", s_speed, s_head);
   }
   else if (strcmp (a_question, "stack"   ) == 0) {
      sprintf (ykine__unit_answer, "TURTLE stack   : total %3d, curr %3d", s_nturtle, s_cturtle);
   }
   else if (strcmp (a_question, "position") == 0) {
      sprintf (ykine__unit_answer, "TURTLE pos     : %8.2fx, %8.2fz, %8.2fy", s_x, s_z, s_y);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}


