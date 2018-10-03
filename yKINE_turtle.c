/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
typedef     struct      cTURTLE     tTURTLE;
struct      cTURTLE {
   float       deg;
   float       xpos;
   float       zpos;
   float       ypos;
};

static      tTURTLE     s_turtles;
static      int         s_nturtle   =    0;


static      tSERVO     *s_servo     = NULL;
static      float       s_speed     = 10.0;
static      float       s_deg       =  0.0;



static      float       s_x         =  0.0;
static      float       s_z         =  0.0;
static      float       s_y         =  0.0;
static      float       s_xz        =  0.0;




/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char
ykine_turtle_prep       (void)
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
   rc  = ykine_queue_popval   (s_speed, &s);
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
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get existing values)------------*/
   rc = ykine_turtle_prep ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get timing)------------------*/
   b  = s_xz / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_lines, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%3.1f, 0.0, 0.0, =)", b);
   rc = ykine_parse_hidden (x_recd);
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
   rc = ykine_turtle_prep ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = ykine_queue_popval   (s_deg, &d);
   if (d <    0.0)   d  = 360.0 + d;
   if (d >  360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   /*---(update turtle)---------------*/
   s_deg  = d;
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_lines, 0.0, 0.0);
   if (rc == 0)  rc = ykine_move_addloc (s_servo, s_x, s_z, s_y);
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
   rc = ykine_turtle_prep ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = ykine_queue_popfrom  (s_deg, &d);
   if (d <    0.0)   d  = 360.0 + d;
   if (d >  360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_value   ("d"         , d);
   /*---(update turtle)---------------*/
   s_deg  = d;
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_lines, 0.0, 0.0);
   if (rc == 0)  rc = ykine_move_addloc (s_servo, s_x, s_z, s_y);
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
   rc = ykine_turtle_prep ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get distance)----------------*/
   rc  = ykine_queue_popval   (0.0, &l);
   DEBUG_YKINE_SCRP  yLOG_value   ("l"         , l);
   /*---(calc coordinates)------------*/
   x =  l * cos (s_deg * DEG2RAD);
   z =  l * sin (s_deg * DEG2RAD);
   /*> if      (s_deg <=  90.0)  z *= -1;                                             <* 
    *> else if (s_deg <= 180.0)  x *= -1;                                             <* 
    *> else if (s_deg <= 270.0)  z *= -1;                                             <* 
    *> else                      x *= -1;                                             <*/
   /*---(get timing)------------------*/
   xz = sqrt ((x * x) + (z * z));
   b  = xz / s_speed;
   s = b * s_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   /*---(save header)-----------------*/
   rc = ykine_move_create (YKINE_MOVE_SERVO, s_servo, myKINE.s_verb, myKINE.s_lines, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   /*---(queue up action)-------------*/
   sprintf (x_recd, "ze_pure (%6.1f, %6.1f, %6.1f, =)", b, s_x + x, s_z + z);
   rc = ykine_parse_hidden (x_recd);
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


