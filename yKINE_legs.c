/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char         /*--> identify the servos effected ----------[ ------ [ ------ ]-*/
ykine_legs_prepservos   (char a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark legs)-------------------*/
   switch (a_verb) {
   case YKINE_ZE : case YKINE_PO :
      rc = ykine_servos ("zz");
      break;
   case YKINE_OR : case YKINE_TI :
      rc = ykine_servos ("oo");
      break;
   default :
      rc  = ykine_scrp_popservo ();
      DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
      break;
   }
   /*---(prepare list)-------------------*/
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (myKINE.servos);
   DEBUG_YKINE_SCRP   yLOG_info    ("servos"    , myKINE.servos);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> identify the servos effected ----------[ ------ [ ------ ]-*/
ykine_legs_prepare      (char *a_one, char *a_two, char *a_thr, char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(check accel)-----------------*/
   yPARSE_top      (myKINE.accel);
   DEBUG_YKINE_SCRP  yLOG_info    ("accel"     , myKINE.accel);
   rc  = ykine_accel_dur (myKINE.accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);
   /*---(check normal)----------------*/
   yPARSE_popval (0.0, &myKINE.b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , myKINE.b);
   if (rc < 0)  strlcpy (myKINE.accel, "", LEN_LABEL);
   else         myKINE.b = -1.0;
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
   /*---(get optional label)----------*/
   rc = yPARSE_popstr    (a_label);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_label"   , "%3d, %s", rc, a_label);
   if (strcmp (a_label, "-") == 0) {
      strlcpy (a_label, "", LEN_LABEL);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_legs_get_prev     (int a_leg)
{
   /*---(locals)-----------+-----+-----+-*/
   tSERVO     *x_servo     =    0;
   char        rc          =    0;
   /*---(get previous)-------------------*/
   myKINE.cb  = yKINE_legdeg (a_leg);
   x_servo = ykine_servo_pointer (a_leg, YKINE_FEMU);
   rc  = ykine_move_savedtail  (x_servo, NULL, &myKINE.fb, NULL, NULL, NULL, NULL);
   x_servo = ykine_servo_pointer (a_leg, YKINE_PATE);
   rc  = ykine_move_savedtail  (x_servo, NULL, &myKINE.pb, NULL, NULL, NULL, NULL);
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   rc  = ykine_move_savedtail  (x_servo, NULL, &myKINE.tb, &myKINE.xb, &myKINE.zb, &myKINE.yb, &myKINE.xzb);
   /*---(override for body)--------------*/
   if (a_leg == YKINE_BODY) {
      x_servo = ykine_servo_pointer (a_leg, YKINE_FOCU);
      rc  = ykine_move_savedtail  (x_servo, NULL, NULL, &myKINE.xb, &myKINE.zb, &myKINE.yb, &myKINE.xzb);
   }
   /*---(adjust for foot)----------------*/
   else {
      myKINE.yb -= yKINE_seglen (YKINE_FOOT);
   }
   /*---(report)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("angles"    , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , myKINE.cb, myKINE.fb, myKINE.pb, myKINE.tb);
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", myKINE.xb, myKINE.zb, myKINE.yb, myKINE.xzb);
   /*---(complete)-----------------------*/
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_legs_set_zero    (char a_verb, float x, float z, float y, float a_beat, char *a_label, char a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("parms"     , "%dm, %6.2fx, %6.2fz, %6.2fy, %8.2fb", a_verb, x, z, y, a_beat);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * myKINE.s_pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s_secs"    , s);
   rc = ykine_move_create (x_servo, YKINE_SERVO, a_verb, myKINE.s_cline, a_label, a_cell, 0.0, s);
   rc = ykine_move_addloc (x_servo, x, z, y);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_legs_set_servo   (char a_verb, char a_leg, int a_seg, float a_deg, float a_beat, char *a_label, char a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   float       x, y, z;
   char        x_meth      = YKINE_IK;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("parms"     , "%dm, %dl, %ds, %8.2fd, %8.2fb, %s", a_verb, a_leg, a_seg, a_deg, a_beat, a_label);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * myKINE.s_pace;
   DEBUG_YKINE_SCRP  yLOG_value   ("s_secs"    , s);
   rc = ykine_move_create (x_servo, YKINE_SERVO, a_verb, myKINE.s_cline, a_label, a_cell, a_deg, s);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (a_verb) {
   case YKINE_OR : case YKINE_TI :
      break;
   default       :
      if (a_verb == YKINE_FK)  x_meth = YKINE_FK;
      rc = yKINE_endpoint    (a_leg, a_seg, x_meth, NULL, NULL, &x, &z, &y, NULL);
      DEBUG_YKINE_SCRP   yLOG_complex ("updated"   , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
      rc = ykine_move_addloc (x_servo, x, z, y);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_single       (char a_verb, char a_leg, float f, float p, float t, float b, char *a_label, char a_cell)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle)-------------------------*/
   ykine_legs_set_servo (a_verb, a_leg, YKINE_FEMU, f, b, ""     , YKINE_NONE);
   ykine_legs_set_servo (a_verb, a_leg, YKINE_PATE, p, b, ""     , YKINE_NONE);
   ykine_legs_set_servo (a_verb, a_leg, YKINE_TIBI, t, b, a_label, a_cell);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_complete     (char a_verb, char a_leg, float b, char *a_accel, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i           =    0;
   float       x_pct       =  0.0;
   float       x_dur       =  0.0;
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(normal)-------------------------*/
   if (b >= 0)  {
      DEBUG_YKINE_SCRP   yLOG_note    ("normal move");
      if (a_verb == YKINE_ZE || a_verb == YKINE_PO) {
         ykine_legs_set_zero  (a_verb, myKINE.xe, myKINE.ze, myKINE.ye, b, a_label, YKINE_NONE);
      } else {
         ykine_legs_single    (a_verb, a_leg, myKINE.fe, myKINE.pe, myKINE.te, b, a_label, YKINE_NONE);
      }
   }
   /*---(accelerated)--------------------*/
   else {
      DEBUG_YKINE_SCRP   yLOG_note    ("accelerated move");
      /*> ykine_accel_leg       (a_verb, a_leg, a_accel);                             <*/
      rc = ykine_accel_calc (a_verb, a_accel [1], a_accel [0], a_accel [2]);
      --rce;  if (rc <  0) {
         DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fd %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
         if (g_accel_info [i].dist < 0.1)   continue;
         if (i == ACCEL_TURTLE)  strlcpy (x_label, a_label, LEN_LABEL);
         else                    strlcpy (x_label, ""     , LEN_LABEL);
         x_pct = g_accel_info [i].pct;
         x_dur = g_accel_info [i].dur;
         rc = ykine_exact_pct_route (a_verb, a_leg, x_pct);
         rc = ykine_legs_partial    (a_verb, a_leg, 's');
         if (a_verb == YKINE_ZE || a_verb == YKINE_PO) {
            ykine_legs_set_zero  (a_verb, myKINE.xc, myKINE.zc, myKINE.yc, x_dur, x_label, g_accel_info [i].abbr);
         } else {
            ykine_legs_single    (a_verb, a_leg, myKINE.fc, myKINE.pc, myKINE.tc, x_dur, x_label, g_accel_info [i].abbr);
         }
         /*---(done)------------------------*/
      }
   }
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
/*===----                         fk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___FK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__legs_fk_getter   (int a_leg, char *f_str, char *p_str, char *t_str, float *f, float *p, float *t)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      fe, pe, te;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_legs_get_prev (a_leg);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (myKINE.s_from, myKINE.fb, f_str, &fe);
   rc = ykine_stance_scale   (myKINE.s_from, myKINE.pb, p_str, &pe);
   rc = ykine_stance_scale   (myKINE.s_from, myKINE.tb, t_str, &te);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("angles"    , "%8.2ff, %8.2fp, %8.2ft", fe, pe, te);
   /*---(save)---------------------------*/
   if (f != NULL)  *f = fe;
   if (p != NULL)  *p = pe;
   if (t != NULL)  *t = te;
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
   ykine_legs_get_prev (a_leg);
   /*---(convert to ck)---------------*/
   ykine_legs_ik2ck (myKINE.cb, myKINE.xb, myKINE.zb, &db, &ob);
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
   ykine_legs_ck2ik (myKINE.cb, de, oe, x, z);
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
   ykine_legs_get_prev (a_leg);
   ykine_legs_ik2rk (myKINE.cb, myKINE.xb, myKINE.zb, &db, &ob);
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
   ykine_legs_rk2ik (myKINE.cb, de, oe, x, z);
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

char
ykine_legs_ik2nk        (float a_coxa, float x, float z, float *nx, float *nz)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz;
   float       xi, zi;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fx, %8.2fz", a_coxa, x, z);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%pnx, %pnz", nx, nz);
   --rce;  if (nx == NULL || nz == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc inner triangle)---------*/
   xi      =  dxz  * cos (a_coxa * DEG2RAD);
   zi      = -dxz  * sin (a_coxa * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdxz, %8.2fx, %8.2fz", dxz, xi, zi);
   /*---(result)-------------------------*/
   *nx    =  x - xi;
   if (*nx > -0.001 && *nx < 0.001)  *nx = 0.000;
   *nz    =  z - zi;
   if (*nz > -0.001 && *nz < 0.001)  *nz = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fnx, %8.2fnz", *nx, *nz);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_legs_nk2ik        (float a_coxa, float nx, float nz, float *x, float *z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       dxz;
   float       xi, zi;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.2fc, %8.2fnx, %8.2fnz", a_coxa, nx, nz);
   DEBUG_YKINE_SCRP  yLOG_complex ("pointers"  , "%px, %pz", x, z);
   --rce;  if (x == NULL || z == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare constants)-----------*/
   dxz     = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   /*---(calc inner triangle)---------*/
   xi      =  dxz  * cos (a_coxa * DEG2RAD);
   zi      = -dxz  * sin (a_coxa * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdxz, %8.2fx, %8.2fz", dxz, xi, zi);
   /*---(result)-------------------------*/
   *x     =  nx + xi;
   if (*x > -0.001 && *x < 0.001)  *x = 0.000;
   *z     =  nz + zi;
   if (*z > -0.001 && *z < 0.001)  *z = 0.000;
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz", *x, *z);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine__legs_nk_getter   (int a_leg, char *x_str, char *z_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       xb, zb, yb;                  /* previous/begin values          */
   double      xe, ze, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (x != NULL)  *x = 0.0;
   if (z != NULL)  *z = 0.0;
   if (y != NULL)  *y = 0.0;
   /*---(prepare)---------------------*/
   ykine_legs_get_prev (a_leg);
   ykine_legs_ik2nk (myKINE.cb, myKINE.xb, myKINE.zb, &xb, &zb);
   yb      = myKINE.yb;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fx, %8.2fz, %8.2fy", xb, zb, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (myKINE.s_from, xb, x_str, &xe);
   rc = ykine_stance_scale   (myKINE.s_from, zb, z_str, &ze);
   rc = ykine_stance_scale   (myKINE.s_from, yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fx, %8.2fz, %8.2fy", xe, ze, ye);
   /*---(convert from rk)-------------*/
   ykine_legs_nk2ik (myKINE.cb, xe, ze, x, z);
   if (y != NULL)  *y = ye;
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
   ykine_legs_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2tk (myKINE.cb, myKINE.xb, myKINE.zb, &ob, &tb);
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
   ykine_legs_tk2ik (myKINE.cb, oe, te, x, z);
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
   ykine_legs_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2sk (myKINE.cb, myKINE.xb, myKINE.zb, &db, &ob);
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
   ykine_legs_sk2ik (myKINE.cb, de, oe, x, z);
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
   double      xe, ze, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_legs_get_prev (a_leg);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.xb, x_str, &xe);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.zb, z_str, &ze);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xe, ze, ye);
   /*---(save)---------------------------*/
   if (x != NULL)  *x = xe;
   if (z != NULL)  *z = ze;
   if (y != NULL)  *y = ye;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        overall driver                        ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVER__________________o (void) {;}

char
ykine__legs_fallback     (void)
{
   DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
   myKINE.xe = myKINE.xb;
   myKINE.ze = myKINE.zb;
   myKINE.ye = myKINE.yb;
   myKINE.ce = myKINE.cb;
   myKINE.fe = myKINE.fb;
   myKINE.pe = myKINE.pb;
   myKINE.te = myKINE.tb;
   return 0;
}

char         /*--> handle tangent verbs ------------------[ ------ [ ------ ]-*/
ykine_legs_driver    (char a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char        x_leg       =    0;
   char        x_seg       =    0;
   char       *x_one       [LEN_LABEL];
   char       *x_two       [LEN_LABEL];
   char       *x_thr       [LEN_LABEL];
   char       *x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare servos)--------------*/
   rc = ykine_legs_prepservos (a_verb);
   DEBUG_YKINE_SCRP   yLOG_value   ("servos"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark servers)----------------*/
   rc = ykine_legs_prepare (x_one, x_two, x_thr, x_label);
   DEBUG_YKINE_SCRP   yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)---------------------*/
   switch (a_verb) {
   case YKINE_FK :                 x_seg = YKINE_FEMU;  break;
   case YKINE_ZE : case YKINE_PO : x_seg = YKINE_FOCU;  break;
   case YKINE_OR : case YKINE_TI : x_seg = YKINE_YAW;   break;
   default       :                 x_seg = YKINE_TIBI;  break;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("x_seg"     , x_seg);
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = ykine_servo_find (x_leg, x_seg);
      if (rc < 0)                     continue;
      if (myKINE.servos [rc] == '_')  continue;
      /*---(get positions)---------------*/
      switch (a_verb) {
      case YKINE_ZE :  rc = ykine_body_ze_getter  (       x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_PO :  rc = ykine_body_po_getter  (       x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_OR :  rc = ykine_body_or_getter  (       x_one, x_two, x_thr, &myKINE.fe, &myKINE.pe, &myKINE.te);  break;
      case YKINE_TI :  rc = ykine_body_ti_getter  (       x_one, x_two, x_thr, &myKINE.fe, &myKINE.pe, &myKINE.te);  break;
      case YKINE_FK :  rc = ykine__legs_fk_getter (x_leg, x_one, x_two, x_thr, &myKINE.fe, &myKINE.pe, &myKINE.te);  break;
      case YKINE_IK :  rc = ykine__legs_ik_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_CK :  rc = ykine__legs_ck_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_RK :  rc = ykine__legs_rk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_TK :  rc = ykine__legs_tk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_NK :  rc = ykine__legs_nk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      case YKINE_SK :  rc = ykine__legs_sk_getter (x_leg, x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
      default       :
                       DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, verb type not known");
                       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
                       return rce;
      }
      DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
      if (rc <  0)   ykine__legs_fallback ();
      switch (a_verb) {
      case YKINE_OR : case YKINE_TI : case YKINE_FK :
         DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
         break;
      default       :
         DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", myKINE.xe, myKINE.ze, myKINE.ye);
         break;
      }
      /*---(inverse kinematics)----------*/
      switch (a_verb) {
      case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI :
         rc = 0;
         break;
      case YKINE_FK :
         rc = yKINE_forward (x_leg, myKINE.fe, myKINE.pe, myKINE.te);
         DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
         break;
      default       :
         rc = yKINE_inverse (x_leg, myKINE.xe, myKINE.ze, myKINE.ye);
         DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
         break;
      }
      if (rc <  0)   ykine__legs_fallback ();
      /*---(retrieve angles)-------------*/
      switch (a_verb) {
      case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI :
         rc = 0;
         break;
      case YKINE_FK :
         rc = yKINE_endpoint (x_leg, YKINE_FOOT, YKINE_FK, NULL, NULL, &myKINE.xe, &myKINE.ze, &myKINE.ye, NULL);
         DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", myKINE.xe, myKINE.ze, myKINE.ye);
         break;
      default       :
         rc = yKINE_angles   (x_leg, YKINE_IK, &myKINE.ce, &myKINE.fe, &myKINE.pe, &myKINE.te);
         DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
         break;
      }
      /*---(get distance)----------------*/
      ykine_exact_dist_route (a_verb);
      DEBUG_YKINE_SCRP  yLOG_double  ("distance"  , myKINE.le);
      /*---(process moves)---------------*/
      ykine_legs_complete     (a_verb, x_leg, myKINE.b, myKINE.accel, x_label);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       ykine_legs_fk   (void)  { return ykine_legs_driver (YKINE_FK); }
char       ykine_legs_ik   (void)  { return ykine_legs_driver (YKINE_IK); }
char       ykine_legs_ck   (void)  { return ykine_legs_driver (YKINE_CK); }
char       ykine_legs_rk   (void)  { return ykine_legs_driver (YKINE_RK); }
char       ykine_legs_tk   (void)  { return ykine_legs_driver (YKINE_TK); }
char       ykine_legs_nk   (void)  { return ykine_legs_driver (YKINE_NK); }
char       ykine_legs_sk   (void)  { return ykine_legs_driver (YKINE_SK); }



/*====================------------------------------------====================*/
/*===----                       partial moves                          ----===*/
/*====================------------------------------------====================*/
static void      o___PARTIAL_________________o (void) {;}

char
ykine_legs_partial      (char a_verb, char a_leg, char a_ik)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_complex ("params"    , "verb %d, leg %d, ik %c", a_verb, a_leg, a_ik);
   /*---(calculate)----------------------*/
   if (a_verb == YKINE_OR || a_verb == YKINE_TI) {
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(forward)------------------------*/
   else if (a_verb == YKINE_FK) {
      DEBUG_YKINE_EXACT   yLOG_note    ("forward positioning");
      DEBUG_YKINE_EXACT   yLOG_complex ("input"     , "%8.2ff, %8.2fp, %8.2ft", myKINE.fc, myKINE.pc, myKINE.tc);
      rc = yKINE_forward  (a_leg, myKINE.fc, myKINE.pc, myKINE.tc);
      yKINE_endpoint (a_leg, YKINE_FOOT, YKINE_FK, NULL, NULL, &myKINE.xc, &myKINE.zc, &myKINE.yc, NULL);
      DEBUG_YKINE_EXACT   yLOG_complex ("output"    , "%8.2fx, %8.2fz, %8.2fy", myKINE.xc, myKINE.zc, myKINE.yc);
   }
   /*---(inverse)------------------------*/
   else {
      DEBUG_YKINE_EXACT   yLOG_note    ("inverse positioning");
      DEBUG_YKINE_EXACT   yLOG_complex ("input"     , "%8.2fx, %8.2fz, %8.2fy", myKINE.xc, myKINE.zc, myKINE.yc);
      switch (a_ik) {
      case 'e' :
         DEBUG_YKINE_EXACT   yLOG_note    ("ticker/progress so adapt");
         rc = yKINE_inverse_adapt (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);
         break;
      case 's' :
         DEBUG_YKINE_EXACT   yLOG_note    ("script reading so do not adapt");
         rc = yKINE_inverse       (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);
         break;
      }
      yKINE_angles   (a_leg, YKINE_IK, NULL, &myKINE.fc, &myKINE.pc, &myKINE.tc);
      DEBUG_YKINE_EXACT   yLOG_complex ("output"    , "%8.2ff, %8.2fp, %8.2ft", myKINE.fc, myKINE.pc, myKINE.tc);
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("rc"        , rc);
   myKINE.rcc = rc;
   /*---(handle errors)------------------*/
   --rce;  if (rc < 0)  {
      DEBUG_YKINE_MOVE  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}



