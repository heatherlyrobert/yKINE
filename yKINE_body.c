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
   else if (a_deg <= 190.0)  x *= -1;
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
   float       x, y, z;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get values)------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("saves"     , "%8.2fx, %8.2fz, %8.2fy, %8.2fd, %8.2fl", x_servo->xsave, x_servo->zsave, x_servo->ysave, x_servo->dsave, x_servo->lsave);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->xsave, &x);
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->zsave, &z);
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->ysave, &y);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->xsave, &x);
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->zsave, &z);
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->ysave, &y);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
   /*---(process moves)---------------*/
   s = b * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, x, z, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(caclulate polar)-------------*/
   rc = ykine__scrp_zero_deg  (x, z, &d, &l);
   DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = x;
   x_servo->zsave  = z;
   x_servo->ysave  = y;
   x_servo->dsave  = d;
   x_servo->lsave  = l;
   DEBUG_YKINE_SCRP  yLOG_complex ("saved"     , "%8.2fx, %8.2fz, %8.2fy, %8.2fd, %8.2fl", x_servo->xsave, x_servo->zsave, x_servo->ysave, x_servo->dsave, x_servo->lsave);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_polar        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d           =  0.0;
   float       l           =  0.0;
   float       b, s        =  0.0;
   float       x, y, z;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get values)------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("saves"     , "%8.2fx, %8.2fz, %8.2fy, %8.2fd, %8.2fl", x_servo->xsave, x_servo->zsave, x_servo->ysave, x_servo->dsave, x_servo->lsave);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->dsave, &d);
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->lsave, &l);
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->ysave, &y);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->dsave, &d);
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->lsave, &l);
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->ysave, &y);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);
   /*---(caclulate end)---------------*/
   rc = ykine__scrp_zero_pos  (d, l, &x, &z);
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
   /*---(process moves)---------------*/
   s = b * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, 0.0, s);
   DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, x, z, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = x;
   x_servo->zsave  = z;
   x_servo->ysave  = y;
   x_servo->dsave  = d;
   x_servo->lsave  = l;
   DEBUG_YKINE_SCRP  yLOG_complex ("saved"     , "%8.2fx, %8.2fz, %8.2fy, %8.2fd, %8.2fl", x_servo->xsave, x_servo->zsave, x_servo->ysave, x_servo->dsave, x_servo->lsave);
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
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get values)------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_popval   (x_servo->dsave, &d);
   } else {
      if (rc == 0)  rc  = ykine_queue_popfrom  (x_servo->dsave, &d);
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
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, a_deg, s);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = 0.0;
   x_servo->zsave  = 0.0;
   x_servo->ysave  = 0.0;
   x_servo->dsave  = a_deg;
   x_servo->lsave  = 0.0;
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
   /*---(speed)--------------------------*/
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

