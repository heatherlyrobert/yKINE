/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



static float s_sec    = 0.0;



/*====================------------------------------------====================*/
/*===----                      shared calculations                     ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char
ykine_exact_calc        (char a_type, float a_beg, float a_end, float a_pct, float *a_cur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =   -10;
   double      x_range     = 0.0;
   double      x_inc       = 0.0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_cur == NULL)     return rce;
   /*---(fix degree range)---------------*/
   if (a_type == 'd') {
      while (a_beg > a_end)    a_end = 360.0 + a_end;
   }
   /*---(calculate)----------------------*/
   if (a_beg <= a_end) {
      x_range = a_end - a_beg;
      x_inc   = x_range * a_pct;
      *a_cur  = a_beg + x_inc;
   } else {
      x_range = a_beg - a_end;
      x_inc   = x_range * a_pct;
      *a_cur  = a_beg - x_inc;
   }
   /*---(limits on degrees)--------------*/
   if (a_type == 'd') {
      while (*a_cur <    0.0)   *a_cur  = 360.0 + *a_cur;
      while (*a_cur >  360.0)   *a_cur  = *a_cur - 360.0;
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         distances                            ----===*/
/*====================------------------------------------====================*/
static void      o___DISTANCE________________o (void) {;}

float        /*--> pure coordinate move distance ---------[ ------ [ ------ ]-*/
ykine_exact_dist_xzy    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   float       xd, zd, yd, ld;
   /*---(full beg/end)-------------------*/
   xd  = g_end.ex  - g_beg.ex;
   zd  = g_end.ez  - g_beg.ez ;
   yd  = g_end.ey  - g_beg.ey ;
   /*---(length)-------------------------*/
   ld = sqrt ((xd * xd) + (zd * zd) + (yd * yd));
   g_end.len  = ld;
   myKINE.lxz = sqrt ((xd * xd) + (zd * zd));
   /*---(complete)-----------------------*/
   return ld;
}

float        /*--> leg polar move distance ---------------[ ------ [ ------ ]-*/
ykine_exact_dist_doy    (char a_verb)
{  /* ´ arc distance = 2÷r is full circum and * (deg/360) cuts our part
    * ´ calc the arc length, then treat it as a triangle side for distance
    * ´ less than 90 deg, the difference is a max of 5% greater length
    */
   /*---(locals)-----------+-----+-----+-*/
   float       dd, od, yd, ld;
   float       dxz, od2;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(full beg/end)-------------------*/
   od  = g_end.out - g_beg.out;
   switch (a_verb) {
   case YKINE_PO :
      dxz  = 0.0;
      break;
   case YKINE_CK :
      dxz  = 0.0;
      break;
   case YKINE_RK :
      dxz  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
      break;
   case YKINE_SK :
      dxz  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
      break;
   }
   od2 = (dxz * 2.0 + g_beg.out + g_end.out) / 2.0;
   /*> od2 = (g_beg.out + g_end.out) / 2.0;                                           <*/
   DEBUG_YKINE   yLOG_complex ("out"       , "ob  %6.1f, oe  %6.1f, od  %6.1f, dxz %6.1f, od2 %6.1f", g_beg.out, g_end.out, od, dxz, od2);
   dd  = ((g_end.deg - g_beg.deg) / 360.0) * 2.0 * 3.1415927 * od2;
   DEBUG_YKINE   yLOG_complex ("deg"       , "db  %6.1f, de  %6.1f, dif %6.1f, 360 %6.1f, dd  %6.1f", g_beg.deg, g_end.deg, g_end.deg - g_beg.deg, (g_end.deg - g_beg.deg) / 360.0, ((g_end.deg - g_beg.deg) / 360.0) * 2.0 * 3.1415927, dd);
   yd  = g_end.ey  - g_beg.ey ;
   DEBUG_YKINE   yLOG_complex ("y"         , "yb  %6.1f, ye  %6.1f, yd  %6.1f", g_beg.ey , g_end.ey , yd);
   /*---(length)-------------------------*/
   ld  = sqrt ((dd * dd) + (od * od) + (yd * yd));
   g_end.len  = ld;
   myKINE.lxz = sqrt ((dd * dd) + (od * od));
   DEBUG_YKINE   yLOG_complex ("doy"       , "dd  %6.1f, od  %6.1f, yd  %6.1f, ld %6.1f", dd, od, yd, ld);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return ld;
}

float        /*--> leg polar move distance ---------------[ ------ [ ------ ]-*/
ykine_exact_dist_ypr    (void)
{  /* ´ arc distance = 2÷r is full circum and * (deg/360) cuts our part
    * ´ calc the arc length, then treat it as a triangle side for distance
    * ´ less than 90 deg, the difference is a max of 5% greater length
    */
   /*---(locals)-----------+-----+-----+-*/
   float       cd, fd, pd, td, ld;
   /*---(full beg/end)-------------------*/
   cd  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   fd  = ((g_end.fd  - g_beg.fd ) / 360.0) * 2.0 * 3.1415927 * cd;
   pd  = ((g_end.pd  - g_beg.pd ) / 360.0) * 2.0 * 3.1415927 * cd;
   td  = ((g_end.td  - g_beg.td ) / 360.0) * 2.0 * 3.1415927 * cd;
   /*---(length)-------------------------*/
   ld  = sqrt ((fd * fd) + (pd * pd) + (td * td));
   g_end.len  = ld;
   myKINE.lxz = 0.0;
   /*---(complete)-----------------------*/
   return ld;
}

