/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



static float s_sec    = 0.0;



/*====================------------------------------------====================*/
/*===----                      shared calculations                     ----===*/
/*====================------------------------------------====================*/
static void      o___CALCS___________________o (void) {;}

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

float        /*--> pure coordinate move distance ---------[ ------ [ ------ ]-*/
ykine_exact_dist_xzy    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   float       xd, zd, yd, ld;
   /*---(full beg/end)-------------------*/
   xd  = myKINE.xe - myKINE.xb;
   zd  = myKINE.ze - myKINE.zb;
   yd  = myKINE.ye - myKINE.yb;
   /*---(length)-------------------------*/
   ld = sqrt ((xd * xd) + (zd * zd) + (yd * yd));
   myKINE.le = ld;
   /*---(complete)-----------------------*/
   return ld;
}

float        /*--> leg polar move distance ---------------[ ------ [ ------ ]-*/
ykine_exact_dist_doy    (void)
{  /* ´ arc distance = 2÷r is full circum and * (deg/360) cuts our part
    * ´ calc the arc length, then treat it as a triangle side for distance
    * ´ less than 90 deg, the difference is a max of 5% greater length
    */
   /*---(locals)-----------+-----+-----+-*/
   float       dd, od, yd, ld;
   /*---(full beg/end)-------------------*/
   dd  = ((myKINE.de - myKINE.db) / 360.0) * 2.0 * 3.1415927 * ((myKINE.ob + myKINE.oe) / 2.0);
   od  = myKINE.oe - myKINE.ob;
   yd  = myKINE.ye - myKINE.yb;
   /*---(length)-------------------------*/
   ld  = sqrt ((dd * dd) + (od * od) + (yd * yd));
   myKINE.le = ld;
   /*---(complete)-----------------------*/
   return ld;
}

/*> float        /+--> zero polar move distance --------------[ ------ [ ------ ]-+/                  <* 
 *> ykine_exact_dist_dty    (void)                                                                    <* 
 *> {                                                                                                 <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                       <* 
 *>    float       dd, td, yd, ld;                                                                    <* 
 *>    /+---(full beg/end)-------------------+/                                                       <* 
 *>    dd  = ((myKINE.de - myKINE.db) / 360.0) * 2.0 * 3.1415927 * ((myKINE.ob + myKINE.oe) / 2.0);   <* 
 *>    od  = myKINE.oe - myKINE.ob;                                                                   <* 
 *>    yd  = myKINE.ye - myKINE.yb;                                                                   <* 
 *>    /+---(length)-------------------------+/                                                       <* 
 *>    ld  = sqrt ((dd * dd) + (od * od) + (yd * yd));                                                <* 
 *>    myKINE.le = ld;                                                                                <* 
 *>    /+---(complete)-----------------------+/                                                       <* 
 *>    return ld;                                                                                     <* 
 *> }                                                                                                 <*/

