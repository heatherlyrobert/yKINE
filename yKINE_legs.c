/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

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
   ykine_exec_get_prev (a_leg);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (g_beg.fd , f_str, &fe);
   rc = ykine_stance_scale   (g_beg.pd , p_str, &pe);
   rc = ykine_stance_scale   (g_beg.td , t_str, &te);
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
   g_beg.deg = g_end.deg = 0.0;
   g_beg.out = g_end.out = 0.0;
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
   ykine_exec_get_prev (a_leg);
   /*---(convert to ck)---------------*/
   ykine_legs_ik2ck (g_beg.cd , g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", g_beg.deg, g_beg.out, g_beg.ey );
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (a_leg , g_beg.deg, d_str, &de);
   rc = ykine_stance_radius  (        g_beg.out, o_str, &oe);
   rc = ykine_stance_scale   (        g_beg.ey , y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from cx)-------------*/
   ykine_legs_ck2ik (g_beg.cd , de, oe, x, z);
   if (y != NULL)  *y = ye;
   g_end.deg = de;
   g_end.out = oe;
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
   DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fdxzi, %8.2fxi, %8.2fzi", dxzi, xi, zi);
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
   ykine_exec_get_prev (a_leg);
   ykine_legs_ik2rk (g_beg.cd , g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", g_beg.deg, g_beg.out, g_beg.ey );
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (a_leg , g_beg.deg, d_str, &de);
   rc = ykine_stance_radius  (        g_beg.out, o_str, &oe);
   rc = ykine_stance_scale   (        g_beg.ey , y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from rk)-------------*/
   ykine_legs_rk2ik (g_beg.cd , de, oe, x, z);
   if (y != NULL)  *y = ye;
   g_end.deg = de;
   g_end.out = oe;
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
   ykine_exec_get_prev (a_leg);
   ykine_legs_ik2nk (g_beg.cd , g_beg.ex , g_beg.ez , &xb, &zb);
   yb      = g_beg.ey ;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fx, %8.2fz, %8.2fy", xb, zb, g_beg.ey );
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
   ykine_legs_nk2ik (g_beg.cd , xe, ze, x, z);
   if (y != NULL)  *y = ye;
   g_beg.deg = g_end.deg = 0.0;
   g_beg.out = g_end.out = 0.0;
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
   ykine_exec_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2tk (g_beg.cd , g_beg.ex , g_beg.ez , &ob, &tb);
   yb = g_beg.ey ;
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
   ykine_legs_tk2ik (g_beg.cd , oe, te, x, z);
   if (y != NULL)  *y = ye;
   g_beg.deg = g_end.deg = 0.0;
   g_beg.out = g_end.out = 0.0;
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
   ykine_exec_get_prev (a_leg);
   /*---(convert to tk)---------------*/
   ykine_legs_ik2sk (g_beg.cd , g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", g_beg.deg, g_beg.out, g_beg.ey );
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_spread  (a_leg , g_beg.deg, d_str, &de);
   rc = ykine_stance_scale   (                       g_beg.out, o_str, &oe);
   rc = ykine_stance_scale   (                       g_beg.ey , y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from tx)-------------*/
   ykine_legs_sk2ik (g_beg.cd , de, oe, x, z);
   if (y != NULL)  *y = ye;
   g_end.deg = de;
   g_end.out = oe;
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
   ykine_exec_get_prev (a_leg);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (                       g_beg.ex , x_str, &xe);
   rc = ykine_stance_scale   (                       g_beg.ez , z_str, &ze);
   rc = ykine_stance_scale   (                       g_beg.ey , y_str, &ye);
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
   g_beg.deg = g_end.deg = 0.0;
   g_beg.out = g_end.out = 0.0;
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
   g_end.ex  = g_beg.ex ;
   g_end.ez  = g_beg.ez ;
   g_end.ey  = g_beg.ey ;
   g_end.cd  = g_beg.cd ;
   g_end.fd  = g_beg.fd ;
   g_end.pd  = g_beg.pd ;
   g_end.td  = g_beg.td ;
   return 0;
}

char       ykine_legs_fk   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_FK); }
char       ykine_legs_ik   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_IK); }
char       ykine_legs_ck   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_CK); }
char       ykine_legs_rk   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_RK); }
char       ykine_legs_tk   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_TK); }
char       ykine_legs_nk   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_NK); }
char       ykine_legs_sk   (int n, uchar *a_verb)  { return ykine_exec_driver (n, a_verb, YKINE_SK); }



