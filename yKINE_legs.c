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

char
ykine__legs_degfix      (float *d)
{
   while (*d >  180.0)  *d -= 360.0;
   while (*d < -180.0)  *d += 360.0;
   if (*d > -0.001 && *d < 0.001)  *d = 0.000;
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
   DEBUG_YKINE_MOVE   yLOG_complex ("polar_d"   , "%8.2fb, %8.2fe, %8.2fc", myKINE.db, myKINE.de, myKINE.dc);
   ykine_exact_calc ('-', myKINE.ob, myKINE.oe, p, &myKINE.oc);
   DEBUG_YKINE_MOVE   yLOG_complex ("polar_o"   , "%8.2fb, %8.2fe, %8.2fc", myKINE.ob, myKINE.oe, myKINE.oc);
   ykine_exact_calc ('-', myKINE.yb, myKINE.ye, p, &myKINE.yc);
   DEBUG_YKINE_MOVE   yLOG_complex ("polar_y"   , "%8.2fb, %8.2fe, %8.2fc", myKINE.yb, myKINE.ye, myKINE.yc);
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
 *   xz = distance from military neutral
 *   y  = height as usual
 *
 *  will support both ck_pure and ck_from
 *
 */

char
ykine_legs_ik2ck        (float a_coxa, float x, float z, float *d, float *o)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz, xz;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fx, %8.2fz", a_coxa, x, z);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%pd, %po", d, o);
   --rce;  if (d == NULL || o == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc angle)------------------*/
   *d     = atan2 (-z, x)  * RAD2DEG - a_coxa;
   ykine__legs_degfix (d);
   /*---(calc outward dist)--------------*/
   xz     = sqrt ((x * x) + (z * z));
   *o      = xz - dxz;
   if (*o > -0.001 && *o < 0.001)  *o = 0.000;
   /*---(report)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.2fdxz, %8.3ff, %8.3fd, %8.2fo", dxz, x_full, *d, *o);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_ck2ik        (float a_coxa, float d, float o, float *x, float *z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz;
   float       xz;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fd, %8.2fo", a_coxa, d, o);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%px, %pz", x, z);
   --rce;  if (x == NULL || z == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(convert from rk)-------------*/
   x_full = a_coxa + d;
   xz     = o + dxz;
   DEBUG_YKINE_SCRP  yLOG_complex ("calcs"     , "%8.2fdxz, %8.2fd, %8.2fxz", dxz, x_full, xz);
   /*---(save)---------------------------*/
   *x     =  xz * cos (x_full * DEG2RAD);
   if (*x > -0.001 && *x < 0.001)  *x = 0.000;
   *z     = -xz * sin (x_full * DEG2RAD);
   if (*z > -0.001 && *z < 0.001)  *z = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz", *x, *z);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine__legs_ck_getter   (int a_leg, char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       db, ob, yb;                  /* previous/begin values          */
   double      de, oe, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(convert to ck)---------------*/
   ykine_legs_ik2ck (coxab, myKINE.xb, myKINE.zb, &db, &ob);
   yb      = myKINE.yb;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", db, ob, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (myKINE.s_from, a_leg , db, d_str, &de);
   rc = ykine_stance_radius  (myKINE.s_from        , ob, o_str, &oe);
   rc = ykine_stance_scale   (myKINE.s_from        , yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from cx)-------------*/
   ykine_legs_ck2ik (coxab, de, oe, x, z);
   if (y != NULL)  *y = ye;
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
 *   xz = distance from military neutral
 *   y  = height as usual
 *
 *  will support both rk_pure and rk_from
 *
 */

