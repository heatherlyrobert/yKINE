/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*
 * previous values saved at start as a fallback in case of failure
 */
static char     s_servos    [LEN_HUND];
static char     s_accel     [LEN_LABEL];
static double   b     =  0.0;    /* traditional duration                      */

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

char         /*--> identify the servos effected ----------[ ------ [ ------ ]-*/
ykine__legs_prepare     (char *a_one, char *a_two, char *a_thr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (s_servos);
   DEBUG_YKINE_SCRP   yLOG_info    ("s_servos"  , s_servos);
   /*---(check accel)-----------------*/
   yPARSE_top      (s_accel);
   DEBUG_YKINE_SCRP  yLOG_info    ("s_accel"   , s_accel);
   rc  = ykine_accel_dur (s_accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);
   /*---(check normal)----------------*/
   yPARSE_popval (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   if (rc < 0)  strlcpy (s_accel, "", LEN_LABEL);
   else         b = -1.0;
   /*---(get positions)---------------*/
   rc = yPARSE_popstr    (a_one);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_one"     , "%3d, %s", rc, a_one);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yPARSE_popstr    (a_two);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_two"     , "%3d, %s", rc, a_two);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yPARSE_popstr    (a_thr);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_thr"     , "%3d, %s", rc, a_thr);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

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

char
ykine__legs_ik_complete (char a_meth, int a_leg, float x, float z, float y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   float       f, p, t;
   float       x_pct;
   float       xd, zd, yd;
   float       b;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(args)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", x, z, y);
   xd = x - xp;
   zd = z - zp;
   yd = y - yp;
   DEBUG_YKINE_SCRP   yLOG_complex ("diffs"     , "%8.3lfx , %8.3lfz , %8.3lfy", xd, zd, yd);
   /*---(inverse kinematics)----------*/
   rc = yKINE_inverse (a_leg, x, z, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
   /*---(retrieve angles)-------------*/
   if (rc >= 0) {
      rc = yKINE_angles   (a_leg, a_meth, NULL, &f, &p, &t);
      DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
      f = fp;
      p = pp;
      t = tp;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", f, p, t);
   /*---(process moves)---------------*/
   rc = ykine__legs_set_servo (a_meth, a_leg, YKINE_FEMU, f, b);
   rc = ykine__legs_set_servo (a_meth, a_leg, YKINE_PATE, p, b);
   rc = ykine__legs_set_servo (a_meth, a_leg, YKINE_TIBI, t, b);
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
   int         x_leg       = 0.0;
   float       f, p, t;
   char       *x_femu      [LEN_LABEL];
   char       *x_pate      [LEN_LABEL];
   char       *x_tibi      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle input)----------------*/
   rc = ykine__legs_prepare (x_femu, x_pate, x_tibi);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("CHECK EACH LEG");
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_FEMU);
      if (s_servos [rc] == '_')  continue;
      /*---(get existing data)-----------*/
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_FEMU, x_femu, &f);
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_PATE, x_pate, &p);
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_TIBI, x_tibi, &t);
      /*---(calculate endpoint)----------*/
      if (rc >= 0)  rc = yKINE_forward     (x_leg, f, p, t);
      DEBUG_YKINE_SCRP  yLOG_value   ("forward"   , rc);
      /*---(process moves)---------------*/
      if (b < 0)  {
         ykine_accel_leg       (YKINE_FK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_FEMU, f, b);
         ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_PATE, p, b);
         ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_TIBI, t, b);
      }
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
   float       x, z, y;
   float       f, p, t;
   char        x_list      [LEN_HUND];
   char       *x_str       [LEN_LABEL];
   char       *z_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   double      x_thor      =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle input)----------------*/
   rc = ykine__legs_prepare (x_str, z_str, y_str);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   --rce;
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (rc < 0)              continue;
      if (s_servos [rc] == '_')  continue;
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
      DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", f, p, t);
      /*---(process moves)---------------*/
      if (b < 0)  {
         ykine_accel_leg       (YKINE_IK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      }
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
   /*---(prepare constants)-----------*/
   x_hadj  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   x_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   /*---(convert to ck)---------------*/
   x_full  = atan2 (-zp, xp)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   /*> x_diff  = cp - x_full;                                                         <*/
   x_diff  = x_full - cp;
   x_out2  = xzp;
   x_out   = x_out2 - x_hadj;
   x_down  = yp + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2f2, %8.2fo, %8.2fy", x_full, x_diff, x_out2, x_out, yp);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread (myKINE.s_from, a_leg , x_diff, d_str, &dt);
   rc = ykine_stance_radius (myKINE.s_from        , x_out , o_str, &ot);
   rc = ykine_stance_height (myKINE.s_from        , x_down, y_str, &yt);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", dt, ot, yt);
   /*---(convert from cx)-------------*/
   ot    += x_hadj;
   /*> x_full = cp - dt;                                                              <*/
   x_full = cp + dt;
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
   float       x, z, y;
   float       f, p, t;
   char       *d_str       [LEN_LABEL];
   char       *o_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle input)----------------*/
   rc = ykine__legs_prepare (d_str, o_str, y_str);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (rc < 0)  continue;
      if (s_servos [rc] == '_')  continue;
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
      if (b < 0)  {
         ykine_accel_leg       (YKINE_IK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      }
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
   float       x_tang, x_out, x_out2, x_down;
   float       x_hadji, x_hadjo, x_vadj;
   float       xi, zi, xo, zo;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(prepare constants)-----------*/
   x_hadji = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   x_hadjo = yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   x_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   /*---(calc inner triangle)---------*/
   xi      =  x_hadji * cos (cp * DEG2RAD);
   zi      = -x_hadji * sin (cp * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fh, %8.2fx, %8.2fz", x_hadji, xi, zi);
   /*---(calc outer triangle)---------*/
   xo      = xp - xi;
   zo      = zp - zi;
   x_out2  = sqrt ((xo * xo) + (zo * zo));
   x_out   = x_out2 - x_hadjo;
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fx, %8.2fz, %8.2f2, %8.2fa, %8.2fo", xo, zo, x_out2, x_hadjo, x_out);
   /*---(calc angles)-----------------*/
   x_full  = atan2 (-zo, xo)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   /*> x_diff  = cp - x_full;                                                         <*/
   x_diff  = x_full - cp;
   /*---(adjust ypos)-----------------*/
   x_down  = yp + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2fo, %8.2fy", x_full, x_diff, x_out, yp);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread (myKINE.s_from, a_leg , x_diff, d_str, &dt);
   rc = ykine_stance_radius (myKINE.s_from        , x_out , o_str, &ot);
   rc = ykine_stance_height (myKINE.s_from        , x_down, y_str, &yt);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", dt, ot, yt);
   /*---(convert from rk)-------------*/
   ot    += x_hadjo;
   /*> x_full = cp - dt;                                                              <*/
   x_full = cp + dt;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   xz     = ot;
   xo     =  xz * cos (x_full * DEG2RAD);
   zo     = -xz * sin (x_full * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("fix_out"   , "%8.3fd, %8.3ff, %8.2fo, %8.2fxo, %8.2fzo", dt, x_full, ot, zo, zo);
   xt     = xi + xo;
   zt     = zi + zo;
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
ykine_legs_rk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   float       x, z, y;
   float       f, p, t;
   char       *d_str       [LEN_LABEL];
   char       *o_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle input)----------------*/
   rc = ykine__legs_prepare (d_str, o_str, y_str);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (s_servos [rc] == '_')  continue;
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
      if (b < 0)  {
         ykine_accel_leg       (YKINE_IK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___NK_MOVES________________o (void) {;}
/*
 *  neighborhood kinematics is just polar IK relative to military attention
 *  position.  it will be very useful for gaits and stepping.
 *
 */

char  /*--> save relative tk based move -----------[ ------ [ ------ ]-*/
ykine__legs_nk_getter   (int a_leg, char *x_str, char *z_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       xi, zi, xo, zo, yo;
   double      xt, zt, yt;                       /* temporary coordinates     */
   float       xz          =  0.0;
   float       x_tang, x_out, x_out2, x_down;
   float       x_hadji, x_vadj;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(prepare constants)-----------*/
   x_hadji = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU);
   x_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   /*---(calc inner triangle)---------*/
   xi      =  x_hadji * cos (cp * DEG2RAD);
   zi      = -x_hadji * sin (cp * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fh, %8.2fx, %8.2fz", x_hadji, xi, zi);
   /*---(calc outer triangle)---------*/
   xo      = xp - xi;
   zo      = zp - zi;
   yo      = yp + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fx, %8.2fz, %8.2fy", xo, zo, yo);
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
   /*---(convert from nk)-------------*/
   xt    += xi;
   zt    += zi;
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
ykine_legs_nk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   float       x, z, y;
   float       f, p, t;
   char       *x_str       [LEN_LABEL];
   char       *z_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle input)----------------*/
   rc = ykine__legs_prepare (x_str, z_str, y_str);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (s_servos [rc] == '_')  continue;
      /*---(get positions)---------------*/
      rc = ykine__legs_nk_getter (x_leg, x_str, z_str, y_str, &x, &z, &y);
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
      if (b < 0)  {
         ykine_accel_leg       (YKINE_IK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      }
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
   /*> x_diff = x_full - cp;                                                          <*/
   x_diff = cp - x_full;
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
   x_diff = atan2 (tt, ot) * RAD2DEG;
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
   float       x, z, y;
   float       f, p, t;
   char       *o_str       [LEN_LABEL];
   char       *t_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   rc = ykine__legs_prepare (o_str, t_str, y_str);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, YKINE_TIBI);
      if (s_servos [rc] == '_')  continue;
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
      if (b < 0)  {
         ykine_accel_leg       (YKINE_IK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}




