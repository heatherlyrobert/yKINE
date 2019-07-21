/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*
 * previous values saved at start as a fallback in case of failure
 */

static float    cp    =  0.0;    /* coxa previous                             */
static float    fp    =  0.0;    /* femu previous                             */
static float    pp    =  0.0;    /* pate previous                             */
static float    tp    =  0.0;    /* tibi previous                             */

static float    xp    =  0.0;    /* xpos previous                             */
static float    zp    =  0.0;    /* zpos previous                             */
static float    xzp   =  0.0;    /* xz dist previous                          */
static float    yp    =  0.0;    /* ypos previous                             */



/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__legs_get_prev    (int a_leg)
{
   /*---(locals)-----------+-----+-----+-*/
   tSERVO     *x_servo     =    0;
   char        rc          =    0;
   /*---(get previous)-------------------*/
   cp  = yKINE_legdeg (a_leg);
   x_servo = ykine_servo_pointer (a_leg, YKINE_FEMU);
   rc  = ykine_move_savedloc  (x_servo, NULL, &fp, NULL, NULL, NULL, NULL);
   x_servo = ykine_servo_pointer (a_leg, YKINE_PATE);
   rc  = ykine_move_savedloc  (x_servo, NULL, &pp, NULL, NULL, NULL, NULL);
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   rc  = ykine_move_savedloc  (x_servo, NULL, &tp, &xp, &zp, &yp, &xzp);
   /*---(adjust for foot)----------------*/
   yp -= yKINE_seglen (YKINE_FOOT);
   /*---(report)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("angles"    , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , cp, fp, pp, tp);
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", xp, zp, yp, xzp);
   /*---(complete)-----------------------*/
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__legs_set_servo   (char a_meth, int a_leg, int a_seg, float a_deg, float a_beat)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   float       x, y, z;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s_secs"    , s);
   if (rc == 0)  rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_cline, a_deg, s);
   if (rc == 0)  rc = yKINE_endpoint    (a_leg, a_seg, a_meth, NULL, NULL, &x, &z, &y);
   if (rc == 0)  rc = ykine_move_addloc (x_servo, x, z, y);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         fk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___FK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__legs_fk_getter       (int a_leg, int a_seg, char *a_entry, float *a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   double      d           =  0.0;
   float       dp          =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get values)------------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, &dp, NULL, NULL, NULL, NULL);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (dp, a_entry, &d);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (dp, a_entry, &d);
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

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_legs_fk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   double      b           =  0.0;
   int         x_leg       = 0.0;
   float       f, p, t;
   char        x_list      [LEN_HUND];
   char       *x_femu      [LEN_LABEL];
   char       *x_pate      [LEN_LABEL];
   char       *x_tibi      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = yPARSE_popstr    (x_femu);
   DEBUG_YKINE_SCRP  yLOG_info    ("x_femu"    , x_femu);
   if (rc >= 0)  rc = yPARSE_popstr    (x_pate);
   DEBUG_YKINE_SCRP  yLOG_info    ("x_pate"    , x_pate);
   if (rc >= 0)  rc = yPARSE_popstr    (x_tibi);
   DEBUG_YKINE_SCRP  yLOG_info    ("x_tibi"    , x_tibi);
   /*---(handle trouble)--------------*/
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("CHECK EACH LEG");
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_FEMU);
      if (x_list [rc] == '_')  continue;
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_FEMU, x_femu, &f);
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_PATE, x_pate, &p);
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_TIBI, x_tibi, &t);
      /*---(calculate endpoint)----------*/
      if (rc >= 0)  rc = yKINE_forward     (x_leg, f, p, t);
      DEBUG_YKINE_SCRP  yLOG_value   ("forward"   , rc);
      /*---(process moves)---------------*/
      if (rc >= 0)  rc = ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_FEMU, f, b);
      if (rc >= 0)  rc = ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_PATE, p, b);
      if (rc >= 0)  rc = ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___IK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__legs_ik_getter   (int a_leg, char *x_str, float *x, char *z_str, float *z, char *y_str, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       xp, zp, yp;                       /* previous coordinates      */
   double      xt, zt, yt;                       /* temporary coordinates     */
   float       x_thor      =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get existing)----------------*/
   x_thor = yKINE_legdeg (a_leg);
   rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
   yp -= yKINE_seglen (YKINE_FOOT);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (xp, x_str, &xt);
      if (rc == 0)  rc  = yPARSE_adjval   (zp, z_str, &zt);
      if (rc == 0)  rc  = yPARSE_adjval   (yp, y_str, &yt);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (xp, x_str, &xt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (zp, z_str, &zt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (yp, y_str, &yt);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
   /*---(save)---------------------------*/
   if (x != NULL)  *x = xt;
   if (z != NULL)  *z = zt;
   if (y != NULL)  *y = yt;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle inverse verbs ------------------[ ------ [ ------ ]-*/
ykine_legs_ik           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   double      b           =  0.0;
   float       x, z, y;
   float       f, p, t;
   char        x_list      [LEN_HUND];
   char       *x_str       [LEN_LABEL];
   char       *z_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   double      x_thor      =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = yPARSE_popstr    (x_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("x_str"     , x_str);
   if (rc >= 0)  rc = yPARSE_popstr    (z_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("z_str"     , z_str);
   if (rc >= 0)  rc = yPARSE_popstr    (y_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("y_str"     , y_str);
   /*---(handle trouble)--------------*/
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   --rce;
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (rc < 0)              continue;
      if (x_list [rc] == '_')  continue;
      /*---(get positions)---------------*/
      if (rc >= 0)  rc = ykine__legs_ik_getter (x_leg, x_str, &x, z_str, &z, y_str, &y);
      DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", x, z, y);
      /*---(inverse kinematics)----------*/
      if (rc >= 0)  rc = yKINE_inverse (x_leg, x, z, y);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rce;
      }
      /*---(retrieve angles)-------------*/
      if (rc >= 0)  rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &f, &p, &t);
      DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce + 1;
      }
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , f);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , p);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , t);
      /*---(process moves)---------------*/
      if (rc >= 0)  rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
      if (rc >= 0)  rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
      if (rc >= 0)  rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ck movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___CK_MOVES________________o (void) {;}
