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
ykine_legs_prepare      (char *a_one, char *a_two, char *a_thr, char *a_label, char *a_mods)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get timing)------------------*/
   ykine_accel_timing ();
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
   /*---(get optional modifiers)------*/
   if (myKINE.b < 0.0) {
      rc = yPARSE_popstr    (a_mods);
      DEBUG_YKINE_SCRP  yLOG_complex ("a_mods"    , "%3d, %s", rc, a_mods);
      if (strcmp (a_mods , "-") == 0) {
         strlcpy (a_mods , "", LEN_LABEL);
      }
      ykine_stepping    (a_mods);
   } else ykine_stepping    ("");
   ykine_step_accels ();
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
   rc = ykine_stance_scale   (myKINE.fb, f_str, &fe);
   rc = ykine_stance_scale   (myKINE.pb, p_str, &pe);
   rc = ykine_stance_scale   (myKINE.tb, t_str, &te);
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
   myKINE.db = myKINE.de = 0.0;
   myKINE.ob = myKINE.oe = 0.0;
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
   double      de, oe, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_legs_get_prev (a_leg);
   /*---(convert to ck)---------------*/
   ykine_legs_ik2ck (myKINE.cb, myKINE.xb, myKINE.zb, &myKINE.db, &myKINE.ob);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", myKINE.db, myKINE.ob, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (a_leg , myKINE.db, d_str, &de);
   rc = ykine_stance_radius  (        myKINE.ob, o_str, &oe);
   rc = ykine_stance_scale   (        myKINE.yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from cx)-------------*/
   ykine_legs_ck2ik (myKINE.cb, de, oe, x, z);
   if (y != NULL)  *y = ye;
   myKINE.de = de;
   myKINE.oe = oe;
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
   double      de, oe, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (x != NULL)  *x = 0.0;
   if (z != NULL)  *z = 0.0;
   if (y != NULL)  *y = 0.0;
   /*---(prepare)---------------------*/
   ykine_legs_get_prev (a_leg);
   ykine_legs_ik2rk (myKINE.cb, myKINE.xb, myKINE.zb, &myKINE.db, &myKINE.ob);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", myKINE.db, myKINE.ob, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (a_leg , myKINE.db, d_str, &de);
   rc = ykine_stance_radius  (        myKINE.ob, o_str, &oe);
   rc = ykine_stance_scale   (        myKINE.yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from rk)-------------*/
   ykine_legs_rk2ik (myKINE.cb, de, oe, x, z);
   if (y != NULL)  *y = ye;
   myKINE.de = de;
   myKINE.oe = oe;
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
   rc = ykine_stance_scale   (xb, x_str, &xe);
   rc = ykine_stance_scale   (zb, z_str, &ze);
   rc = ykine_stance_scale   (yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fx, %8.2fz, %8.2fy", xe, ze, ye);
   /*---(convert from rk)-------------*/
   ykine_legs_nk2ik (myKINE.cb, xe, ze, x, z);
   if (y != NULL)  *y = ye;
   myKINE.db = myKINE.de = 0.0;
   myKINE.ob = myKINE.oe = 0.0;
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
   rc = ykine_stance_radius  (ob, o_str, &oe);
   rc = ykine_stance_scale   (tb, t_str, &te);
   rc = ykine_stance_scale   (yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fo, %8.2ft, %8.2fy", oe, te, ye);
   /*---(convert from tx)-------------*/
   ykine_legs_tk2ik (myKINE.cb, oe, te, x, z);
   if (y != NULL)  *y = ye;
   myKINE.db = myKINE.de = 0.0;
   myKINE.ob = myKINE.oe = 0.0;
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
   double      de, oe, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_legs_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2sk (myKINE.cb, myKINE.xb, myKINE.zb, &myKINE.db, &myKINE.ob);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", myKINE.db, myKINE.ob, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (a_leg , myKINE.db, d_str, &de);
   rc = ykine_stance_scale   (                       myKINE.ob, o_str, &oe);
   rc = ykine_stance_scale   (                       myKINE.yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from tx)-------------*/
   ykine_legs_sk2ik (myKINE.cb, de, oe, x, z);
   if (y != NULL)  *y = ye;
   myKINE.de = de;
   myKINE.oe = oe;
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
   rc = ykine_stance_scale   (                       myKINE.xb, x_str, &xe);
   rc = ykine_stance_scale   (                       myKINE.zb, z_str, &ze);
   rc = ykine_stance_scale   (                       myKINE.yb, y_str, &ye);
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
   myKINE.db = myKINE.de = 0.0;
   myKINE.ob = myKINE.oe = 0.0;
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
ykine_legs_driver       (int n, uchar *v, char a_code)
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
   char       *x_mods      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("args"      , "%dn, %s, %dc", n, v, a_code);
   /*---(prepare verb)----------------*/
   rc = ykine_scrp_verb (v);
   DEBUG_YKINE_SCRP   yLOG_value   ("verb"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare servos)--------------*/
   rc = ykine_legs_prepservos (a_code);
   DEBUG_YKINE_SCRP   yLOG_value   ("servos"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark servers)----------------*/
   rc = ykine_legs_prepare (x_one, x_two, x_thr, x_label, x_mods);
   DEBUG_YKINE_SCRP   yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)---------------------*/
   switch (a_code) {
   case YKINE_FK :                 x_seg = YKINE_FEMU;  break;
   case YKINE_ZE : case YKINE_PO : x_seg = YKINE_FOCU;  break;
   case YKINE_OR : case YKINE_TI : x_seg = YKINE_YAW;   break;
   default       :                 x_seg = YKINE_TIBI;  break;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("x_seg"     , x_seg);
   myKINE.s_tline = n;
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = yKINE_servo_index (x_leg, x_seg);
      if (rc < 0)                     continue;
      if (myKINE.servos [rc] == '_')  continue;
      DEBUG_YKINE_SCRP   yLOG_complex ("LEG"       , "%d, %s, %s", x_leg, yKINE_legtwo (x_leg), yKINE_legfull (x_leg));
      /*---(get positions)---------------*/
      switch (a_code) {
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
      switch (a_code) {
      case YKINE_OR : case YKINE_TI : case YKINE_FK :
         DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
         break;
      default       :
         DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", myKINE.xe, myKINE.ze, myKINE.ye);
         break;
      }
      /*---(inverse kinematics)----------*/
      switch (a_code) {
      case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI :
         rc = 0;
         break;
      case YKINE_FK :
         rc = yKINE_forward (x_leg, myKINE.fe, myKINE.pe, myKINE.te);
         DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
         break;
      default       :
         ykine_tick_setbody (myKINE.se);
         rc = yKINE_adapt   (x_leg, myKINE.xe, myKINE.ze, myKINE.ye);
         /*> rc = yKINE_inverse (x_leg, myKINE.xe, myKINE.ze, myKINE.ye);             <*/
         DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
         break;
      }
      if (rc <  0)   ykine__legs_fallback ();
      /*---(retrieve angles)-------------*/
      switch (a_code) {
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
      /*---(if step, create raise)-------*/
      rc = ykine_accel_reset (x_leg);
      rc = ykine_step_begin  ();
      rc = ykine_step_raise  (a_code);
      /*---(get distance)----------------*/
      ykine_exact_dist_route (a_code);
      DEBUG_YKINE_SCRP  yLOG_double  ("distance"  , myKINE.le);
      /*---(process moves)---------------*/
      DEBUG_YKINE_SCRP   yLOG_complex ("beg"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xb, myKINE.zb, myKINE.yb);
      DEBUG_YKINE_SCRP   yLOG_complex ("end"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xe, myKINE.ze, myKINE.ye);
      DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
      if (myKINE.b >= 0)  ykine_accel_immediate   (a_code, x_leg, myKINE.b    , x_label);
      else                ykine_accel_append      (a_code, 'm', myKINE.a_middle);
      /*---(if step, create plant)-------*/
      rc = ykine_step_plant    (a_code);
      rc = ykine_accel_execute (x_label);
      rc = ykine_step_end      ();
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       ykine_legs_fk   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_FK); }
char       ykine_legs_ik   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_IK); }
char       ykine_legs_ck   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_CK); }
char       ykine_legs_rk   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_RK); }
char       ykine_legs_tk   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_TK); }
char       ykine_legs_nk   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_NK); }
char       ykine_legs_sk   (int n, char *v)  { return ykine_legs_driver (n, v, YKINE_SK); }



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
         rc = yKINE_adapt   (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);
         break;
      case 's' :
         DEBUG_YKINE_EXACT   yLOG_note    ("script reading so do not adapt");
         rc = yKINE_adapt   (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);
         /*> rc = yKINE_inverse (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);             <*/
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



