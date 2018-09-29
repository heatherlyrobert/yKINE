/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                       body movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___BODY_MOVES______________o (void) {;}

char
ykine__scrp_zero_deg    (float a_xpos, float a_zpos, float *a_deg, float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   float       r           = 0.0;
   float       d           = 0.0;
   float       l           = 0.0;
   r = atan2f (a_xpos, a_zpos);
   if (r < 0.0)  r = (2 * M_PI) - r;
   d = r * RAD2DEG;
   l = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
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
   x = a_len * cos (a_deg * DEG2RAD);
   z = a_len * sin (a_deg * DEG2RAD);
   if (a_xpos != NULL)  *a_xpos = x;
   if (a_zpos != NULL)  *a_zpos = z;
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_zero_adjust (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_seg       =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from != YKINE_FROM) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(x-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("xsave"     , x_servo->xsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos"    , myKINE.s_xpos);
   myKINE.s_xpos += x_servo->xsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos new", myKINE.s_xpos);
   /*---(z-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("zsave"     , x_servo->zsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos"    , myKINE.s_zpos);
   myKINE.s_zpos += x_servo->zsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos new", myKINE.s_zpos);
   /*---(y-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("ysave"     , x_servo->ysave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos"    , myKINE.s_ypos);
   myKINE.s_ypos += x_servo->ysave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos new", myKINE.s_ypos);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_zero         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   tSERVO     *x_servo     =    0;
   float       x_deg       =  0.0;
   float       x_len       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   x_leg = YKINE_BODY;
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjust positions)------------*/
   rc = ykine__scrp_zero_adjust ();
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process moves)---------------*/
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, 0.0, myKINE.s_secs);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(caclulate polar)-------------*/
   rc = ykine__scrp_zero_deg  (myKINE.s_xpos, myKINE.s_zpos, &x_deg, &x_len);
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = myKINE.s_xpos;
   x_servo->zsave  = myKINE.s_zpos;
   x_servo->ysave  = myKINE.s_ypos;
   x_servo->dsave  = x_deg;
   x_servo->lsave  = x_len;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_zero_polar   (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   tSERVO     *x_servo     =    0;
   float       x_deg       =  0.0;
   float       x_len       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   x_leg = YKINE_BODY;
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(caclulate end)---------------*/
   x_deg = myKINE.s_xpos - 90.0;
   x_len = myKINE.s_zpos;
   rc = ykine__scrp_zero_pos  (x_deg, x_len, &myKINE.s_xpos, &myKINE.s_zpos);
   /*---(process moves)---------------*/
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, 0.0, myKINE.s_secs);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = myKINE.s_xpos;
   x_servo->zsave  = myKINE.s_zpos;
   x_servo->ysave  = myKINE.s_ypos;
   x_servo->dsave  = x_deg;
   x_servo->lsave  = x_len;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_orient_adjust   (int a_seg, float *a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from != YKINE_FROM) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(from)---------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("dsave"     , x_servo->dsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("a_deg"     , *a_deg);
   *a_deg += x_servo->dsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("a_deg new" , *a_deg);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_orient_servo    (int a_seg, float a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
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
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, a_deg, myKINE.s_secs);
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
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_orient       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(adjust angles)---------------*/
   rc = ykine__scrp_orient_adjust (YKINE_YAW  , &myKINE.s_femu);
   rc = ykine__scrp_orient_adjust (YKINE_PITCH, &myKINE.s_pate);
   rc = ykine__scrp_orient_adjust (YKINE_ROLL , &myKINE.s_tibi);
   /*---(process moves)---------------*/
   rc = ykine__scrp_orient_servo  (YKINE_YAW  , myKINE.s_femu);
   rc = ykine__scrp_orient_servo  (YKINE_PITCH, myKINE.s_pate);
   rc = ykine__scrp_orient_servo  (YKINE_ROLL , myKINE.s_tibi);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}