char
ykine_legs_ik2rk        (float a_coxa, float x, float z, float *d, float *o)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxzi, dxzo;
   float       xi, zi, xo, zo;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fx, %8.2fz", a_coxa, x, z);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%pd, %po", d, o);
   --rce;  if (d == NULL || o == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxzi    = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   dxzo    = yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc inner triangle)---------*/
   xi      =  dxzi * cos (a_coxa * DEG2RAD);
   zi      = -dxzi * sin (a_coxa * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdzxi, %8.2fxi, %8.2fzi", dxzi, xi, zi);
   /*---(calc outer triangle)---------*/
   xo      = x - xi;
   zo      = z - zi;
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fxb, %8.2fzb, %8.2fxo, %8.2fzo", x, z, xo, zo);
   /*---(calc angles)-----------------*/
   *d      = atan2 (-zo, xo)  * RAD2DEG - a_coxa;
   ykine__legs_degfix (d);
   /*---(save)---------------------------*/
   *o      = sqrt ((xo * xo) + (zo * zo)) - dxzo;
   if (*o > -0.001 && *o < 0.001)  *o = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.2fdxzo, %8.3ff, %8.3fd, %8.2fo", dxzo, x_full, *d, *o);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_rk2ik        (float a_coxa, float d, float o, float *x, float *z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxzi, dxzo;
   float       xi, zi, xo, zo, xz;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fd, %8.2fo", a_coxa, d, o);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%px, %pz", x, z);
   --rce;  if (x == NULL || z == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxzi     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   dxzo     = yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc inner triangle)---------*/
   xi      =  dxzi * cos (a_coxa * DEG2RAD);
   zi      = -dxzi * sin (a_coxa * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdxzi, %8.2fx, %8.2fz", dxzi, xi, zi);
   /*---(convert from rk)-------------*/
   x_full = a_coxa + d;
   ykine__legs_degfix (&x_full);
   xz     = o + dxzo;
   xo     =  xz * cos (x_full * DEG2RAD);
   zo     = -xz * sin (x_full * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fdxzo, %8.2fd, %8.2fxz, %8.2fxo, %8.2fzo", dxzo, x_full, xz, xo, zo);
   /*---(save)---------------------------*/
   *x     = xi + xo;
   *z     = zi + zo;
   if (*z > -0.001 && *z < 0.001)  *z = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz", *x, *z);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine__legs_rk_getter   (int a_leg, char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       db, ob, yb;                  /* previous/begin values          */
   double      de, oe, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (x != NULL)  *x = 0.0;
   if (z != NULL)  *z = 0.0;
   if (y != NULL)  *y = 0.0;
   /*---(prepare)---------------------*/
   ykine__legs_get_prev (a_leg);
   ykine_legs_ik2rk (coxab, myKINE.xb, myKINE.zb, &db, &ob);
   yb      = myKINE.yb;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", db, ob, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (myKINE.s_from, a_leg , db, d_str, &de);
   rc = ykine_stance_radius  (myKINE.s_from        , ob, o_str, &oe);
   rc = ykine_stance_scale   (myKINE.s_from        , yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from rk)-------------*/
   ykine_legs_rk2ik (coxab, de, oe, x, z);
   if (y != NULL)  *y = ye;
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

char
ykine_legs_ik2tk        (float a_coxa, float x, float z, float *o, float *t)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz, xz;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fx, %8.2fz", a_coxa, x, z);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%po, %pt", o, t);
   --rce;  if (o == NULL || t == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc angle)------------------*/
   x_full = a_coxa - atan2 (-z, x)  * RAD2DEG;
   ykine__legs_degfix (&x_full);
   xz     = sqrt ((x * x) + (z * z));
   DEBUG_YKINE_SCRP  yLOG_complex ("calcs"     , "%8.2fdxz, %8.2fd, %8.2fxz", dxz, x_full, xz);
   /*---(calc distances)--------------*/
   *o =  xz * cos (x_full * DEG2RAD) - dxz;
   if (*o > -0.001 && *o < 0.001)  *o = 0.000;
   *t = -xz * sin (x_full * DEG2RAD);
   if (*t > -0.001 && *t < 0.001)  *t = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("distance"  , "%8.2fo, %8.2ft", *o, *t);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_tk2ik        (float a_coxa, float o, float t, float *x, float *z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz, xz;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fo, %8.2ft", a_coxa, o, t);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%px, %pz", x, z);
   --rce;  if (x == NULL || z == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz    = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc angle)------------------*/
   o     += dxz;
   x_full = a_coxa + atan2 (t, o) * RAD2DEG;
   ykine__legs_degfix (&x_full);
   xz     = sqrt ((o * o) + (t * t));
   DEBUG_YKINE_SCRP  yLOG_complex ("calcs"     , "%8.2fdxz, %8.2fd, %8.2fxz", dxz, x_full, xz);
   /*---(calc position)---------------*/
   *x     =  xz * cos (x_full * DEG2RAD);
   if (*x > -0.001 && *x < 0.001)  *x = 0.000;
   *z     = -xz * sin (x_full * DEG2RAD);
   if (*z > -0.001 && *z < 0.001)  *z = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz", *x, *z);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative tk based move -----------[ ------ [ ------ ]-*/
ykine__legs_tk_getter   (int a_leg, char *o_str, char *t_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       ob, tb, yb;
   double      oe, te, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2tk (coxab, myKINE.xb, myKINE.zb, &ob, &tb);
   yb = myKINE.yb;
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_radius  (myKINE.s_from        , ob, o_str, &oe);
   rc = ykine_stance_scale   (myKINE.s_from        , tb, t_str, &te);
   rc = ykine_stance_scale   (myKINE.s_from        , yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fo, %8.2ft, %8.2fy", oe, te, ye);
   /*---(convert from tx)-------------*/
   ykine_legs_tk2ik (coxab, oe, te, x, z);
   if (y != NULL)  *y = ye;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         sk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___SK_MOVES________________o (void) {;}
/*
 *  step kinematics is starts with direction and is relative to the military
 *  posture.  this should be useful for stepping patterns.
 *
 *   d  = direction of travel
 *   o  = outward distance along direction of travel
 *   y  = height as usual
 *
 *  will support both tk_pure and tk_from
 *
 */

char
ykine_legs_ik2sk        (float a_coxa, float x, float z, float *d, float *o)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz;
   float       xi, zi, xo, zo;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fx, %8.2fz", a_coxa, x, z);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%pd, %po", d, o);
   --rce;  if (d == NULL || o == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc inner triangle)---------*/
   xi      =  dxz * cos (a_coxa * DEG2RAD);
   zi      = -dxz * sin (a_coxa * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdzx, %8.2fxi, %8.2fzi", dxz, xi, zi);
   /*---(calc outer triangle)---------*/
   xo      = x - xi;
   zo      = z - zi;
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fxb, %8.2fzb, %8.2fxo, %8.2fzo", x, z, xo, zo);
   /*---(calc angles)-----------------*/
   *d      = atan2 (-zo, xo)  * RAD2DEG;
   ykine__legs_degfix (d);
   if (*d > -0.001 && *d < 0.001)  *d = 0.000;
   *o      = sqrt ((xo * xo) + (zo * zo));
   if (*o > -0.001 && *o < 0.001)  *o = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("distance"  , "%8.2fd, %8.2fo", *d, *o);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_sk2ik        (float a_coxa, float d, float o, float *x, float *z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz, xi, zi, xo, zo;
   float       x_full;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fd, %8.2fo", a_coxa, d, o);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%px, %pz", x, z);
   --rce;  if (x == NULL || z == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz    = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc inner triangle)---------*/
   xi      =  dxz * cos (a_coxa * DEG2RAD);
   zi      = -dxz * sin (a_coxa * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdxz, %8.2fx, %8.2fz", dxz, xi, zi);
   /*---(convert from rk)-------------*/
   ykine__legs_degfix (&d);
   xo     =  o * cos (d * DEG2RAD);
   zo     = -o * sin (d * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("outer"     , "%8.2fxo, %8.2fzo", xo, zo);
   /*---(save)---------------------------*/
   *x     = xi + xo;
   if (*x > -0.001 && *x < 0.001)  *x = 0.000;
   *z     = zi + zo;
   if (*z > -0.001 && *z < 0.001)  *z = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz", *x, *z);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative sk based move -----------[ ------ [ ------ ]-*/
ykine__legs_sk_getter   (int a_leg, char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       db, ob, yb;
   double      de, oe, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2sk (coxab, myKINE.xb, myKINE.zb, &db, &ob);
   yb = myKINE.yb;
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (myKINE.s_from, a_leg , db, d_str, &de);
   rc = ykine_stance_scale   (myKINE.s_from        , ob, o_str, &oe);
   rc = ykine_stance_scale   (myKINE.s_from        , yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from tx)-------------*/
   ykine_legs_sk2ik (coxab, de, oe, x, z);
   if (y != NULL)  *y = ye;
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
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine__legs_get_prev (a_leg);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.xb, x_str, x);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.zb, z_str, z);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.yb, y_str, y);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", *x, *z, *y);
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
      DEBUG_YKINE_SCRP  yLOG_double  ("distance"  , myKINE.le);
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
      /*> rc = yKINE_inverse (a_leg, myKINE.xc, myKINE.zc, myKINE.yc - 6.3);          <*/
      rc = yKINE_inverse (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);
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