char
ykine_exact_dist_route  (char a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       x_total     =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_value   ("a_verb"    , a_verb);
   --rce;  switch (a_verb) {
   case YKINE_OR : case YKINE_TI :
      x_total = ykine_exact_dist_ypr    ();
      break;
   case YKINE_ZE :
      x_total = ykine_exact_dist_xzy    ();
      break;
   case YKINE_PO :
      x_total = ykine_exact_dist_doy    (a_verb);
      break;
   case YKINE_FK :
      x_total = ykine_exact_dist_xzy    ();
      break;
   case YKINE_IK : case YKINE_TK : case YKINE_NK :
      x_total = ykine_exact_dist_xzy    ();
      break;
   case YKINE_CK : case YKINE_RK : case YKINE_SK :
      x_total = ykine_exact_dist_doy    (a_verb);
      /*> x_total = ykine_exact_dist_xzy    ();                                       <*/
      break;
   case YKINE_SYNC : case YKINE_LEGE : case YKINE_BODE : case YKINE_NOOP :
      x_total = ykine_exact_dist_xzy    ();
      break;
   default       :
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_YKINE   yLOG_value   ("x_total"   , x_total);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         percentages                          ----===*/
/*====================------------------------------------====================*/
static void      o___PERCENT_________________o (void) {;}

char
ykine_exact_pct_xzy     (float a_pct)
{
   /*---(calculate)----------------------*/
   myKINE.pct = a_pct;
   ykine_exact_calc ('-', g_beg.sec, g_end.sec, a_pct, &g_cur.sec);
   ykine_exact_calc ('-', g_beg.ex , g_end.ex , a_pct, &g_cur.ex );
   ykine_exact_calc ('-', g_beg.ez , g_end.ez , a_pct, &g_cur.ez );
   ykine_exact_calc ('-', g_beg.ey , g_end.ey , a_pct, &g_cur.ey );
   ykine_exact_calc ('-', g_beg.fd , g_end.fd , a_pct, &g_cur.fd );
   ykine_exact_calc ('-', g_beg.pd , g_end.pd , a_pct, &g_cur.pd );
   ykine_exact_calc ('-', g_beg.td , g_end.td , a_pct, &g_cur.td );
   DEBUG_YKINE   yLOG_complex ("linear"    , "%8.2fx, %8.2fz, %8.2fy", g_cur.ex , g_cur.ez , g_cur.ey );
   DEBUG_YKINE   yLOG_complex ("angles"    , "%8.2ff, %8.2fp, %8.2ft", g_cur.fd , g_cur.pd , g_cur.td );
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_pct_doy     (char a_verb, float a_pct)
{
   myKINE.pct = a_pct;
   ykine_exact_calc ('-', g_beg.sec, g_end.sec, a_pct, &g_cur.sec);
   switch (a_verb) {
   case YKINE_PO :
      DEBUG_YKINE   yLOG_note    ("zero polar kinematics");
      ykine_body_xz2po (           g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
      ykine_body_xz2po (           g_end.ex , g_end.ez , &g_end.deg, &g_end.out);
      break;
   case YKINE_CK :
      DEBUG_YKINE   yLOG_note    ("center kinematics");
      ykine_legs_ik2ck (g_cur.cd , g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
      ykine_legs_ik2ck (g_cur.cd , g_end.ex , g_end.ez , &g_end.deg, &g_end.out);
      break;
   case YKINE_RK :
      DEBUG_YKINE   yLOG_note    ("radial kinematics");
      ykine_legs_ik2rk (g_cur.cd , g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
      ykine_legs_ik2rk (g_cur.cd , g_end.ex , g_end.ez , &g_end.deg, &g_end.out);
      break;
   case YKINE_SK :
      DEBUG_YKINE   yLOG_note    ("step kinematics");
      ykine_legs_ik2sk (g_cur.cd , g_beg.ex , g_beg.ez , &g_beg.deg, &g_beg.out);
      ykine_legs_ik2sk (g_cur.cd , g_end.ex , g_end.ez , &g_end.deg, &g_end.out);
      break;
   }
   DEBUG_YKINE   yLOG_complex ("begin"     , "%8.2fx, %8.2fz, %8.2fd, %8.2fo", g_beg.ex , g_beg.ex , g_beg.deg, g_beg.out);
   DEBUG_YKINE   yLOG_complex ("end"       , "%8.2fx, %8.2fz, %8.2fd, %8.2fo", g_end.ex , g_end.ex , g_end.deg, g_end.out);
   ykine_exact_calc ('-', g_beg.deg, g_end.deg, a_pct, &g_cur.deg);
   ykine_exact_calc ('-', g_beg.out, g_end.out, a_pct, &g_cur.out);
   ykine_exact_calc ('-', g_beg.ey , g_end.ey , a_pct, &g_cur.ey );
   DEBUG_YKINE   yLOG_complex ("polar"     , "%8.2fd, %8.2fo, %8.2fy", g_cur.deg, g_cur.out, g_cur.ey );
   switch (a_verb) {
   case YKINE_PO :  ykine_body_po2xz (           g_cur.deg, g_cur.out, &g_cur.ex , &g_cur.ez );  break;
   case YKINE_CK :  ykine_legs_ck2ik (g_cur.cd , g_cur.deg, g_cur.out, &g_cur.ex , &g_cur.ez );  break;
   case YKINE_RK :  ykine_legs_rk2ik (g_cur.cd , g_cur.deg, g_cur.out, &g_cur.ex , &g_cur.ez );  break;
   case YKINE_SK :  ykine_legs_sk2ik (g_cur.cd , g_cur.deg, g_cur.out, &g_cur.ex , &g_cur.ez );  break;
   }
   DEBUG_YKINE   yLOG_complex ("endpoint"  , "%8.2fx, %8.2fz, %8.2fy", g_cur.ex , g_cur.ez , g_cur.ey );
   ykine_exact_calc ('-', g_beg.fd , g_end.fd , a_pct, &g_cur.fd );
   ykine_exact_calc ('-', g_beg.pd , g_end.pd , a_pct, &g_cur.pd );
   ykine_exact_calc ('-', g_beg.td , g_end.td , a_pct, &g_cur.td );
   DEBUG_YKINE   yLOG_complex ("angles"    , "%8.2ff, %8.2fp, %8.2ft", g_cur.fd , g_cur.pd , g_cur.td );
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_pct_ypr     (float a_pct)
{
   myKINE.pct = a_pct;
   ykine_exact_calc ('-', g_beg.sec, g_end.sec, a_pct, &g_cur.sec);
   ykine_exact_calc ('-', g_beg.fd , g_end.fd , a_pct, &g_cur.fd );
   ykine_exact_calc ('-', g_beg.pd , g_end.pd , a_pct, &g_cur.pd );
   ykine_exact_calc ('-', g_beg.td , g_end.td , a_pct, &g_cur.td );
   DEBUG_YKINE   yLOG_complex ("angles"    , "%8.2ff, %8.2fp, %8.2ft", g_cur.fd , g_cur.pd , g_cur.td );
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_pct_route   (char a_verb, float a_pct)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_complex ("params"    , "verb %d, pct %5.3f", a_verb, a_pct);
   /*---(find interim point)-------------*/
   --rce;  switch (a_verb) {
   case YKINE_OR : case YKINE_TI :
      DEBUG_YKINE   yLOG_note    ("orient angular partial");
      ykine_exact_pct_ypr     (a_pct);
      break;
   case YKINE_ZE :
      DEBUG_YKINE   yLOG_note    ("zero linear partial");
      ykine_exact_pct_xzy     (a_pct);
      break;
   case YKINE_PO :
      DEBUG_YKINE   yLOG_note    ("zero polar partial");
      ykine_exact_pct_doy  (a_verb, a_pct);
      break;
   case YKINE_FK :
      DEBUG_YKINE   yLOG_note    ("forward partial");
      ykine_exact_pct_xzy     (a_pct);
      break;
   case YKINE_IK : case YKINE_TK : case YKINE_NK :
      DEBUG_YKINE   yLOG_note    ("inverse linear partial");
      ykine_exact_pct_xzy  (a_pct);
      break;
   case YKINE_CK : case YKINE_RK : case YKINE_SK :
      DEBUG_YKINE   yLOG_note    ("inverse polar partial");
      ykine_exact_pct_doy  (a_verb, a_pct);
      break;
   case YKINE_SYNC : case YKINE_LEGE : case YKINE_BODE : case YKINE_NOOP :
      DEBUG_YKINE   yLOG_note    ("wait, errors, and noop partial");
      ykine_exact_pct_xzy  (a_pct);
      break;
   default       :
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     servo positioning                        ----===*/
/*====================------------------------------------====================*/
static void      o___SERVOS__________________o (void) {;}

char         /*--> identify the current move -------------[ ------ [ ------ ]-*/
ykine__exact_check      (tMOVE *a_curr, float a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_beg       =    0;
   int         x_cur       =    0;
   int         x_end       =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   if (a_curr == NULL) {
      DEBUG_YKINE  yLOG_snote   ("null move");
      DEBUG_YKINE  yLOG_sexitr  (__FUNCTION__, -2);
      return -2;
   }
   /*---(prepare)------------------------*/
   x_cur = round (a_sec * 1000.0);
   if (a_curr->s_prev != NULL)   x_beg = round (a_curr->s_prev->secs * 1000.0);
   else                          x_beg = 0.0;
   x_end = round (a_curr->secs * 1000.0);
   DEBUG_YKINE  yLOG_sint    (a_curr->seq);
   /*---(start time)---------------------*/
   if (x_cur == x_end && x_end == 0.00 && a_curr->s_prev == NULL) {
      DEBUG_YKINE  yLOG_snote   ("init/start");
      DEBUG_YKINE  yLOG_sexit   (__FUNCTION__);
      return  2;
   }
   /*---(check not yet)------------------*/
   if (x_cur >  x_end) {
      DEBUG_YKINE  yLOG_snote   ("not yet");
      DEBUG_YKINE  yLOG_sexit   (__FUNCTION__);
      return  1;
   }
   /*---(check missed)-------------------*/
   if (x_cur <= x_beg) {
      DEBUG_YKINE  yLOG_snote   ("too far");
      DEBUG_YKINE  yLOG_sexit   (__FUNCTION__);
      return -1;
   }
   /*---(exactly)------------------------*/
   if (x_cur == x_end) {
      DEBUG_YKINE  yLOG_snote   ("exactly");
      DEBUG_YKINE  yLOG_sexit   (__FUNCTION__);
      return  2;
   }
   /*---(found it)-----------------------*/
   DEBUG_YKINE  yLOG_snote   ("in range");
   DEBUG_YKINE  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> identify the current move -------------[ ------ [ ------ ]-*/
ykine__exact_find       (tSERVO *a_servo, float a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMOVE      *x_next      = NULL;
   int         x_seq       =   -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (a_servo == NULL) {
      DEBUG_YKINE  yLOG_note    ("null servo");
      DEBUG_YKINE  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   DEBUG_YKINE  yLOG_complex ("servo"     , "%s, %2dl, %2ds, %3dc, %ph, %pn", a_servo->label, a_servo->leg, a_servo->seg, a_servo->count, a_servo->head, a_servo->curr);
   a_servo->exact = '-';
   a_servo->deg   = 0.0;
   a_servo->xexp  = 0.0;
   a_servo->zexp  = 0.0;
   a_servo->yexp  = 0.0;
   if (a_servo->curr == NULL)  x_next = a_servo->head;
   else                        x_next = a_servo->curr;
   /*---(walk thru moves)----------------*/
   DEBUG_YKINE  yLOG_point   ("x_next"    , x_next);
   while (x_next != NULL) {
      DEBUG_YKINE  yLOG_value   ("seq"       , x_next->seq);
      rc = ykine__exact_check (x_next, a_sec);
      if (rc == 0 || rc == 2) {
         DEBUG_YKINE  yLOG_note    ("success, save values");
         a_servo->curr  = x_next;
         if (rc == 2)  a_servo->exact = 'y';
         else          a_servo->exact = 'n';
         DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
         return 0;
      }
      if      (rc ==  1)   x_next = x_next->s_next;
      else if (rc == -1)   x_next = x_next->s_prev;
   }
   /*---(complete)-----------------------*/
   --rce;
   a_servo->curr = NULL;
   DEBUG_YKINE  yLOG_note    ("failed, current move never found");
   DEBUG_YKINE  yLOG_exitr   (__FUNCTION__, --rce);
   return rce;
}

char
ykine_move_getservos    (char a_leg, char a_seg, tMOVE **a_curr, tMOVE **a_prev, char *a_exact)
{
   /*---(locals)-----------+-----+-----+-*/
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   if (a_curr != NULL)  *a_curr = NULL;
   if (a_prev != NULL)  *a_prev = NULL;
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE  yLOG_point   ("x_servo"   , x_servo);
   DEBUG_YKINE  yLOG_info    ("->label"   , x_servo->label);
   if (x_servo != NULL) {
      *a_curr = x_servo->curr;
      if (*a_curr != NULL)  *a_prev = (*a_curr)->s_prev;
   }
   DEBUG_YKINE  yLOG_point   ("*a_curr"   , *a_curr);
   DEBUG_YKINE  yLOG_point   ("*a_prev"   , *a_prev);
   if (a_exact != NULL) {
      if (x_servo->exact == 'y')  *a_exact = 'y';
      else                        *a_exact = '-';
   }
   /*---complete)------------------------*/
   DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_move_copyout      (tMOVE *a_move, float *d, float *s, float *x, float *z, float *y, float *xz)
{
   /*---(defaults)-----------------------*/
   if (a_move == NULL) {
      if (d  != NULL)  *d  = 0.0;
      if (s  != NULL)  *s  = 0.0;
      if (x  != NULL)  *x  = 0.0;
      if (z  != NULL)  *z  = 0.0;
      if (y  != NULL)  *y  = 0.0;
      if (xz != NULL)  *xz = 0.0;
      return 0;
   }
   /*---(good move)----------------------*/
   if (d  != NULL)  *d  = a_move->pure_d;
   if (s  != NULL)  *s  = a_move->secs;
   if (x  != NULL)  *x  = a_move->pure_x;
   if (z  != NULL)  *z  = a_move->pure_z;
   if (y  != NULL)  *y  = a_move->pure_y;
   if (xz != NULL)  *xz = a_move->pure_xz;
   /*---(leg correction)-----------------*/
   /*> if (y != NULL && a_move->servo->leg != YKINE_BODY)  *y -= yKINE_seglen (YKINE_FOOT);   <*/
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_clearwork   (tWORK *a)
{
   a->sec  = 0.0;
   a->rc   = 0;
   a->cd   = 0.0;
   a->fd   = a->fx   = a->fz   = a->fy   = 0.0;
   a->pd   = a->px   = a->pz   = a->py   = 0.0;
   a->td   = a->tx   = a->tz   = a->ty   = 0.0;
   a->ed   = a->ex   = a->ez   = a->ey   = 0.0;
   a->deg  = a->out  = a->len  = a->xz   = 0.0;
   return 0;
}

char
ykine_exact_clearall    (void)
{
   /*---(globals)------------------------*/
   myKINE.exact = myKINE.part  = myKINE.cell  = '-';
   myKINE.label [0] = '\0';
   /*---(work containers)----------------*/
   ykine_exact_clearwork (&g_beg);
   ykine_exact_clearwork (&g_cur);
   ykine_exact_clearwork (&g_end);
   ykine_exact_clearwork (&g_sav);
   /*---(full containers)----------------*/
   ykine_exact_clearwork (&g_pure);
   ykine_exact_clearwork (&g_adapt);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_copy2pure   (float a_sec, char a_leg, char a_meth, char a_rc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(pull values)--------------------*/
   g_pure.sec  = a_sec;
   rc = yKINE_endpoint  (a_leg, YKINE_THOR, a_meth, &(g_pure.cd) , NULL, NULL         , NULL         , NULL         , NULL);
   rc = yKINE_endpoint  (a_leg, YKINE_FEMU, a_meth, &(g_pure.fd) , NULL, &(g_pure.fx) , &(g_pure.fz) , &(g_pure.fy) , NULL);
   rc = yKINE_endpoint  (a_leg, YKINE_PATE, a_meth, &(g_pure.pd) , NULL, &(g_pure.px) , &(g_pure.pz) , &(g_pure.py) , NULL);
   rc = yKINE_endpoint  (a_leg, YKINE_TIBI, a_meth, &(g_pure.td) , NULL, &(g_pure.tx) , &(g_pure.tz) , &(g_pure.ty) , NULL);
   if (a_rc >= 0) {
      rc = yKINE_endpoint  (a_leg, YKINE_FOOT, a_meth, &(g_pure.ed) , NULL, &(g_pure.ex) , &(g_pure.ez) , &(g_pure.ey) , NULL);
   } else {
      rc = yKINE_endpoint  (a_leg, YKINE_TARG, a_meth, NULL         , NULL, &(g_pure.ex) , &(g_pure.ez) , &(g_pure.ey) , NULL);
   }
   g_pure.rc  = a_rc;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_copy2adapt  (float a_sec, char a_leg, char a_meth, char a_rc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(pull values)--------------------*/
   g_adapt.sec = a_sec;
   rc = yKINE_endpoint  (a_leg, YKINE_THOR, a_meth, &(g_adapt.cd), NULL, NULL         , NULL         , NULL         , NULL);
   rc = yKINE_endpoint  (a_leg, YKINE_FEMU, a_meth, &(g_adapt.fd), NULL, &(g_adapt.fx), &(g_adapt.fz), &(g_adapt.fy), NULL);
   rc = yKINE_endpoint  (a_leg, YKINE_PATE, a_meth, &(g_adapt.pd), NULL, &(g_adapt.px), &(g_adapt.pz), &(g_adapt.py), NULL);
   rc = yKINE_endpoint  (a_leg, YKINE_TIBI, a_meth, &(g_adapt.td), NULL, &(g_adapt.tx), &(g_adapt.tz), &(g_adapt.ty), NULL);
   if (a_rc >= 0) {
      rc = yKINE_endpoint  (a_leg, YKINE_FOOT, a_meth, &(g_adapt.ed), NULL, &(g_adapt.ex), &(g_adapt.ez), &(g_adapt.ey), NULL);
   } else {
      rc = yKINE_endpoint  (a_leg, YKINE_TARG, a_meth, NULL         , NULL, &(g_adapt.ex), &(g_adapt.ez), &(g_adapt.ey), NULL);
   }
   g_adapt.rc  = a_rc;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_fail2pure   (float a_sec, char a_leg, char a_meth, char a_rc, float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(pull values)--------------------*/
   ykine_exact_clearwork (&g_pure);
   g_pure.sec = a_sec;
   g_pure.ex  = a_x;
   g_pure.ez  = a_z;
   g_pure.ey  = a_y;
   g_pure.rc  = a_rc;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_fail2adapt  (float a_sec, char a_leg, char a_meth, char a_rc, float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(pull values)--------------------*/
   ykine_exact_clearwork (&g_adapt);
   g_adapt.sec = a_sec;
   g_adapt.ex  = a_x;
   g_adapt.ez  = a_z;
   g_adapt.ey  = a_y;
   g_adapt.rc  = a_rc;
   /*---(complete)-----------------------*/
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_exact_context     (char a_leg, float a_margin)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_prev      = NULL;
   /*---(wipe)---------------------------*/
   ykine_exact_clearall ();
   /*---(zero point)---------------------*/
   if (a_leg == YKINE_CENTER) {
      ykine_move_getservos (YKINE_BODY, YKINE_FOCU , &x_curr, &x_prev, &myKINE.exact);
      ykine_move_copyout   (x_prev, NULL      , &g_beg.sec, &g_beg.ex , &g_beg.ez , &g_beg.ey , &g_beg.xz  );
      ykine_move_copyout   (x_curr, NULL      , &g_end.sec, &g_end.ex , &g_end.ez , &g_end.ey , &g_end.xz  );
   }
   /*---(body and legs)------------------*/
   else {
      /*---(femur/yaw)----------------------*/
      ykine_move_getservos (a_leg , YKINE_FEMU , &x_curr, &x_prev, NULL);
      ykine_move_copyout   (x_prev, &g_beg.fd , NULL      , NULL      , NULL      , NULL      , NULL       );
      ykine_move_copyout   (x_curr, &g_end.fd , NULL      , NULL      , NULL      , NULL      , NULL       );
      if (x_curr != NULL)  ++rc;
      /*---(patella/pitch)------------------*/
      ykine_move_getservos (a_leg , YKINE_PATE , &x_curr, &x_prev, NULL);
      ykine_move_copyout   (x_prev, &g_beg.pd , NULL      , NULL      , NULL      , NULL      , NULL       );
      ykine_move_copyout   (x_curr, &g_end.pd , NULL      , NULL      , NULL      , NULL      , NULL       );
      if (x_curr != NULL)  ++rc;
      /*---(tibia/roll)---------------------*/
      ykine_move_getservos (a_leg , YKINE_TIBI , &x_curr, &x_prev, &myKINE.exact);
      if (a_leg == YKINE_BODY) {
         ykine_move_copyout   (x_prev, &g_beg.td , &g_beg.sec, NULL      , NULL      , NULL      , NULL       );
         ykine_move_copyout   (x_curr, &g_end.td , &g_end.sec, NULL      , NULL      , NULL      , NULL       );
      } else {
         ykine_move_copyout   (x_prev, &g_beg.td , &g_beg.sec, &g_beg.tx , &g_beg.tz , &g_beg.ty , &g_beg.xz  );
         ykine_move_copyout   (x_curr, &g_end.td , &g_end.sec, &g_end.tx , &g_end.tz , &g_end.ty , &g_end.xz  );
         ykine_move_getservos (a_leg , YKINE_FOOT , &x_curr, &x_prev, &myKINE.exact);
         ykine_move_copyout   (x_prev, &g_beg.ed , NULL      , &g_beg.ex , &g_beg.ez , &g_beg.ey , NULL       );
         ykine_move_copyout   (x_curr, &g_end.ed , NULL      , &g_end.ex , &g_end.ez , &g_end.ey , NULL       );
      }
      if (x_curr != NULL)  ++rc;
      /*---(coxa)---------------------------*/
      if (a_leg != YKINE_BODY && x_curr != NULL) {
         g_cur.cd   = yKINE_legdeg (a_leg);
         g_end.cd   = g_cur.cd ;
      } else {
         g_cur.cd   = 0.0;
         g_end.cd   = 0.0;
      }
   }
   /*---(verb)---------------------------*/
   if (x_curr != NULL)  myKINE.vb  = x_curr->verb;
   else                 myKINE.vb  = -1;
   /*---(distance)-----------------------*/
   if (myKINE.vb >= 0)  ykine_exact_dist_route (myKINE.vb);
   /*---(exact)--------------------------*/
   DEBUG_YKINE  yLOG_complex ("beg"       , "%6.2fsc, %6.2fsb, %6.2fdi, %6.2fma, %c", s_sec, g_beg.sec, s_sec - g_beg.sec, a_margin, (s_sec - g_beg.sec <= a_margin + 0.01) ? 'y' : '-');
   myKINE.exact = '-';
   if (x_curr != NULL && s_sec - g_beg.sec <= a_margin + 0.01) {
      DEBUG_YKINE  yLOG_note    ("mark exact");
      myKINE.exact = 'y';
   }
   /*---(labels)-------------------------*/
   if (x_curr != NULL) {
      DEBUG_YKINE  yLOG_note    ("copy labels and marks");
      strlcpy (myKINE.label, x_curr->label, LEN_LABEL);
      myKINE.part = x_curr->part;
      myKINE.cell = x_curr->cell;
   }
   /*---(report)-------------------------*/
   DEBUG_YKINE  yLOG_value   ("verb"      , myKINE.vb);
   DEBUG_YKINE  yLOG_complex ("b.angles"  , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , g_cur.cd , g_beg.fd , g_beg.pd , g_beg.td );
   DEBUG_YKINE  yLOG_complex ("b.coords"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", g_beg.ex , g_beg.ez , g_beg.ey , g_beg.xz  );
   DEBUG_YKINE  yLOG_complex ("e.angles"  , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , g_end.cd , g_end.fd , g_end.pd , g_end.td );
   DEBUG_YKINE  yLOG_complex ("e.coords"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", g_end.ex , g_end.ez , g_end.ey , g_end.xz  );
   /*---(complete)-----------------------*/
   return rc;
}

char         /*--> set all the servos to the right time --[ ------ [ ------ ]-*/
yKINE_exact_all         (float a_sec)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         i           =    0;
   tSERVO     *x_servo     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE  yLOG_double  ("a_sec"     , a_sec);
   /*---(save seconds)-------------------*/
   s_sec = a_sec;
   /*---(set current move)---------------*/
   for (i = 0; i < g_nservo; ++i) {
      x_servo   = &(g_servo_info [i]);
      rc = ykine__exact_find  (x_servo, s_sec);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> set all the servos to the right time --[ ------ [ ------ ]-*/
yKINE_exact_leg         (char a_leg, float a_margin, char *a_exact, char *a_label, char *a_cell, char *pure, float *f, float *p, float *t, float *x, float *z, float *y, char *adapt, float *fr, float *pr, float *tr, float *xr, float *zr, float *yr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   float       x_pct       =  0.0;
   float       x_range     =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE  yLOG_value   ("a_leg"     , a_leg);
   /*---(initialize)---------------------*/
   if (a_exact != NULL)  *a_exact = '-';
   if (a_label != NULL)   a_label [0] = '\0';
   if (a_cell  != NULL)  *a_cell  = '-';
   if (pure    != NULL)  *pure    =   0;
   if (x       != NULL)  *x       = 0.0;
   if (z       != NULL)  *z       = 0.0;
   if (y       != NULL)  *y       = 0.0;
   if (f       != NULL)  *f       = 0.0;
   if (p       != NULL)  *p       = 0.0;
   if (t       != NULL)  *t       = 0.0;
   if (adapt   != NULL)  *adapt   =   0;
   if (xr      != NULL)  *xr      = 0.0;
   if (zr      != NULL)  *zr      = 0.0;
   if (yr      != NULL)  *yr      = 0.0;
   if (fr      != NULL)  *fr      = 0.0;
   if (pr      != NULL)  *pr      = 0.0;
   if (tr      != NULL)  *tr      = 0.0;
   /*---(prepare)------------------------*/
   myKINE.rcc = 0;
   /*---(beg and end points)-------------*/
   ykine_exact_context (a_leg, a_margin);
   DEBUG_YKINE  yLOG_value   ("vb"        , myKINE.vb);
   --rce;  if (myKINE.vb < 0) {
      DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
      myKINE.rcc = rce;
      return rce;
   }
   if (myKINE.exact == 'y' && a_exact != NULL)  *a_exact = 'y';
   if (a_label != NULL)  strlcpy (a_label, myKINE.label, LEN_LABEL);
   if (a_cell  != NULL)  *a_cell  = myKINE.cell;
   /*---(calc percent)-------------------*/
   x_range = g_end.sec - g_beg.sec;
   if (x_range == 0.0)  x_pct   = 1.00;
   else                 x_pct   = (s_sec - g_beg.sec) / x_range;
   DEBUG_YKINE  yLOG_complex ("percent"   , "sb %6.1fs, se %6.1fs, ra %6.1fs, pc %6.1fs", g_beg.sec, g_end.sec, x_range, x_pct);
   /*---(calc current endpoint)----------*/
   rc = ykine_exact_pct_route (myKINE.vb, x_pct);
   DEBUG_YKINE  yLOG_value   ("exact"     , rc);
   if (myKINE.part == 'm')  ykine_step_yshaper (myKINE.lxz, x_pct, &g_cur.ey );
   /*---(save current endpoint)----------*/
   if (x     != NULL)  *x     = g_cur.ex ;
   if (z     != NULL)  *z     = g_cur.ez ;
   if (y     != NULL)  *y     = g_cur.ey ;
   DEBUG_YKINE  yLOG_complex ("orig pos"  , "%6.1fx, %6.1fz, %6.1fy", g_cur.ex , g_cur.ez , g_cur.ey );
   /*---(set zero-point)-----------------*/
   if (a_leg == YKINE_CENTER) {
      yKINE_zero   (g_cur.ex , g_cur.ez , g_cur.ey );
      if (xr    != NULL)  *xr    = g_cur.ex ;
      if (zr    != NULL)  *zr    = g_cur.ez ;
      if (yr    != NULL)  *yr    = g_cur.ey ;
      DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
      return myKINE.rcc;
   }
   /*---(set orientation)----------------*/
   if (a_leg == YKINE_BODY)  {
      yKINE_orient (g_cur.fd , g_cur.pd , g_cur.td );
      if (fr    != NULL)  *fr    = g_cur.fd ;
      if (pr    != NULL)  *pr    = g_cur.pd ;
      if (tr    != NULL)  *tr    = g_cur.td ;
      DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
      return myKINE.rcc;
   }
   /*---(inverse kinematics)-------------*/
   rc = ykine_exec_partial  (myKINE.vb, a_leg, 'e');
   /*> ykine_step_copy (&g_cur, &g_adapt);                                            <*/
   DEBUG_YKINE  yLOG_value   ("partial"   , rc);
   --rce;  if (myKINE.rcc < 0)  {
      DEBUG_YKINE  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(return codes)-------------------*/
   if (pure  != NULL)  *pure  = g_pure.rc ;
   if (adapt != NULL)  *adapt = g_adapt.rc;
   /*---(save current angles)------------*/
   g_cur.cd  = g_pure.cd ;
   if (f     != NULL)  *f     = g_pure.fd ;
   if (p     != NULL)  *p     = g_pure.pd ;
   if (t     != NULL)  *t     = g_pure.td ;
   DEBUG_YKINE  yLOG_complex ("orig ang"  , "%6.1ff, %6.1fp, %6.1ft", g_pure.fd , g_pure.pd , g_pure.td );
   /*---(save revised endpoint)----------*/
   if (xr    != NULL)  *xr    = g_adapt.ex ;
   if (zr    != NULL)  *zr    = g_adapt.ez ;
   if (yr    != NULL)  *yr    = g_adapt.ey ;
   DEBUG_YKINE  yLOG_complex ("rev pos"   , "%6.1fx, %6.1fz, %6.1fy", g_adapt.ex , g_adapt.ez , g_adapt.ey );
   /*---(save revised angles)------------*/
   if (fr    != NULL)  *fr    = g_adapt.fd ;
   if (pr    != NULL)  *pr    = g_adapt.pd ;
   if (tr    != NULL)  *tr    = g_adapt.td ;
   DEBUG_YKINE  yLOG_complex ("rev ang"   , "%6.1ff, %6.1fp, %6.1ft", g_adapt.fd , g_adapt.pd , g_adapt.td );
   /*---(complete)-----------------------*/
   DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
   return myKINE.rcc;
}

char         /*--> set all the servos to the right time --[ ------ [ ------ ]-*/
ykine_exact_setbody     (float a_sec)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         i           =    0;
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_prev      = NULL;
   float       sb = 0.0, se = 0.0, sc = 0.0;
   float       xb = 0.0, xe = 0.0, xc = 0.0;
   float       zb = 0.0, ze = 0.0, zc = 0.0;
   float       yb = 0.0, ye = 0.0, yc = 0.0;
   float       fb = 0.0, fe = 0.0, fc = 0.0;
   float       pb = 0.0, pe = 0.0, pc = 0.0;
   float       tb = 0.0, te = 0.0, tc = 0.0;
   float       x_pct       =  0.0;
   float       x_range     =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE  yLOG_double  ("a_sec"     , a_sec);
   /*---(set current move)---------------*/
   x_servo   = ykine_servo_pointer     (YKINE_BODY, YKINE_FOCU);
   rc = ykine__exact_find  (x_servo, a_sec);
   /*---(set zero-point)-----------------*/
   ykine_move_getservos (YKINE_BODY, YKINE_FOCU, &x_curr, &x_prev, NULL);
   ykine_move_copyout   (x_prev, NULL      , &sb, &xb, &zb, &yb, NULL);
   ykine_move_copyout   (x_curr, NULL      , &se, &xe, &ze, &ye, NULL);
   DEBUG_YKINE   yLOG_complex ("pos-beg"   , "%8.2fsb, %8.2fxb, %8.2fzb, %8.2fyb", sb, xb, zb, yb);
   DEBUG_YKINE   yLOG_complex ("pos-end"   , "%8.2fse, %8.2fxe, %8.2fze, %8.2fye", se, xe, ze, ye);
   /*---(calc percent)-------------------*/
   x_range = se - sb;
   if (x_range == 0.0)  x_pct   = 1.00;
   else                 x_pct   = (a_sec - sb) / x_range;
   DEBUG_YKINE  yLOG_complex ("percent"   , "sb %6.1fs, se %6.1fs, ra %6.1fs, pc %6.1fs", sb, se, x_range, x_pct);
   ykine_exact_calc ('-', sb, se, x_pct, &sc);
   ykine_exact_calc ('-', xb, xe, x_pct, &xc);
   ykine_exact_calc ('-', zb, ze, x_pct, &zc);
   ykine_exact_calc ('-', yb, ye, x_pct, &yc);
   DEBUG_YKINE   yLOG_complex ("pos-cur"   , "%8.2fsc, %8.2fxc, %8.2fzc, %8.2fyc", sc, xc, zc, yc);
   rc = yKINE_zero   (xc, zc, yc);
   /*---(femur/yaw)----------------------*/
   ykine_move_getservos (YKINE_BODY, YKINE_FEMU, &x_curr, &x_prev, NULL);
   ykine_move_copyout   (x_prev, &fb, NULL, NULL, NULL, NULL, NULL);
   ykine_move_copyout   (x_curr, &fe, NULL, NULL, NULL, NULL, NULL);
   /*---(patella/pitch)------------------*/
   ykine_move_getservos (YKINE_BODY, YKINE_PATE, &x_curr, &x_prev, NULL);
   ykine_move_copyout   (x_prev, &pb, NULL, NULL, NULL, NULL, NULL);
   ykine_move_copyout   (x_curr, &pe, NULL, NULL, NULL, NULL, NULL);
   /*---(tibia/roll)---------------------*/
   ykine_move_getservos (YKINE_BODY, YKINE_TIBI, &x_curr, &x_prev, NULL);
   ykine_move_copyout   (x_prev, &tb, &sb, NULL, NULL, NULL, NULL);
   ykine_move_copyout   (x_curr, &te, &se, NULL, NULL, NULL, NULL);
   DEBUG_YKINE   yLOG_complex ("ang-beg"   , "%8.2fsb, %8.2ffb, %8.2fpb, %8.2ftb", sb, fb, pb, tb);
   DEBUG_YKINE   yLOG_complex ("ang-end"   , "%8.2fse, %8.2ffe, %8.2fpe, %8.2fte", se, fe, pe, te);
   /*---(orientation)--------------------*/
   x_range = se - sb;
   if (x_range == 0.0)  x_pct   = 1.00;
   else                 x_pct   = (a_sec - sb) / x_range;
   DEBUG_YKINE  yLOG_complex ("percent"   , "sb %6.1fs, se %6.1fs, ra %6.1fs, pc %6.1fs", sb, se, x_range, x_pct);
   ykine_exact_calc ('-', sb, se, x_pct, &sc);
   ykine_exact_calc ('-', fb, fe, x_pct, &fc);
   ykine_exact_calc ('-', pb, pe, x_pct, &pc);
   ykine_exact_calc ('-', tb, te, x_pct, &tc);
   DEBUG_YKINE   yLOG_complex ("ang-cur"   , "%8.2fsc, %8.2ffc, %8.2fpc, %8.2ftc", sc, fc, pc, tc);
   rc = yKINE_orient (fc, pc, tc);
   /*---(complete)-----------------------*/
   DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     unit testing support                     ----===*/
/*====================------------------------------------====================*/
static void      o___UNIT_TEST_______________o (void) {;}

char
ykine_exact_fake_beg    (float db, float sb, float xb, float zb, float yb, float ob)
{
   g_beg.deg = db;
   g_beg.sec = sb;
   g_beg.ex  = xb;
   g_beg.ez  = zb;
   g_beg.ey  = yb;
   g_beg.out = ob;
   return 0;
}

char
ykine_exact_fake_end    (float de, float se, float xe, float ze, float ye, float oe)
{
   g_end.deg = de;
   g_end.sec = se;
   g_end.ex  = xe;
   g_end.ez  = ze;
   g_end.ey  = ye;
   g_end.out = oe;
   return 0;
}

char
ykine_exact_fake_abeg   (float sb, float cb, float fb, float pb, float tb)
{
   g_beg.sec = sb;
   g_cur.cd  = cb;
   g_beg.fd  = fb;
   g_beg.pd  = pb;
   g_beg.td  = tb;
   return 0;
}

char
ykine_exact_fake_aend   (float se, float ce, float fe, float pe, float te)
{
   g_end.sec = se;
   g_end.cd  = ce;
   g_end.fd  = fe;
   g_end.pd  = pe;
   g_end.td  = te;
   return 0;
}

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__exact_unit       (char *a_question, int a_cnt)
{
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "EXACT unit     : question not understood", LEN_RECD);
   /*---(answer)-------------------------*/
   if (strcmp (a_question, "begin"   ) == 0) {
      sprintf (ykine__unit_answer, "EXACT begin      : %6.1lfd %6.3lfs %6.1lfx %6.1lfz %6.1lfy %6.1lfo %6.1lfc %6.1lff %6.1lfp %6.1lft", g_beg.deg, g_beg.sec, g_beg.ex , g_beg.ez , g_beg.ey , g_beg.out, g_cur.cd , g_beg.fd , g_beg.pd , g_beg.td );
   }
   else if (strcmp (a_question, "end"     ) == 0) {
      sprintf (ykine__unit_answer, "EXACT end        : %6.1lfd %6.3lfs %6.1lfx %6.1lfz %6.1lfy %6.1lfo %6.1lfc %6.1lff %6.1lfp %6.1lft", g_end.deg, g_end.sec, g_end.ex , g_end.ez , g_end.ey , g_end.out, g_end.cd , g_end.fd , g_end.pd , g_end.td );
   }
   else if (strcmp (a_question, "current" ) == 0) {
      sprintf (ykine__unit_answer, "EXACT curr (%3d) : %6.1lfd %6.3lfs %6.1lfx %6.1lfz %6.1lfy %6.1lfo %6.1lfc %6.1lff %6.1lfp %6.1lft", myKINE.rcc, g_cur.deg, g_cur.sec, g_cur.ex , g_cur.ez , g_cur.ey , g_cur.out, g_cur.cd , g_cur.fd , g_cur.pd , g_cur.td );
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}


