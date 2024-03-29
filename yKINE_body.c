/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*> static tSERVO  *s_servo   = NULL;                                                 <*/
/*> static char     s_accel     [LEN_LABEL] = "";                                     <*/
/*> static double   b, s;               /+ input and adjusted durations           +/   <*/
/*> static float    yb, pb, rb, db, tb; /+ begin  yaw, pitch, roll, dir, and tilt +/   <*/
/*> static double   ye, pe, re, de, te; /+ end    yaw, pitch, roll, dir, and tilt +/   <*/



/*====================------------------------------------====================*/
/*===----                     zero-point movements                     ----===*/
/*====================------------------------------------====================*/
static void      o___ZERO____________________o (void) {;}

char
ykine_body_xz2po        (float a_xpos, float a_zpos, float *a_deg, float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   float       r           = 0.0;
   float       d           = 0.0;
   float       l           = 0.0;
   /*---(fix degree)---------------------*/
   r = atan2 (-a_zpos, a_xpos);
   d = r * RAD2DEG;
   d = round (d * 10.0) / 10.0;
   while (d <    0.0)   d += 360.0;
   while (d >  360.0)   d -= 360.0;
   /*---(fix length)---------------------*/
   l = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   l = round (l * 10.0) / 10.0;
   if (l < 0.1 && l > -0.1)   d = l = 0.0;
   /*---(save results)-------------------*/
   if (a_deg  != NULL)  *a_deg  = d;
   if (a_len  != NULL)  *a_len  = l;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_body_po2xz        (float a_deg, float a_len, float *a_xpos, float *a_zpos)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x           = 0.0;
   float       z           = 0.0;
   /*---(fix degree)---------------------*/
   while (a_deg <    0.0)   a_deg += 360.0;
   while (a_deg >  360.0)   a_deg -= 360.0;
   /*---(set coordinates)----------------*/
   x =  a_len * cos (a_deg * DEG2RAD);
   z = -a_len * sin (a_deg * DEG2RAD);
   x = round (x * 10.0) / 10.0;
   z = round (z * 10.0) / 10.0;
   if (x < 0.1 && x > -0.1)   x = 0.0;
   if (z < 0.1 && z > -0.1)   z = 0.0;
   /*---(save results)-------------------*/
   if (a_xpos != NULL)  *a_xpos = x;
   if (a_zpos != NULL)  *a_zpos = z;
   /*---(complete)-----------------------*/
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_body_ze_getter    (char *x_str, char *z_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      xe, ze, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_exec_get_prev (YKINE_BODY);
   /*---(calculate)-------------------*/
   rc = ykine_stance_scale   (          g_beg.ex , x_str, &xe);
   rc = ykine_stance_scale   (          g_beg.ez , z_str, &ze);
   rc = ykine_stance_height  (YKINE_ZE, g_beg.ey , y_str, &ye);
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

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine_body_po_getter   (char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   double      de, oe, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_exec_get_prev (YKINE_BODY);
   /*---(convert to po)---------------*/
   ykine_body_xz2po (g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", g_beg.deg, g_beg.out, g_beg.ey );
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (          g_beg.deg, d_str, &de);
   rc = ykine_stance_scale   (          g_beg.out, o_str, &oe);
   rc = ykine_stance_height  (YKINE_PO, g_beg.ey , y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(save off)--------------------*/
   g_end.deg = de;
   g_end.out = oe;
   /*---(convert from po)-------------*/
   ykine_body_po2xz (g_end.deg, g_end.out, x, z);
   if (y != NULL)  *y = ye;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char   ykine_body_zero         (int n, uchar *a_verb) { return ykine_exec_driver (n, a_verb, YKINE_ZE); }
char   ykine_body_polar        (int n, uchar *a_verb) { return ykine_exec_driver (n, a_verb, YKINE_PO); }



/*====================------------------------------------====================*/
/*===----                     orientation movements                    ----===*/
/*====================------------------------------------====================*/
static void      o___ORIENT__________________o (void) {;}

char         /*--> check orientation limits --------------[ ------ [ ------ ]-*/
ykine_body_orient_valid (float y, float p, float r)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(test yaw)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sdouble (y);
   --rce;  if (y > 40.0 || y < -40.0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("yaw out of range");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(test pitch)---------------------*/
   DEBUG_YKINE_SCRP   yLOG_sdouble (p);
   --rce;  if (p > 30.0 || p < -30.0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("pitch out of range");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(test roll)----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sdouble (r);
   --rce;  if (r > 30.0 || r < -30.0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("roll out of range");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

float
ykine_round_val     (float a)
{
   a = round (a * 10.0) / 10.0;
   if (a > -0.1 && a < 0.1)  a = 0.0;
   return a;
}

float
ykine_round_deg     (float a)
{
   a = round (a * 10.0) / 10.0;
   if (a > -0.1 && a < 0.1)  a = 0.0;
   while (a <= -180.0)   a += 360.0;
   while (a >   180.0)   a -= 360.0;
   return a;
}

char         /*--> convert orientation into position -----[ ------ [ ------ ]-*/
ykine_body_ypr2xzy      (float a_yaw, float a_pitch, float a_roll, float *a_x, float *a_z, float *a_y, float *a_l)
{  /*---(design notes)-------------------*/
   /*  returns highest point on body, so y is always returned positive.
    *  yaw, pitch, and roll ranges must be checked *before* this function.
    */
   /*---(locals)-----------+-----+-----+-*/
   float       h, a, d, da, t, x, z, y, l;
   /*---(convert)------------------------*/
   ykine_body_pr2ti       (a_pitch, a_roll, &d, &t);
   da  = 90.0 - (d + a_yaw);
   /*> printf ("%8.2fy, %8.2fp, %8.2fr, %8.2fd, %8.2ft, %8.2fda\n", a_yaw, a_pitch, a_roll, d, t, da);   <*/
   /*---(vertical)-----------------------*/
   h   = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   y   = ykine_round_val ( h * sin (t  * DEG2RAD));
   /*> printf ("           %8.2fh, %8.2fy\n", h, y);                                  <*/
   /*---(horizontal)---------------------*/
   h   = h * cos (t * DEG2RAD);
   x   = ykine_round_val ( h * cos (da * DEG2RAD));
   z   = ykine_round_val (-h * sin (da * DEG2RAD));
   /*---(length as check)----------------*/
   l   = ykine_round_val (sqrt ((x * x) + (z * z) + (y * y)));
   /*> printf ("           %8.2fh, %8.2fx, %8.2fz, %8.2fl\n", h, x, z, l);            <*/
   /*---(save returns)-------------------*/
   if (a_x != NULL)  *a_x  = x;
   if (a_z != NULL)  *a_z  = z;
   if (a_y != NULL)  *a_y  = y;
   if (a_l != NULL)  *a_l  = l;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> convert dir/tilt to orientation -------[ ------ [ ------ ]-*/
ykine_body_ti2pr        (float d, float t, float *p, float *r)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_pitch     =    0;
   float       x_roll      =    0;
   /*---(calculate)----------------------*/
   x_pitch =  t * cos (d * DEG2RAD);
   x_roll  =  t * sin (d * DEG2RAD);
   /*---(fix and round)------------------*/
   x_pitch = round (x_pitch * 10.0) / 10.0;
   x_roll  = round (x_roll  * 10.0) / 10.0;
   if (x_pitch > -0.1 && x_pitch < 0.1)  x_pitch = 0.0;
   if (x_roll  > -0.1 && x_roll  < 0.1)  x_roll  = 0.0;
   while (x_pitch <= -180.0)   x_pitch += 360.0;
   while (x_pitch >   180.0)   x_pitch -= 360.0;
   while (x_roll  <= -180.0)   x_roll  += 360.0;
   while (x_roll  >   180.0)   x_roll  -= 360.0;
   /*---(save returns)-------------------*/
   if (p != NULL)  *p = x_pitch;
   if (r != NULL)  *r = x_roll;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> convert orientation to dir/tilt -------[ ------ [ ------ ]-*/
ykine_body_pr2ti        (float p, float r, float *d, float *t)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_len       =    0;
   float       x_dir       =    0;
   /*---(calculate)----------------------*/
   x_len = sqrt ((p * p) + (r * r));
   x_dir = RAD2DEG * atan2 (r, p);
   /*---(fix and round)------------------*/
   x_len = round (x_len * 10.0) / 10.0;
   x_dir = round (x_dir * 10.0) / 10.0;
   while (x_dir <= -180.0)   x_dir += 360.0;
   while (x_dir >   180.0)   x_dir -= 360.0;
   /*---(save returns)-------------------*/
   if (d != NULL)  *d = x_dir;
   if (t != NULL)  *t = x_len;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_body_or_getter   (char *f_str, char *p_str, char *t_str, float *f, float *p, float *t)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      fe, pe, te;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_exec_get_prev (YKINE_BODY);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.2fp, %8.2ft", g_beg.fd , g_beg.pd , g_beg.td );
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (g_beg.fd , f_str, &fe);
   rc = ykine_stance_scale   (g_beg.pd , p_str, &pe);
   rc = ykine_stance_scale   (g_beg.td , t_str, &te);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3ff, %8.2fp, %8.2tr", fe, pe, te);
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

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_body_ti_getter   (char *f_str, char *p_str, char *t_str, float *f, float *p, float *t)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       db, tb;
   double      fe, de, te;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get previous)-------------------*/
   ykine_exec_get_prev (YKINE_BODY);
   /*---(convert to dir/tilt)------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3ff, %8.2fp, %8.2ft", g_beg.fd , g_beg.pd , g_beg.td );
   rc = ykine_body_pr2ti       (g_beg.pd , g_beg.td , &db, &tb);
   DEBUG_YKINE_SCRP  yLOG_complex ("converted" , "%8.2fd, %8.2ft", db, tb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (g_beg.fd , f_str, &fe);
   rc = ykine_stance_scale   (db       , p_str, &de);
   rc = ykine_stance_scale   (tb       , t_str, &te);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3ff, %8.2fd, %8.2ft", fe, de, te);
   /*---(convert back to pitch/roll)-----*/
   rc = ykine_body_ti2pr        (de, te, &g_end.pd , &g_end.td );
   DEBUG_YKINE_SCRP  yLOG_complex ("converted" , "%8.2fp, %8.2fr", g_end.pd , g_end.td );
   if (f != NULL)  *f = fe;
   g_beg.deg = g_end.deg = 0.0;
   g_beg.out = g_end.out = 0.0;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char   ykine_body_orient       (int n, uchar *a_verb) { return ykine_exec_driver (n, a_verb, YKINE_OR); }
char   ykine_body_tilt         (int n, uchar *a_verb) { return ykine_exec_driver (n, a_verb, YKINE_TI); }



