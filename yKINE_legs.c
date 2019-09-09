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

static float    coxab, femub, pateb, tibib;   /* angle begin/previous         */
static float    coxac, femuc, patec, tibic;   /* angle current                */
static float    coxae, femue, patee, tibie;   /* angle end/next               */



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
   coxab  = yKINE_legdeg (a_leg);
   x_servo = ykine_servo_pointer (a_leg, YKINE_FEMU);
   rc  = ykine_move_savedloc  (x_servo, NULL, &femub, NULL, NULL, NULL, NULL);
   x_servo = ykine_servo_pointer (a_leg, YKINE_PATE);
   rc  = ykine_move_savedloc  (x_servo, NULL, &pateb, NULL, NULL, NULL, NULL);
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   rc  = ykine_move_savedloc  (x_servo, NULL, &tibib, &myKINE.xb, &myKINE.zb, &myKINE.yb, &myKINE.xzb);
   /*---(adjust for foot)----------------*/
   myKINE.yb -= yKINE_seglen (YKINE_FOOT);
   /*---(report)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("angles"    , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , coxab, femub, pateb, tibib);
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", myKINE.xb, myKINE.zb, myKINE.yb, myKINE.xzb);
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
   DEBUG_YKINE_SCRP   yLOG_complex ("parms"     , "%dm, %dl, %ds, %8.2fd, %8.2fb", a_meth, a_leg, a_seg, a_deg, a_beat);
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
   rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_cline, a_deg, s);
   rc = yKINE_endpoint    (a_leg, a_seg, a_meth, NULL, NULL, &x, &z, &y, NULL);
   DEBUG_YKINE_SCRP   yLOG_complex ("updated"   , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
   rc = ykine_move_addloc (x_servo, x, z, y);
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
   float       x_pct;
   float       xd, zd, yd;
   float       b;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(args)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", x, z, y);
   xd = x - myKINE.xb;
   zd = z - myKINE.zb;
   yd = y - myKINE.yb;
   DEBUG_YKINE_SCRP   yLOG_complex ("diffs"     , "%8.3lfx , %8.3lfz , %8.3lfy", xd, zd, yd);
   /*---(inverse kinematics)----------*/
   rc = yKINE_inverse (a_leg, x, z, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
   /*---(retrieve angles)-------------*/
   if (rc >= 0) {
      rc = yKINE_angles   (a_leg, a_meth, NULL, &femue, &patee, &tibie);
      DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
      femue = femub;
      patee = pateb;
      tibie = tibib;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", femue, patee, tibie);
   /*---(process moves)---------------*/
   rc = ykine__legs_set_servo (a_meth, a_leg, YKINE_FEMU, femue, b);
   rc = ykine__legs_set_servo (a_meth, a_leg, YKINE_PATE, patee, b);
   rc = ykine__legs_set_servo (a_meth, a_leg, YKINE_TIBI, tibie, b);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                  support for partial moves                   ----===*/
/*====================------------------------------------====================*/
static void      o___EXACT___________________o (void) {;}

char
ykine_legs_dist_linear  (char t, float *l)
{
   /*---(locals)-----------+-----+-----+-*/
   float       xd, zd, yd, ld;
   /*---(full beg/end)-------------------*/
   if (t == 'f') {
      xd  = myKINE.xe - myKINE.xb;
      zd  = myKINE.ze - myKINE.zb;
      yd  = myKINE.ye - myKINE.yb;
   }
   /*---(curr/prev)----------------------*/
   else if (t == 'c') {
      xd  = myKINE.xc - myKINE.xp;
      zd  = myKINE.zc - myKINE.zp;
      yd  = myKINE.yc - myKINE.yp;
   }
   /*---(length)-------------------------*/
   ld = sqrt ((xd * xd) + (zd * zd) + (yd * yd));
   if      (t == 'f')   myKINE.le = ld;
   else if (t == 'c')   myKINE.lc = ld;
   /*---(save back)----------------------*/
   if (l != NULL)   *l = ld;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_legs_dist_polar   (char t, float *l)
{  /* ´ arc distance = 2÷r is full circum and * (deg/360) cuts our part
    * ´ calc the arc length, then treat it as a triangle side for distance
    * ´ less than 90 deg, the difference is a max of 5% greater length
    */
   float       dd, od, yd, ld;
   /*---(full beg/end)-------------------*/
   if (t == 'f') {
      dd  = ((myKINE.de - myKINE.db) / 360.0) * 2.0 * 3.1415927 * ((myKINE.ob + myKINE.oe) / 2.0);
      od  = myKINE.oe - myKINE.ob;
      yd  = myKINE.ye - myKINE.yb;
   }
   /*---(curr/prev)----------------------*/
   else if (t == 'c') {
      dd  = ((myKINE.dc - myKINE.dp) / 360.0) * 2.0 * 3.1415927 * ((myKINE.op + myKINE.oc) / 2.0);
      od  = myKINE.oc - myKINE.op;
      yd  = myKINE.yc - myKINE.yp;
   }
   /*---(length)-------------------------*/
   ld  = sqrt ((dd * dd) + (od * od) + (yd * yd));
   if      (t == 'f')   myKINE.le = ld;
   else if (t == 'c')   myKINE.lc = ld;
   /*---(save back)----------------------*/
   if (l != NULL) *l = ld;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_legs_exact_linear (float p)
{
   /*---(calculate)----------------------*/
   myKINE.pct = p;
   ykine_exact_calc ('-', myKINE.xb, myKINE.xe, p, &myKINE.xc);
   ykine_exact_calc ('-', myKINE.zb, myKINE.ze, p, &myKINE.zc);
   ykine_exact_calc ('-', myKINE.yb, myKINE.ye, p, &myKINE.yc);
   DEBUG_YKINE_MOVE   yLOG_complex ("linear"    , "%8.2fx, %8.2fz, %8.2fy", myKINE.xc, myKINE.zc, myKINE.yc);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_legs_exact_polar  (float p)
{
   /*---(calculate)----------------------*/
   myKINE.pct = p;
   ykine_exact_calc ('d', myKINE.db, myKINE.de, p, &myKINE.dc);
   ykine_exact_calc ('-', myKINE.ob, myKINE.oe, p, &myKINE.oc);
   ykine_exact_calc ('-', myKINE.yb, myKINE.ye, p, &myKINE.yc);
   DEBUG_YKINE_MOVE   yLOG_complex ("polar"     , "%8.2fd, %8.2fo, %8.2fy", myKINE.dc, myKINE.oc, myKINE.yc);
   /*---(complete)-----------------------*/
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
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_FEMU, x_femu, &femue);
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_PATE, x_pate, &patee);
      if (rc >= 0)  rc = ykine__legs_fk_getter (x_leg, YKINE_TIBI, x_tibi, &tibie);
      /*---(calculate endpoint)----------*/
      if (rc >= 0)  rc = yKINE_forward     (x_leg, femue, patee, tibie);
      DEBUG_YKINE_SCRP  yLOG_value   ("forward"   , rc);
      /*---(process moves)---------------*/
      if (b < 0)  {
         ykine_leg_accel       (YKINE_FK, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_FEMU, femue, b);
         ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_PATE, patee, b);
         ykine__legs_set_servo (YKINE_FK, x_leg, YKINE_TIBI, tibie, b);
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
   x_full  = atan2 (-myKINE.zb, myKINE.xb)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   /*> x_diff  = coxab - x_full;                                                         <*/
   x_diff  = x_full - coxab;
   x_out2  = myKINE.xzb;
   x_out   = x_out2 - x_hadj;
   x_down  = myKINE.yb + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2f2, %8.2fo, %8.2fy", x_full, x_diff, x_out2, x_out, myKINE.yb);
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
   /*> x_full = coxab - dt;                                                              <*/
   x_full = coxab + dt;
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
   xi      =  x_hadji * cos (coxab * DEG2RAD);
   zi      = -x_hadji * sin (coxab * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fh, %8.2fx, %8.2fz", x_hadji, xi, zi);
   /*---(calc outer triangle)---------*/
   xo      = myKINE.xb - xi;
   zo      = myKINE.zb - zi;
   x_out2  = sqrt ((xo * xo) + (zo * zo));
   x_out   = x_out2 - x_hadjo;
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fx, %8.2fz, %8.2f2, %8.2fa, %8.2fo", xo, zo, x_out2, x_hadjo, x_out);
   /*---(calc angles)-----------------*/
   x_full  = atan2 (-zo, xo)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   /*> x_diff  = coxab - x_full;                                                         <*/
   x_diff  = x_full - coxab;
   /*---(adjust ypos)-----------------*/
   x_down  = myKINE.yb + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.3fd, %8.2fo, %8.2fy", x_full, x_diff, x_out, myKINE.yb);
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
   /*> x_full = coxab - dt;                                                              <*/
   x_full = coxab + dt;
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
   xi      =  x_hadji * cos (coxab * DEG2RAD);
   zi      = -x_hadji * sin (coxab * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fh, %8.2fx, %8.2fz", x_hadji, xi, zi);
   /*---(calc outer triangle)---------*/
   xo      = myKINE.xb - xi;
   zo      = myKINE.zb - zi;
   yo      = myKINE.yb + x_vadj;
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fx, %8.2fz, %8.2fy", xo, zo, yo);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (myKINE.xb, x_str, &xt);
      if (rc == 0)  rc  = yPARSE_adjval   (myKINE.zb, z_str, &zt);
      if (rc == 0)  rc  = yPARSE_adjval   (myKINE.yb, y_str, &yt);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (myKINE.xb, x_str, &xt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (myKINE.zb, z_str, &zt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (myKINE.yb, y_str, &yt);
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
   x_full = atan2 (-myKINE.zb, myKINE.xb)  * RAD2DEG;
   if (x_full > 360)  x_full -= 360;
   if (x_full <   0)  x_full += 360;
   /*> x_diff = x_full - coxab;                                                          <*/
   x_diff = coxab - x_full;
   x_hadj  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   x_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   x_tang = -myKINE.xzb * sin (x_diff * DEG2RAD);
   x_out2 =  myKINE.xzb * cos (x_diff * DEG2RAD);
   x_out  = x_out2 - x_hadj;
   x_down = myKINE.yb + x_vadj;
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
   x_full = coxab + x_diff;
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



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___IK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__legs_ik_getter   (int a_leg, char *x_str, char *z_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      xt, zt, yt;                       /* temporary coordinates     */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_adjval   (myKINE.xb, x_str, &xt);
      if (rc == 0)  rc  = yPARSE_adjval   (myKINE.zb, z_str, &zt);
      if (rc == 0)  rc  = yPARSE_adjval   (myKINE.yb, y_str, &yt);
   } else {
      if (rc == 0)  rc  = yPARSE_adjfrom  (myKINE.xb, x_str, &xt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (myKINE.zb, z_str, &zt);
      if (rc == 0)  rc  = yPARSE_adjfrom  (myKINE.yb, y_str, &yt);
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

char         /*--> handle tangent verbs ------------------[ ------ [ ------ ]-*/
ykine__legs_ik_driver    (char a_meth)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   char       *x_one       [LEN_LABEL];
   char       *x_two       [LEN_LABEL];
   char       *x_thr       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   rc = ykine__legs_prepare (x_one, x_two, x_thr);
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
      switch (a_meth) {
      case YKINE_IK :  rc = ykine__legs_ik_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_CK :  rc = ykine__legs_ck_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_RK :  rc = ykine__legs_rk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_TK :  rc = ykine__legs_tk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_NK :  rc = ykine__legs_nk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_SK :  break;
      }
      DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         myKINE.xe = myKINE.xb;
         myKINE.ze = myKINE.zb;
         myKINE.ye = myKINE.yb;
      }
      DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", myKINE.xe, myKINE.ze, myKINE.ye);
      /*---(inverse kinematics)----------*/
      rc = yKINE_inverse (x_leg, myKINE.xe, myKINE.ze, myKINE.ye - 6.3);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      /*---(retrieve angles)-------------*/
      if (rc >= 0) {
         rc = yKINE_angles   (x_leg, YKINE_IK, &coxae, &femue, &patee, &tibie);
         DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      } else {
         DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
         coxae = coxab;
         femue = femub;
         patee = pateb;
         tibie = tibib;
      }
      DEBUG_YKINE_SCRP  yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", femue, patee, tibie);
      /*---(get distance)----------------*/
      switch (a_meth) {
      case YKINE_IK : case YKINE_TK : case YKINE_NK : case YKINE_SK :
         ykine_legs_dist_linear ('f', NULL);
         break;
      case YKINE_CK : case YKINE_RK :
         ykine_legs_dist_polar  ('f', NULL);
         break;
      }
      DEBUG_YKINE_SCRP  yLOG_value   ("distance"  , myKINE.le);
      /*---(process moves)---------------*/
      if (b < 0)  {
         ykine_leg_accel       (a_meth, x_leg, s_accel);
      } else {
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_FEMU, femue, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_PATE, patee, b);
         ykine__legs_set_servo (YKINE_IK, x_leg, YKINE_TIBI, tibie, b);
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       ykine_legs_ik   (void)  { return ykine__legs_ik_driver (YKINE_IK); }
char       ykine_legs_ck   (void)  { return ykine__legs_ik_driver (YKINE_CK); }
char       ykine_legs_rk   (void)  { return ykine__legs_ik_driver (YKINE_RK); }
char       ykine_legs_tk   (void)  { return ykine__legs_ik_driver (YKINE_TK); }
char       ykine_legs_nk   (void)  { return ykine__legs_ik_driver (YKINE_NK); }
char       ykine_legs_sk   (void)  { return ykine__legs_ik_driver (YKINE_SK); }



/*====================------------------------------------====================*/
/*===----                    accelerated moves                         ----===*/
/*====================------------------------------------====================*/
static void      o___ACCEL___________________o (void) {;}

char
ykine_leg_accel         (char a_meth, int a_leg, char *a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   float       x_pct;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_point   ("a_dur"     , a_dur);
   /*---(calculate)----------------------*/
   rc = ykine_accel_calc (a_meth, a_dur [1], a_dur [0], a_dur [2]);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare moves)------------------*/
   --rce;
   for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
      DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fd %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
      if (g_accel_info [i].dist < 0.1)   continue;
      x_pct = g_accel_info [i].pct;
      /*---(set current)-----------------*/
      switch (a_meth) {
      case YKINE_IK : case YKINE_TK : case YKINE_NK : case YKINE_SK :
         ykine_legs_exact_linear (x_pct);
         break;
      case YKINE_CK : case YKINE_RK :
         ykine_legs_exact_polar  (x_pct);
         break;
      }
      /*---(inverse on small move)-------*/
      rc = yKINE_inverse (a_leg, myKINE.xc, myKINE.zc, myKINE.yc - 6.3);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rce;
      }
      /*---(retrieve angles)-------------*/
      yKINE_angles   (a_leg, YKINE_IK, &coxac, &femuc, &patec, &tibic);
      DEBUG_YKINE_MOVE   yLOG_complex ("angles"    , "%8.3fc, %8.3ff, %8.3fp, %8.3ft", coxac, femuc, patec, tibic);
      /*---(create moves)----------------*/
      ykine__legs_set_servo (YKINE_IK, a_leg, YKINE_FEMU, femuc, g_accel_info [i].dur * myKINE.s_pace);
      ykine__legs_set_servo (YKINE_IK, a_leg, YKINE_PATE, patec, g_accel_info [i].dur * myKINE.s_pace);
      ykine__legs_set_servo (YKINE_IK, a_leg, YKINE_TIBI, tibic, g_accel_info [i].dur * myKINE.s_pace);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}