/*
 *  center polar kinematics are polar relative to the body center.  useful for
 *  turning and circling.
 *
 *   d  = from center relative to thorax
 *   xz = distance along that angle
 *   y  = height as usual
 *
 *  will support both ck_pure and ck_from
 *
 */

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine__legs_ck_getter   (int a_leg, char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   double      xt, zt, yt, dt, ot, tt, xz;       /* temporary coordinates     */
   float       x_full, x_diff;
   float       x_tang, x_out, x_out2, x_down;
   float       x_hadj, x_vadj;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(convert to ck)---------------*/
   x_full = atan2 (-zp, xp)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   x_diff = cp - x_full;
   x_hadj  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   x_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   x_out2 = xzp;
   x_out  = x_out2 - x_hadj;
   x_down = yp + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2f2, %8.2fo, %8.2fy", x_full, x_diff, x_out2, x_out, yp);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (x_diff, d_str, &dt);
      if (rc == 0)  rc  = yPARSE_adjval   (x_out , o_str, &ot);
      if (rc == 0)  rc  = yPARSE_adjval   (x_down, y_str, &yt);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_diff, d_str, &dt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_out , o_str, &ot);
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_down, y_str, &yt);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", dt, ot, yt);
   /*---(convert from cx)-------------*/
   ot    += x_hadj;
   x_full = cp - dt;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   xz     = ot;
   DEBUG_YKINE_SCRP  yLOG_complex ("fix"       , "%8.3fd, %8.3ff, %8.2fo, %8.2ft, %8.2fxz", dt, x_full, ot, tt, xz);
   zt     = -xz * sin (x_full * DEG2RAD);
   xt     =  xz * cos (x_full * DEG2RAD);
   yt    -= x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
   /*---(save)---------------------------*/
   if (x != NULL)  *x = xt;
   if (z != NULL)  *z = zt;
   if (y != NULL)  *y = yt;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle center verbs -------------------[ ------ [ ------ ]-*/
ykine_legs_ck           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   double      b           =  0.0;
   float       x, z, y;
   float       f, p, t;
   char        x_list      [LEN_HUND];
   char       *d_str       [LEN_LABEL];
   char       *o_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = yPARSE_popstr    (d_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("d_str"     , d_str);
   if (rc >= 0)  rc = yPARSE_popstr    (o_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("o_str"     , o_str);
   if (rc >= 0)  rc = yPARSE_popstr    (y_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("y_str"     , y_str);
   /*---(handle trouble)--------------*/
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (x_list [rc] == '_')  continue;
      /*---(get positions)---------------*/
      rc = ykine__legs_ck_getter (x_leg, d_str, o_str, y_str, &x, &z, &y);
      DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         x = xp;
         z = zp;
         y = yp;
      }
      DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", x, z, y);
      /*---(inverse kinematics)----------*/
      rc = yKINE_inverse (x_leg, x, z, y);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      /*---(retrieve angles)-------------*/
      if (rc >= 0) {
         rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &f, &p, &t);
         DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      } else {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         f = fp;
         p = pp;
         t = tp;
      }
      DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", f, p, t);
      /*---(process moves)---------------*/
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}




/*====================------------------------------------====================*/
/*===----                         rk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___RK_MOVES________________o (void) {;}
/*
 *  relative polar kinematics are relative from the hip.  likely
 *  limited use, but very useful to change leg positions and stances.
 *
 *   d  = degree for femur
 *   xz = distance along that angle
 *   y  = height as usual
 *
 *  will support both rk_pure and rk_from
 *
 */

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine__legs_rk_getter   (int a_leg, char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   double      xt, zt, yt, dt, ot, tt, xz;       /* temporary coordinates     */
   float       x_full, x_diff;
   float       x_tang, x_out, x_out2;
   float       x_adj;
   float       xi, zi, xo, zo;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(convert to rk)---------------*/
   x_adj  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   xi     = -x_adj * sin (cp * DEG2RAD);
   zi     =  x_adj * cos (cp * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fh, %8.2fx, %8.2fz", x_adj, xi, zi);
   xo     = xp - xi;
   zo     = zp - zi;
   x_out  = sqrt ((xo * xo) + (zo * zo));
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fo, %8.2fx, %8.2fz", x_out, xo, zo);
   x_full = atan2 (-zo, xo)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   x_diff = x_full - cp;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2fo, %8.2fy", x_full, x_diff, x_out, yp);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (x_diff, d_str, &dt);
      if (rc == 0)  rc  = yPARSE_adjval   (x_out , o_str, &ot);
      if (rc == 0)  rc  = yPARSE_adjval   (yp    , y_str, &yt);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_diff, d_str, &dt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_out , o_str, &ot);
      if (rc == 0)  rc  = yPARSE_adjfrom  (yp    , y_str, &yt);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", dt, ot, yt);
   /*---(convert from cx)-------------*/
   x_full = dt + cp;
   xz     = ot;
   xo     =  xz * cos (x_full * DEG2RAD);
   zo     = -xz * sin (x_full * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("fix_out"   , "%8.3fd, %8.3ff, %8.2fo, %8.2fxo, %8.2fzo", dt, x_full, ot, zo, zo);
   xt     = xi + xo;
   zt     = zi + zo;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
   /*---(save)---------------------------*/
   if (x != NULL)  *x = xt;
   if (z != NULL)  *z = zt;
   if (y != NULL)  *y = yt;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle center verbs -------------------[ ------ [ ------ ]-*/
ykine_legs_rk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   double      b           =  0.0;
   float       x, z, y;
   float       f, p, t;
   char        x_list      [LEN_HUND];
   char       *d_str       [LEN_LABEL];
   char       *o_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = yPARSE_popstr    (d_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("d_str"     , d_str);
   if (rc >= 0)  rc = yPARSE_popstr    (o_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("o_str"     , o_str);
   if (rc >= 0)  rc = yPARSE_popstr    (y_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("y_str"     , y_str);
   /*---(handle trouble)--------------*/
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (x_list [rc] == '_')  continue;
      /*---(get positions)---------------*/
      rc = ykine__legs_rk_getter (x_leg, d_str, o_str, y_str, &x, &z, &y);
      DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         x = xp;
         z = zp;
         y = yp;
      }
      DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", x, z, y);
      /*---(inverse kinematics)----------*/
      rc = yKINE_inverse (x_leg, x, z, y);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      /*---(retrieve angles)-------------*/
      if (rc >= 0) {
         rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &f, &p, &t);
         DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      } else {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         f = fp;
         p = pp;
         t = tp;
      }
      DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", f, p, t);
      /*---(process moves)---------------*/
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___TK_MOVES________________o (void) {;}
/*
 *  tangent kinematics is relative to the thorax angle of the leg.  likely
 *  limited use, but very useful to change leg positions and stances.
 *
 *   x  = distance from attention stance along thorax angle (pos = out, neg = in)
 *   z  = tangent distance to thorax (pos = CCW/left, neg = CW/right)
 *   y  = height as usual
 *
 *  will support both tk_pure and tk_from
 *
 */

char  /*--> save relative tk based move -----------[ ------ [ ------ ]-*/
ykine__legs_tk_getter   (int a_leg, char *o_str, char *t_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      xt, zt, yt, tt, ot;               /* temporary coordinates     */
   float       x_full, x_diff;
   float       xz          =  0.0;
   float       x_tang, x_out, x_out2, x_down;
   float       x_hadj, x_vadj;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   x_full = atan2 (-zp, xp)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   x_diff = x_full - cp;
   x_hadj  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   x_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   x_tang = -xzp * sin (x_diff * DEG2RAD);
   x_out2 =  xzp * cos (x_diff * DEG2RAD);
   x_out  = x_out2 - x_hadj;
   x_down = yp + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2f2, %8.2fo, %8.2ft", x_full, x_diff, x_out2, x_out, x_tang);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (x_out , o_str, &ot);
      if (rc == 0)  rc  = yPARSE_adjval   (x_tang, t_str, &tt);
      if (rc == 0)  rc  = yPARSE_adjval   (x_down, y_str, &yt);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_out , o_str, &ot);
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_tang, t_str, &tt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (x_down, y_str, &yt);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fo, %8.2ft, %8.2fy", ot, tt, yt);
   /*---(convert from tx)-------------*/
   ot    += x_hadj;
   x_diff = -atan2 (tt, ot) * RAD2DEG;
   x_full = cp + x_diff;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   xz     = sqrt ((ot * ot) + (tt * tt));
   DEBUG_YKINE_SCRP  yLOG_complex ("fix"       , "%8.3fd, %8.3ff, %8.2fo, %8.2ft, %8.2fxz", x_diff, x_full, ot, tt, xz);
   zt     = -xz * sin (x_full * DEG2RAD);
   xt     =  xz * cos (x_full * DEG2RAD);
   yt    -= x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
   /*---(save)---------------------------*/
   if (x != NULL)  *x = xt;
   if (z != NULL)  *z = zt;
   if (y != NULL)  *y = yt;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle tangent verbs ------------------[ ------ [ ------ ]-*/
ykine_legs_tk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   double      b           =  0.0;
   float       x, z, y;
   float       f, p, t;
   char        x_list      [LEN_HUND];
   char       *o_str       [LEN_LABEL];
   char       *t_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = yPARSE_popstr    (o_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("o_str"     , o_str);
   if (rc >= 0)  rc = yPARSE_popstr    (t_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("t_str"     , t_str);
   if (rc >= 0)  rc = yPARSE_popstr    (y_str);
   DEBUG_YKINE_SCRP  yLOG_info    ("y_str"     , y_str);
   /*---(handle trouble)--------------*/
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (x_list [rc] == '_')  continue;
      /*---(get positions)---------------*/
      rc = ykine__legs_tk_getter (x_leg, o_str, t_str, y_str, &x, &z, &y);
      DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         x = xp;
         z = zp;
         y = yp;
      }
      DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", x, z, y);
      /*---(inverse kinematics)----------*/
      rc = yKINE_inverse (x_leg, x, z, y);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      /*---(retrieve angles)-------------*/
      if (rc >= 0) {
         rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &f, &p, &t);
         DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      } else {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         f = fp;
         p = pp;
         t = tp;
      }
      DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", f, p, t);
      /*---(process moves)---------------*/
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
      rc = ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}