char
ykine_exact_pct_xzy     (float p)
{
   /*---(calculate)----------------------*/
   myKINE.pct = p;
   ykine_exact_calc ('-', myKINE.sb, myKINE.se, p, &myKINE.sc);
   ykine_exact_calc ('-', myKINE.xb, myKINE.xe, p, &myKINE.xc);
   ykine_exact_calc ('-', myKINE.zb, myKINE.ze, p, &myKINE.zc);
   ykine_exact_calc ('-', myKINE.yb, myKINE.ye, p, &myKINE.yc);
   ykine_exact_calc ('-', myKINE.fb, myKINE.fe, p, &myKINE.fc);
   ykine_exact_calc ('-', myKINE.pb, myKINE.pe, p, &myKINE.pc);
   ykine_exact_calc ('-', myKINE.tb, myKINE.te, p, &myKINE.tc);
   DEBUG_YKINE_EXACT   yLOG_complex ("linear"    , "%8.2fx, %8.2fz, %8.2fy", myKINE.xc, myKINE.zc, myKINE.yc);
   DEBUG_YKINE_EXACT   yLOG_complex ("angles"    , "%8.2ff, %8.2fp, %8.2ft", myKINE.fc, myKINE.pc, myKINE.tc);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_exact_pct_doy     (char a_verb, int a_leg, float p)
{
   myKINE.pct = p;
   ykine_exact_calc ('-', myKINE.sb, myKINE.se, p, &myKINE.sc);
   switch (a_verb) {
   case YKINE_CK :
      DEBUG_YKINE_EXACT   yLOG_note    ("center kinematics");
      ykine_legs_ik2ck (myKINE.cb, myKINE.xb, myKINE.zb, &myKINE.db, &myKINE.ob);
      ykine_legs_ik2ck (myKINE.cb, myKINE.xe, myKINE.ze, &myKINE.de, &myKINE.oe);
      break;
   case YKINE_RK :
      DEBUG_YKINE_EXACT   yLOG_note    ("radial kinematics");
      ykine_legs_ik2rk (myKINE.cb, myKINE.xb, myKINE.zb, &myKINE.db, &myKINE.ob);
      ykine_legs_ik2rk (myKINE.cb, myKINE.xe, myKINE.ze, &myKINE.de, &myKINE.oe);
      break;
   case YKINE_SK :
      DEBUG_YKINE_EXACT   yLOG_note    ("step kinematics");
      ykine_legs_ik2sk (myKINE.cb, myKINE.xb, myKINE.zb, &myKINE.db, &myKINE.ob);
      ykine_legs_ik2sk (myKINE.cb, myKINE.xe, myKINE.ze, &myKINE.de, &myKINE.oe);
      break;
   }
   DEBUG_YKINE_EXACT   yLOG_complex ("begin"     , "%8.2fx, %8.2fz, %8.2fd, %8.2fo", myKINE.xb, myKINE.xb, myKINE.db, myKINE.ob);
   DEBUG_YKINE_EXACT   yLOG_complex ("end"       , "%8.2fx, %8.2fz, %8.2fd, %8.2fo", myKINE.xe, myKINE.xe, myKINE.de, myKINE.oe);
   ykine_exact_calc ('-', myKINE.db, myKINE.de, p, &myKINE.dc);
   ykine_exact_calc ('-', myKINE.ob, myKINE.oe, p, &myKINE.oc);
   ykine_exact_calc ('-', myKINE.yb, myKINE.ye, p, &myKINE.yc);
   DEBUG_YKINE_EXACT   yLOG_complex ("polar"     , "%8.2fd, %8.2fo, %8.2fy", myKINE.dc, myKINE.oc, myKINE.yc);
   switch (a_verb) {
   case YKINE_CK :  ykine_legs_ck2ik (myKINE.cb, myKINE.dc, myKINE.oc, &myKINE.xc, &myKINE.zc);  break;
   case YKINE_RK :  ykine_legs_rk2ik (myKINE.cb, myKINE.dc, myKINE.oc, &myKINE.xc, &myKINE.zc);  break;
   case YKINE_SK :  ykine_legs_sk2ik (myKINE.cb, myKINE.dc, myKINE.oc, &myKINE.xc, &myKINE.zc);  break;
   }
   DEBUG_YKINE_EXACT   yLOG_complex ("endpoint"  , "%8.2fx, %8.2fz, %8.2fy", myKINE.xc, myKINE.zc, myKINE.yc);
   ykine_exact_calc ('-', myKINE.fb, myKINE.fe, p, &myKINE.fc);
   ykine_exact_calc ('-', myKINE.pb, myKINE.pe, p, &myKINE.pc);
   ykine_exact_calc ('-', myKINE.tb, myKINE.te, p, &myKINE.tc);
   DEBUG_YKINE_EXACT   yLOG_complex ("angles"    , "%8.2ff, %8.2fp, %8.2ft", myKINE.fc, myKINE.pc, myKINE.tc);
   /*---(complete)-----------------------*/
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
   DEBUG_YKINE_EXACT  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   if (a_curr == NULL) {
      DEBUG_YKINE_EXACT  yLOG_snote   ("null move");
      DEBUG_YKINE_EXACT  yLOG_sexitr  (__FUNCTION__, -2);
      return -2;
   }
   /*---(prepare)------------------------*/
   x_cur = round (a_sec * 1000.0);
   if (a_curr->s_prev != NULL)   x_beg = round (a_curr->s_prev->secs * 1000.0);
   else                          x_beg = 0.0;
   x_end = round (a_curr->secs * 1000.0);
   DEBUG_YKINE_EXACT  yLOG_sint    (a_curr->seq);
   /*---(start time)---------------------*/
   if (x_cur == x_end && x_end == 0.00 && a_curr->s_prev == NULL) {
      DEBUG_YKINE_EXACT  yLOG_snote   ("init/start");
      DEBUG_YKINE_EXACT  yLOG_sexit   (__FUNCTION__);
      return  2;
   }
   /*---(check not yet)------------------*/
   if (x_cur >  x_end) {
      DEBUG_YKINE_EXACT  yLOG_snote   ("not yet");
      DEBUG_YKINE_EXACT  yLOG_sexit   (__FUNCTION__);
      return  1;
   }
   /*---(check missed)-------------------*/
   if (x_cur <= x_beg) {
      DEBUG_YKINE_EXACT  yLOG_snote   ("too far");
      DEBUG_YKINE_EXACT  yLOG_sexit   (__FUNCTION__);
      return -1;
   }
   /*---(exactly)------------------------*/
   if (x_cur == x_end) {
      DEBUG_YKINE_EXACT  yLOG_snote   ("exactly");
      DEBUG_YKINE_EXACT  yLOG_sexit   (__FUNCTION__);
      return  2;
   }
   /*---(found it)-----------------------*/
   DEBUG_YKINE_EXACT  yLOG_snote   ("in range");
   DEBUG_YKINE_EXACT  yLOG_sexit   (__FUNCTION__);
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
   DEBUG_YKINE_EXACT  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (a_servo == NULL) {
      DEBUG_YKINE_EXACT  yLOG_note    ("null servo");
      DEBUG_YKINE_EXACT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   a_servo->exact = '-';
   a_servo->deg   = 0.0;
   a_servo->xexp  = 0.0;
   a_servo->zexp  = 0.0;
   a_servo->yexp  = 0.0;
   if (a_servo->curr == NULL)  x_next = a_servo->head;
   else                        x_next = a_servo->curr;
   /*---(walk thru moves)----------------*/
   while (x_next != NULL) {
      rc = ykine__exact_check (x_next, a_sec);
      if (rc == 0 || rc == 2) {
         DEBUG_YKINE_EXACT  yLOG_note    ("success, save values");
         a_servo->curr  = x_next;
         if (rc == 2)  a_servo->exact = 'y';
         else          a_servo->exact = 'n';
         DEBUG_YKINE_EXACT  yLOG_exit    (__FUNCTION__);
         return 0;
      }
      if      (rc ==  1)   x_next = x_next->s_next;
      else if (rc == -1)   x_next = x_next->s_prev;
   }
   /*---(complete)-----------------------*/
   --rce;
   a_servo->curr = NULL;
   DEBUG_YKINE_EXACT  yLOG_note    ("failed, current move never found");
   DEBUG_YKINE_EXACT  yLOG_exitr   (__FUNCTION__, --rce);
   return rce;
}

char
ykine_move_getservos    (char a_leg, char a_seg, tMOVE **a_curr, tMOVE **a_prev)
{
   /*---(locals)-----------+-----+-----+-*/
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_EXACT  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   if (a_curr != NULL)  *a_curr = NULL;
   if (a_prev != NULL)  *a_prev = NULL;
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_EXACT  yLOG_point   ("x_servo"   , x_servo);
   DEBUG_YKINE_EXACT  yLOG_info    ("->label"   , x_servo->label);
   if (x_servo != NULL) {
      *a_curr = x_servo->curr;
      if (*a_curr != NULL)  *a_prev = (*a_curr)->s_prev;
   }
   DEBUG_YKINE_EXACT  yLOG_point   ("*a_curr"   , *a_curr);
   DEBUG_YKINE_EXACT  yLOG_point   ("*a_prev"   , *a_prev);
   /*---complete)------------------------*/
   DEBUG_YKINE_EXACT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_move_copyout      (tMOVE *a_move, float *d, float *s, float *x, float *z, float *y, float *xz)
{
   if (a_move == NULL) {
      if (d  != NULL)  *d  = 0.0;
      if (s  != NULL)  *s  = 0.0;
      if (x  != NULL)  *x  = 0.0;
      if (z  != NULL)  *z  = 0.0;
      if (y  != NULL)  *y  = 0.0;
      if (xz != NULL)  *xz = 0.0;
      return 0;
   }
   if (d  != NULL)  *d  = a_move->degs;
   if (s  != NULL)  *s  = a_move->secs;
   if (x  != NULL)  *x  = a_move->x_pos;
   if (z  != NULL)  *z  = a_move->z_pos;
   if (y  != NULL)  *y  = a_move->y_pos - yKINE_seglen (YKINE_FOOT);
   if (xz != NULL)  *xz = a_move->xz_len;
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_exact_context     (char a_leg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_prev      = NULL;
   /*---(wipe)---------------------------*/
   myKINE.sb = myKINE.db = myKINE.cb = myKINE.fb = myKINE.pb = myKINE.tb = myKINE.xb = myKINE.zb = myKINE.yb = myKINE.xzb = myKINE.ob = 0.0;
   myKINE.sc = myKINE.dc = myKINE.cc = myKINE.fc = myKINE.pc = myKINE.tc = myKINE.xc = myKINE.zc = myKINE.yc = myKINE.xzc = myKINE.oc = 0.0;
   myKINE.se = myKINE.de = myKINE.ce = myKINE.fe = myKINE.pe = myKINE.te = myKINE.xe = myKINE.ze = myKINE.ye = myKINE.xze = myKINE.oe = 0.0;
   /*---(body/coxa)----------------------*/
   if (a_leg == YKINE_BODY) {
      ykine_move_getservos (a_leg , YKINE_FOCU , &x_curr, &x_prev);
      ykine_move_copyout   (x_prev, NULL      , NULL      , &myKINE.xb, &myKINE.zb, &myKINE.yb, &myKINE.xzb);
      ykine_move_copyout   (x_curr, NULL      , NULL      , &myKINE.xe, &myKINE.ze, &myKINE.ye, &myKINE.xze);
      if (x_curr != NULL)  myKINE.ov  = x_curr->verb;
      else                 myKINE.ov  = -1;
   }
   /*---(femur/yaw)----------------------*/
   ykine_move_getservos (a_leg , YKINE_FEMU , &x_curr, &x_prev);
   ykine_move_copyout   (x_prev, &myKINE.fb, NULL      , NULL      , NULL      , NULL      , NULL       );
   ykine_move_copyout   (x_curr, &myKINE.fe, NULL      , NULL      , NULL      , NULL      , NULL       );
   if (x_curr != NULL)  ++rc;
   /*---(patella/pitch)------------------*/
   ykine_move_getservos (a_leg , YKINE_PATE , &x_curr, &x_prev);
   ykine_move_copyout   (x_prev, &myKINE.pb, NULL      , NULL      , NULL      , NULL      , NULL       );
   ykine_move_copyout   (x_curr, &myKINE.pe, NULL      , NULL      , NULL      , NULL      , NULL       );
   if (x_curr != NULL)  ++rc;
   /*---(tibia/roll)---------------------*/
   ykine_move_getservos (a_leg , YKINE_TIBI , &x_curr, &x_prev);
   if (a_leg == YKINE_BODY) {
      ykine_move_copyout   (x_prev, &myKINE.tb, &myKINE.sb, NULL      , NULL      , NULL      , NULL       );
      ykine_move_copyout   (x_curr, &myKINE.te, &myKINE.se, NULL      , NULL      , NULL      , NULL       );
   } else {
      ykine_move_copyout   (x_prev, &myKINE.tb, &myKINE.sb, &myKINE.xb, &myKINE.zb, &myKINE.yb, &myKINE.xzb);
      ykine_move_copyout   (x_curr, &myKINE.te, &myKINE.se, &myKINE.xe, &myKINE.ze, &myKINE.ye, &myKINE.xze);
   }
   if (x_curr != NULL)  ++rc;
   /*---(coxa)---------------------------*/
   if (a_leg != YKINE_BODY && x_curr != NULL) {
      myKINE.cb  = yKINE_legdeg (a_leg);
      myKINE.ce  = myKINE.cb;
   } else {
      myKINE.cb  = 0.0;
      myKINE.ce  = 0.0;
   }
   /*---(verb)---------------------------*/
   if (x_curr != NULL)  myKINE.vb  = x_curr->verb;
   else                 myKINE.vb  = -1;
   /*---(report)-------------------------*/
   DEBUG_YKINE_EXACT  yLOG_value   ("verb"      , myKINE.vb);
   DEBUG_YKINE_EXACT  yLOG_complex ("b.angles"  , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , myKINE.cb, myKINE.fb, myKINE.pb, myKINE.tb);
   DEBUG_YKINE_EXACT  yLOG_complex ("b.coords"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", myKINE.xb, myKINE.zb, myKINE.yb, myKINE.xzb);
   DEBUG_YKINE_EXACT  yLOG_complex ("e.angles"  , "%8.3fc, %8.3ff, %8.3fp, %8.3ft" , myKINE.ce, myKINE.fe, myKINE.pe, myKINE.te);
   DEBUG_YKINE_EXACT  yLOG_complex ("e.coords"  , "%8.2fx, %8.2fz, %8.2fy, %8.2fxz", myKINE.xe, myKINE.ze, myKINE.ye, myKINE.xze);
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
   DEBUG_YKINE_EXACT  yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_EXACT  yLOG_double  ("a_sec"     , a_sec);
   /*---(save seconds)-------------------*/
   s_sec = a_sec;
   /*---(set current move)---------------*/
   for (i = 0; i < g_nservo; ++i) {
      x_servo   = &(g_servo_info [i]);
      rc = ykine__exact_find  (x_servo, s_sec);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_EXACT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> set all the servos to the right time --[ ------ [ ------ ]-*/
yKINE_exact_body        (float *x, float *z, float *y, float *a_yaw, float *a_pit, float *a_rol)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   float       x_pct       =  0.0;
   float       x_range     =  0.0;
   /*---(beg and end points)-------------*/
   ykine_exact_context (YKINE_BODY);
   /*---(calc percent)-------------------*/
   x_range = myKINE.se - myKINE.sb;
   if (x_range == 0.0)  x_pct   = 1.00;
   else                 x_pct   = (s_sec - myKINE.sb) / x_range;
   /*---(calc current endpoint)----------*/
   switch (myKINE.vb) {
   case YKINE_ZE : ykine_exact_pct_xzy     (x_pct);  break;
   case YKINE_ZP : ykine_exact_pct_xzy     (x_pct);  break;
   }
   /*---(save current endpoint)----------*/
   if (x     != NULL)  *x     = myKINE.xc;
   if (z     != NULL)  *z     = myKINE.zc;
   if (y     != NULL)  *y     = myKINE.yc;
   /*---(calc current angles)------------*/
   switch (myKINE.ov) {
   case YKINE_OR : ykine_exact_pct_xzy     (x_pct);  break;
   case YKINE_OP : ykine_exact_pct_xzy     (x_pct);  break;
   }
   /*---(save current endpoint)----------*/
   if (a_yaw != NULL)  *a_yaw = myKINE.fc;
   if (a_pit != NULL)  *a_pit = myKINE.pc;
   if (a_rol != NULL)  *a_rol = myKINE.tc;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> set all the servos to the right time --[ ------ [ ------ ]-*/
yKINE_exact_leg         (char a_leg, float *f, float *p, float *t, float *x, float *z, float *y, float *fr, float *pr, float *tr, float *xr, float *zr, float *yr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   float       x_pct       =  0.0;
   float       x_range     =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_EXACT  yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_EXACT  yLOG_value   ("a_leg"     , a_leg);
   /*---(beg and end points)-------------*/
   ykine_exact_context (a_leg);
   /*---(calc percent)-------------------*/
   x_range = myKINE.se - myKINE.sb;
   if (x_range == 0.0)  x_pct   = 1.00;
   else                 x_pct   = (s_sec - myKINE.sb) / x_range;
   /*---(calc current endpoint)----------*/
   switch (myKINE.vb) {
   case YKINE_FK :
   case YKINE_IK : case YKINE_TK : case YKINE_NK :
      ykine_exact_pct_xzy     (x_pct);
      break;
   case YKINE_CK : case YKINE_RK : case YKINE_SK :
      ykine_exact_pct_doy     (myKINE.vb, a_leg, x_pct);
      break;
   }
   /*---(save current endpoint)----------*/
   if (x     != NULL)  *x     = myKINE.xc;
   if (z     != NULL)  *z     = myKINE.zc;
   if (y     != NULL)  *y     = myKINE.yc;
   /*---(save current angles)------------*/
   myKINE.cc = myKINE.cb;
   if (f     != NULL)  *f     = myKINE.fc;
   if (p     != NULL)  *p     = myKINE.pc;
   if (t     != NULL)  *t     = myKINE.tc;
   /*---(inverse kinematics)-------------*/
   myKINE.rcc = yKINE_inverse_adapt (a_leg, myKINE.xc, myKINE.zc, myKINE.yc);
   --rce;  if (rc < 0)  return rce;
   rc = yKINE_endpoint (a_leg, YKINE_FOOT, YKINE_IK, NULL, NULL, &myKINE.xc, &myKINE.zc, &myKINE.yc, NULL);
   rc = yKINE_angles   (a_leg, YKINE_IK, NULL, &myKINE.fc, &myKINE.pc, &myKINE.tc);
   /*---(save revised endpoint)----------*/
   if (xr    != NULL)  *xr    = myKINE.xc;
   if (zr    != NULL)  *zr    = myKINE.zc;
   if (yr    != NULL)  *yr    = myKINE.yc;
   /*---(save revised angles)------------*/
   if (fr    != NULL)  *fr    = myKINE.fc;
   if (pr    != NULL)  *pr    = myKINE.pc;
   if (tr    != NULL)  *tr    = myKINE.tc;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_EXACT  yLOG_exit    (__FUNCTION__);
   return myKINE.rcc;
}



/*====================------------------------------------====================*/
/*===----                     unit testing support                     ----===*/
/*====================------------------------------------====================*/
static void      o___UNIT_TEST_______________o (void) {;}

char
ykine_exact_fake_beg    (float db, float sb, float xb, float zb, float yb, float ob)
{
   myKINE.db = db;
   myKINE.sb = sb;
   myKINE.xb = xb;
   myKINE.zb = zb;
   myKINE.yb = yb;
   myKINE.ob = ob;
   return 0;
}

char
ykine_exact_fake_end    (float de, float se, float xe, float ze, float ye, float oe)
{
   myKINE.de = de;
   myKINE.se = se;
   myKINE.xe = xe;
   myKINE.ze = ze;
   myKINE.ye = ye;
   myKINE.oe = oe;
   return 0;
}

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__exact_unit       (char *a_question, int a_cnt)
{
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "EXACT unit     : question not understood", LEN_STR);
   /*---(answer)-------------------------*/
   if (strcmp (a_question, "begin"   ) == 0) {
      sprintf (ykine__unit_answer, "EXACT begin    : %6.1lfd %6.3lfs %6.1lfx %6.1lfz %6.1lfy %6.1lfo %6.1lfc %6.1lff %6.1lfp %6.1lft", myKINE.db, myKINE.sb, myKINE.xb, myKINE.zb, myKINE.yb, myKINE.ob, myKINE.cb, myKINE.fb, myKINE.pb, myKINE.tb);
   }
   else if (strcmp (a_question, "end"     ) == 0) {
      sprintf (ykine__unit_answer, "EXACT end      : %6.1lfd %6.3lfs %6.1lfx %6.1lfz %6.1lfy %6.1lfo %6.1lfc %6.1lff %6.1lfp %6.1lft", myKINE.de, myKINE.se, myKINE.xe, myKINE.ze, myKINE.ye, myKINE.oe, myKINE.ce, myKINE.fe, myKINE.pe, myKINE.te);
   }
   else if (strcmp (a_question, "current" ) == 0) {
      sprintf (ykine__unit_answer, "EXACT cur (%3d): %6.1lfd %6.3lfs %6.1lfx %6.1lfz %6.1lfy %6.1lfo %6.1lfc %6.1lff %6.1lfp %6.1lft", myKINE.rcc, myKINE.dc, myKINE.sc, myKINE.xc, myKINE.zc, myKINE.yc, myKINE.oc, myKINE.cc, myKINE.fc, myKINE.pc, myKINE.tc);
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}


