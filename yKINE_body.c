/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                     zero-point movements                     ----===*/
/*====================------------------------------------====================*/
static void      o___ZERO____________________o (void) {;}

char
ykine__scrp_zero_deg    (float a_xpos, float a_zpos, float *a_deg, float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   float       r           = 0.0;
   float       d           = 0.0;
   float       l           = 0.0;
   r = atan2f (a_xpos, -a_zpos);
   d = r * RAD2DEG;
   l = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   if (d <    0.0)   d  = 360.0 + d;
   if (d >  360.0)   d  = d - 360.0;
   if (l ==   0.0)   d  = 0.0;
   if (a_deg  != NULL)  *a_deg  = d;
   if (a_len  != NULL)  *a_len  = l;
   return 0;
}

char
ykine__scrp_zero_pos    (float a_deg, float a_len, float *a_xpos, float *a_zpos)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x           = 0.0;
   float       z           = 0.0;
   if (a_deg <    0.0)   a_deg  = 360.0 + a_deg;
   if (a_deg >  360.0)   a_deg  = a_deg - 360.0;
   x =  a_len * cos (a_deg * DEG2RAD);
   z =  a_len * sin (a_deg * DEG2RAD);
   if      (a_deg <=  90.0)  z *= -1;
   else if (a_deg <= 180.0)  x *= -1;
   else if (a_deg <= 270.0)  z *= -1;
   else                      x *= -1;
   if (a_xpos != NULL)  *a_xpos = x;
   if (a_zpos != NULL)  *a_zpos = z;
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_zero         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d           =  0.0;
   float       l           =  0.0;
   float       b, s        =  0.0;
   float       x , z , y;                        /* current coordinates       */
   float       xp, zp, yp;                       /* previous coordinates      */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(det coordinates)-------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (xp, &x);
      if (rc == 0)  rc  = ykine_queue_popval   (zp, &z);
      if (rc == 0)  rc  = ykine_queue_popval   (yp, &y);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (xp, &x);
      if (rc == 0)  rc  = ykine_queue_popfrom  (zp, &z);
      if (rc == 0)  rc  = ykine_queue_popfrom  (yp, &y);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
   /*---(caclulate polar)-------------*/
   rc = ykine__scrp_zero_deg  (x, z, &d, &l);
   DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);
   /*---(process moves)---------------*/
   s = b * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   if (myKINE.s_hidden != 'y')  rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_cline, d, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   if (rc == 0)  rc = ykine_move_addloc (x_servo, x, z, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_zpolar       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d           =  0.0;
   float       l           =  0.0;
   float       b, s        =  0.0;
   float       x , z , y;                        /* current coordinates       */
   float       dp, lp, yp;                       /* previous coordinates      */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(det coordinates)-------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, &dp, NULL, NULL, &yp, &lp);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (dp, &d);
      if (rc == 0)  rc  = ykine_queue_popval   (lp, &l);
      if (rc == 0)  rc  = ykine_queue_popval   (yp, &y);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (dp, &d);
      if (rc == 0)  rc  = ykine_queue_popfrom  (lp, &l);
      if (rc == 0)  rc  = ykine_queue_popfrom  (yp, &y);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (d <    0.0)   d  = 360.0 + d;
   if (d >  360.0)   d  = d - 360.0;
   DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);
   /*---(caclulate end)---------------*/
   rc = ykine__scrp_zero_pos  (d, l, &x, &z);
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
   /*---(process moves)---------------*/
   s = b * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_cline, d, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   if (rc == 0)  rc = ykine_move_addloc (x_servo, x, z, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     orientation movements                    ----===*/
/*====================------------------------------------====================*/
static void      o___ORIENT__________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_orient_adjust   (int a_seg, float *a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d           =  0.0;
   float       dp          =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(det coordinates)-------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, &dp, NULL, NULL, NULL, NULL);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (dp, &d);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (dp, &d);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("deg"       , d);
   /*---(return)-------------------------*/
   *a_deg = d;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_orient_servo    (int a_seg, float a_deg, float a_beat)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * x_servo->pace;
   if (rc == 0)  rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_cline, a_deg, s);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_orient       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       b           =  0.0;
   float       f, p, t;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(timing)-------------------------*/
   rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(adjust angles)---------------*/
   rc = ykine__scrp_orient_adjust (YKINE_YAW  , &f);
   rc = ykine__scrp_orient_adjust (YKINE_PITCH, &p);
   rc = ykine__scrp_orient_adjust (YKINE_ROLL , &t);
   /*---(process moves)---------------*/
   rc = ykine__scrp_orient_servo  (YKINE_YAW  , f, b);
   rc = ykine__scrp_orient_servo  (YKINE_PITCH, p, b);
   rc = ykine__scrp_orient_servo  (YKINE_ROLL , t, b);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_opolar       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d           =  0.0;
   float       b           =  0.0;
   float       x , z , y;                        /* current coordinates       */
   float       dp, lp, yp;                       /* previous coordinates      */
   float       p, t;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get duration)----------------*/
   rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(det coordinates)-------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, &dp, NULL, NULL, &yp, &lp);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (dp, &d);
      if (rc == 0)  rc  = ykine_queue_popval   (yp, &y);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (dp, &d);
      if (rc == 0)  rc  = ykine_queue_popfrom  (yp, &y);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("deg/y"     , "%8.2fd, %8.2fy", d, y);
   /*---(caclulate end)---------------*/
   rc = ykine__scrp_zero_pos  (d, 1.0, &x, &z);
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
   p  = x * y;
   t  = z * y;
   if      (d <=  90.0)  t *= -1.0;
   else if (d <= 180.0)  p *= -1.0;
   else if (d <= 270.0)  t *= -1.0;
   else                  p *= -1.0;
   DEBUG_YKINE_SCRP  yLOG_complex ("orient"    , "%8.2fp, %8.2ft", p, t);
   /*---(process moves)---------------*/
   rc = ykine__scrp_orient_servo  (YKINE_YAW  , 0.0, b);
   rc = ykine__scrp_orient_servo  (YKINE_PITCH, p  , b);
   rc = ykine__scrp_orient_servo  (YKINE_ROLL , t  , b);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



