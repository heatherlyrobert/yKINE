/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                        create moves                          ----===*/
/*====================------------------------------------====================*/
static void      o___CREATION________________o (void) {;};

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_accel__zero      (char a_verb, char a_rc, float x, float z, float y, float a_beat, char *a_label, char a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_complex ("parms"     , "%dm, %6.2fx, %6.2fz, %6.2fy, %8.2fb", a_verb, x, z, y, a_beat);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * myKINE.s_pace;
   DEBUG_YKINE  yLOG_value   ("s_secs"    , s);
   rc = ykine_move_create   (x_servo, YKINE_SERVO, a_verb, myKINE.s_tline, a_label, '-', a_cell, s);
   rc = ykine_move_add_zero (x_servo, a_rc, x, z, y);
   --rce;  if (rc <  0) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel__pure      (tSERVO *s, char r, float d, float x, float z, float y)
{
   ykine_move_add_pure  (s, r, d, x, z, y);
   DEBUG_YKINE   yLOG_complex (s->label, "pure  %3dr, %6.1fd, %6.1fx, %6.1fz, %6.1fy", r, d, x, z, y);
   return 0;
}

char
ykine_accel__adapt     (tSERVO *s, char r, float d, float x, float z, float y)
{
   ykine_move_add_adapt (s, r, d, x, z, y);
   DEBUG_YKINE   yLOG_complex (s->label, "adapt %3dr, %6.1fd, %6.1fx, %6.1fz, %6.1fy", r, d, x, z, y);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_accel__servo     (char a_verb, char a_rc, char a_leg, int a_seg, float a_deg, float a_beat, char *a_label, char a_part, char a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   float       d, x, y, z;
   char        x_meth      = YKINE_IK;
   char        PURE_RC     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_complex ("parms"     , "%dm, %dl, %ds, %8.2fd, %8.2fb, %s", a_verb, a_leg, a_seg, a_deg, a_beat, a_label);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * myKINE.s_pace;
   DEBUG_YKINE  yLOG_value   ("s_secs"    , s);
   rc = ykine_move_create (x_servo, YKINE_SERVO, a_verb, myKINE.s_tline, a_label, a_part, a_cell, s);
   switch (a_verb) {
   case YKINE_OR : case YKINE_TI :
      rc = ykine_move_add_orient (x_servo, a_rc, a_deg);
      break;
      /*> case YKINE_SYNC :                                                                                   <* 
       *>    switch (a_seg) {                                                                                 <* 
       *>    case YKINE_FEMU :                                                                                <* 
       *>       ykine_accel__pure    (x_servo, a_rc      , g_end.fd  , g_end.fx  , g_end.fz  , g_end.fy  );   <* 
       *>       ykine_accel__adapt   (x_servo, a_rc      , g_end.fd  , g_end.fx  , g_end.fz  , g_end.fy  );   <* 
       *>       break;                                                                                        <* 
       *>    case YKINE_PATE :                                                                                <* 
       *>       ykine_accel__pure    (x_servo, a_rc      , g_end.pd  , g_end.px  , g_end.pz  , g_end.py  );   <* 
       *>       ykine_accel__adapt   (x_servo, a_rc      , g_end.pd  , g_end.px  , g_end.pz  , g_end.py  );   <* 
       *>       break;                                                                                        <* 
       *>    case YKINE_TIBI :                                                                                <* 
       *>       ykine_accel__pure    (x_servo, a_rc      , g_end.td  , g_end.tx  , g_end.tz  , g_end.ty  );   <* 
       *>       ykine_accel__adapt   (x_servo, a_rc      , g_end.td  , g_end.tx  , g_end.tz  , g_end.ty  );   <* 
       *>       break;                                                                                        <* 
       *>    case YKINE_FOOT :                                                                                <* 
       *>       ykine_accel__pure    (x_servo, a_rc      , g_end.ed  , g_end.ex  , g_end.ez  , g_end.ey  );   <* 
       *>       ykine_accel__adapt   (x_servo, a_rc      , g_end.ed  , g_end.ex  , g_end.ez  , g_end.ey  );   <* 
       *>       break;                                                                                        <* 
       *>    }                                                                                                <* 
       *>    break;                                                                                           <*/
   case YKINE_FK :
      x_meth = YKINE_FK;
   default       :
      /*> rc = yKINE_endpoint  (a_leg, a_seg, x_meth, NULL, NULL, &x, &z, &y, NULL);   <*/
      switch (a_seg) {
      case YKINE_FEMU :
         ykine_move_add_pure  (x_servo, g_pure.rc , g_pure.fd , g_pure.fx , g_pure.fz , g_pure.fy );
         ykine_move_add_adapt (x_servo, g_adapt.rc, g_adapt.fd, g_adapt.fx, g_adapt.fz, g_adapt.fy);
         DEBUG_YKINE   yLOG_complex ("femu-pure" , "%3dr, %6.1fx, %6.1fz, %6.1fy", g_pure.rc , g_pure.fd , g_pure.fx , g_pure.fz , g_pure.fy );
         DEBUG_YKINE   yLOG_complex ("femu-adapt", "%3dr, %6.1fx, %6.1fz, %6.1fy", g_adapt.rc, g_adapt.fd, g_adapt.fx, g_adapt.fz, g_adapt.fy);
         break;
      case YKINE_PATE :
         ykine_move_add_pure  (x_servo, g_pure.rc , g_pure.pd , g_pure.px , g_pure.pz , g_pure.py );
         ykine_move_add_adapt (x_servo, g_adapt.rc, g_adapt.pd, g_adapt.px, g_adapt.pz, g_adapt.py);
         DEBUG_YKINE   yLOG_complex ("pate-pure" , "%3dr, %6.1fx, %6.1fz, %6.1fy", g_pure.rc , g_pure.pd , g_pure.px , g_pure.pz , g_pure.py );
         DEBUG_YKINE   yLOG_complex ("pate-adapt", "%3dr, %6.1fx, %6.1fz, %6.1fy", g_adapt.rc, g_adapt.pd, g_adapt.px, g_adapt.pz, g_adapt.py);
         break;
      case YKINE_TIBI :
         ykine_move_add_pure  (x_servo, g_pure.rc , g_pure.td , g_pure.tx , g_pure.tz , g_pure.ty );
         ykine_move_add_adapt (x_servo, g_adapt.rc, g_adapt.td, g_adapt.tx, g_adapt.tz, g_adapt.ty);
         DEBUG_YKINE   yLOG_complex ("tibi-pure" , "%3dr, %6.1fx, %6.1fz, %6.1fy", g_pure.rc , g_pure.td , g_pure.tx , g_pure.tz , g_pure.ty );
         DEBUG_YKINE   yLOG_complex ("tibi-adapt", "%3dr, %6.1fx, %6.1fz, %6.1fy", g_adapt.rc, g_adapt.td, g_adapt.tx, g_adapt.tz, g_adapt.ty);
         break;
      case YKINE_FOOT :
         ykine_move_add_pure  (x_servo, g_pure.rc , g_pure.ed , g_pure.ex , g_pure.ez , g_pure.ey );
         ykine_move_add_adapt (x_servo, g_adapt.rc, g_adapt.ed, g_adapt.ex, g_adapt.ez, g_adapt.ey);
         DEBUG_YKINE   yLOG_complex ("ends-pure" , "%3dr, %6.1fx, %6.1fz, %6.1fy", g_pure.rc , g_pure.ed , g_pure.ex , g_pure.ez , g_pure.ey );
         DEBUG_YKINE   yLOG_complex ("ends-adapt", "%3dr, %6.1fx, %6.1fz, %6.1fy", g_adapt.rc, g_adapt.ed, g_adapt.ex, g_adapt.ez, g_adapt.ey);
         break;
      }
      break;
   }
   /*> DEBUG_YKINE   yLOG_complex ("updated"   , "%8.2fx, %8.2fz, %8.2fy", x, z, y);   <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel__single     (char a_verb, char a_rc, char a_leg, float f, float p, float t, float b, char *a_label, char a_part, char a_cell)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(handle)-------------------------*/
   ykine_accel__servo (a_verb, a_rc, a_leg, YKINE_FEMU, f, b, a_label, a_part, a_cell);
   ykine_accel__servo (a_verb, a_rc, a_leg, YKINE_PATE, p, b, a_label, a_part, a_cell);
   ykine_accel__servo (a_verb, a_rc, a_leg, YKINE_TIBI, t, b, a_label, a_part, a_cell);
   ykine_accel__servo (a_verb, a_rc, a_leg, YKINE_FOOT, 0, b, a_label, a_part, a_cell);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_execute     (char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i, j;
   float       x_pct       =  0.0;
   float       x_adj       =  0.0;
   char        x_verb      =   -1;
   float       a           =  0.0;
   char        x_pure      =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_value   ("b"         , g_timing.beats);
   --rce;  if (g_timing.beats >= 0) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   for (j = 0; j < MAX_PARTS; ++j) {
      ykine_accel__restore (j);
      a = 0.0;
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i)  a += g_accel_info [i].dur [j];
      if (a <= 0.0)  continue;
      DEBUG_YKINE   yLOG_value   ("ACCEL PART", j);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         DEBUG_YKINE   yLOG_complex ("level"     , "%c %5.1fd %5.1fd %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist [j], g_accel_info [i].dur [j], g_accel_info [i].pct [j]);
         if (g_accel_info [i].dur [j]  < 0.1)   continue;
         x_pct  = g_accel_info [i].pct [j];
         x_adj  = g_accel_info [i].adj [j];
         x_verb = g_accel_ends [j].verb;
         DEBUG_YKINE   yLOG_complex ("keys"      , "%5.1fp %5.1fa %cv", x_pct, x_adj, x_verb);
         rc     = ykine_exact_pct_route (x_verb, x_pct);
         x_pure = ykine_exec_partial    (x_verb, myKINE.leg, 's');
         if (x_verb == YKINE_ZE || x_verb == YKINE_PO) {
            ykine_accel__zero   (x_verb, x_pure, g_cur.ex , g_cur.ez , g_cur.ey , x_adj, a_label, g_accel_info [i].abbr);
         } else {
            ykine_accel__single (x_verb, x_pure, myKINE.leg, g_cur.fd , g_cur.pd , g_cur.td , x_adj, a_label, g_accel_ends [j].part, g_accel_info [i].abbr);
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_immediate   (char a_verb, char a_rc, char a_leg, float a_beats, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i, j;
   float       x_pct       =  0.0;
   float       x_adj       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(normal)-------------------------*/
   --rce;  if (a_beats < 0)  {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE   yLOG_note    ("unaccelerated move");
   switch (a_verb) {
   case YKINE_ZE : case YKINE_PO :
      ykine_accel__zero   (a_verb, a_rc, g_end.ex , g_end.ez , g_end.ey , a_beats, a_label, YKINE_NONE);
      break;
   default  :
      ykine_accel__single (a_verb, a_rc, a_leg, g_end.fd , g_end.pd , g_end.td , a_beats, a_label, '-', YKINE_NONE);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_exec_wait         (char a_leg, float a_wait)
{
   char        rc          =    0;
   float       s           =  0.0;
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_complex ("args"      , "%d, %6.1fs", a_leg, a_wait);
   s = a_wait * g_timing.exact;
   DEBUG_YKINE   yLOG_complex ("secs"      , "%6.1fw, %6.1fx, %6.1fs", a_wait, g_timing.exact, s);
   rc = ykine_accel_immediate (YKINE_SYNC, 0, a_leg, s, "wait_before");
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
ykine_exec_fill         (char a_leg, float a_wait)
{
   char        rc          =    0;
   float       s           =  0.0;
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE   yLOG_complex ("args"      , "%d, %6.1fs", a_leg, a_wait);
   s = a_wait * g_timing.exact;
   DEBUG_YKINE   yLOG_complex ("secs"      , "%6.1fw, %6.1fx, %6.1fs", a_wait, g_timing.exact, s);
   rc = ykine_accel_immediate (YKINE_SYNC, 0, a_leg, s, "fill_after");
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return rc;
}




char         /*--> identify the servos effected ----------[ ------ [ ------ ]-*/
ykine_exec_servos       (char a_verb)
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
      break;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare list)-------------------*/
   ykine_servo_list (myKINE.servos);
   DEBUG_YKINE_SCRP   yLOG_info    ("servos"    , myKINE.servos);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_exec_get_prev     (int a_leg)
{
   /*---(locals)-----------+-----+-----+-*/
   tSERVO     *x_servo     =    0;
   char        rc          =    0;
   /*---(get coxa)-----------------------*/
   g_beg.cd   = yKINE_legdeg (a_leg);
   /*---(get femu)-----------------------*/
   x_servo = ykine_servo_pointer (a_leg, YKINE_FEMU);
   rc  = ykine_move_savedtail  (x_servo, NULL      , &g_beg.fd , &g_beg.fx , &g_beg.fz , &g_beg.fy , NULL);
   DEBUG_YKINE_SCRP  yLOG_complex ("femu"      , "%6.1fd, %6.1fx, %6.1fz, %6.1fy" , g_beg.fd , g_beg.fx , g_beg.fz , g_beg.fy );
   /*---(get pate)-----------------------*/
   x_servo = ykine_servo_pointer (a_leg, YKINE_PATE);
   rc  = ykine_move_savedtail  (x_servo, NULL      , &g_beg.pd , &g_beg.px , &g_beg.pz , &g_beg.py , NULL);
   DEBUG_YKINE_SCRP  yLOG_complex ("pate"      , "%6.1fd, %6.1fx, %6.1fz, %6.1fy" , g_beg.pd , g_beg.px , g_beg.pz , g_beg.py );
   /*---(get tibi)-----------------------*/
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   rc  = ykine_move_savedtail  (x_servo, &g_beg.sec, &g_beg.td , &g_beg.tx , &g_beg.tz , &g_beg.ty , &g_beg.xz  );
   DEBUG_YKINE_SCRP  yLOG_complex ("tibi"      , "%6.1fd, %6.1fx, %6.1fz, %6.1fy" , g_beg.td , g_beg.tx , g_beg.tz , g_beg.ty );
   /*---(get foot/end)-------------------*/
   if (a_leg != YKINE_BODY) {
      x_servo = ykine_servo_pointer (a_leg, YKINE_FOOT);
      rc  = ykine_move_savedtail  (x_servo, &g_beg.sec, &g_beg.ed , &g_beg.ex , &g_beg.ez , &g_beg.ey , NULL       );
   }
   /*---(or, get zero-point)-------------*/
   else {
      x_servo = ykine_servo_pointer (a_leg, YKINE_FOCU);
      rc  = ykine_move_savedtail  (x_servo, NULL, NULL, &g_beg.ex , &g_beg.ez , &g_beg.ey , &g_beg.xz  );
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("ends"      , "%6.1fd, %6.1fx, %6.1fz, %6.1fy" , g_beg.ed , g_beg.ex , g_beg.ez , g_beg.ey );
   /*---(complete)-----------------------*/
   return 0;
}






char         /*--> identify the servos effected ----------[ ------ [ ------ ]-*/
ykine_exec_prepare      (int a_line, char *a_one, char *a_two, char *a_thr, char *a_label, char *a_mods, char a_code, char *a_seg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(set line)--------------------*/
   myKINE.s_tline = a_line;
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
   if (g_timing.beats < 0.0) {
      rc = yPARSE_popstr    (a_mods);
      DEBUG_YKINE_SCRP  yLOG_complex ("a_mods"    , "%3d, %s", rc, a_mods);
      if (strcmp (a_mods , "-") == 0) {
         strlcpy (a_mods , "", LEN_LABEL);
      }
      ykine_stepping    (a_mods);
   } else ykine_stepping    ("");
   ykine_step_accels ();
   /*---(segment)---------------------*/
   if (a_seg != NULL) {
      switch (a_code) {
      case YKINE_FK :                 *a_seg = YKINE_FEMU;  break;
      case YKINE_ZE : case YKINE_PO : *a_seg = YKINE_FOCU;  break;
      case YKINE_OR : case YKINE_TI : *a_seg = YKINE_YAW;   break;
      default       :                 *a_seg = YKINE_TIBI;  break;
      }
      DEBUG_YKINE_SCRP   yLOG_value   ("a_seg"     , *a_seg);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_exec__begpoint    (char a_code, char a_leg, char *a_one, char *a_two, char *a_thr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(clear all values)---------------*/
   ykine_exact_clearall ();
   /*---(get beginning)------------------*/
   --rce;  switch (a_code) {
      /*---(zero-point)------------------*/
   case YKINE_ZE :  rc = ykine_body_ze_getter  (       a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_PO :  rc = ykine_body_po_getter  (       a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_OR :  rc = ykine_body_or_getter  (       a_one, a_two, a_thr, &g_end.fd , &g_end.pd , &g_end.td );  break;
   case YKINE_TI :  rc = ykine_body_ti_getter  (       a_one, a_two, a_thr, &g_end.fd , &g_end.pd , &g_end.td );  break;
      /*---(forward)---------------------*/
   case YKINE_FK :  rc = ykine__legs_fk_getter (a_leg, a_one, a_two, a_thr, &g_end.fd , &g_end.pd , &g_end.td );  break;
      /*---(inverse)---------------------*/
   case YKINE_IK :  rc = ykine__legs_ik_getter (a_leg, a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_CK :  rc = ykine__legs_ck_getter (a_leg, a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_RK :  rc = ykine__legs_rk_getter (a_leg, a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_TK :  rc = ykine__legs_tk_getter (a_leg, a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_NK :  rc = ykine__legs_nk_getter (a_leg, a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
   case YKINE_SK :  rc = ykine__legs_sk_getter (a_leg, a_one, a_two, a_thr, &g_end.ex , &g_end.ez , &g_end.ey );  break;
      /*---(trouble)---------------------*/
   default       :
                    DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, verb type not known");
                    DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
                    return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
   DEBUG_YKINE_SCRP   yLOG_value   ("sb"        , g_beg.sec);
   if (rc <  0)   ykine__legs_fallback ();
   switch (a_code) {
   case YKINE_OR : case YKINE_TI : case YKINE_FK :
      DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", g_end.fd , g_end.pd , g_end.td );
      break;
   default       :
      DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", g_end.ex , g_end.ez , g_end.ey );
      break;
   }
   return rc;
}

char
ykine_exec__endpoint    (char a_code, char a_leg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       x_sec_end   =  0.0;
   /*---(end seconds)--------------------*/
   x_sec_end = g_beg.sec + myKINE.step_total / 10.0;
   /*---(kinematics)------------------*/
   switch (a_code) {
   case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI :
      rc = 0;
      break;
   case YKINE_FK :
      DEBUG_YKINE_SCRP   yLOG_complex ("timing"    , "%6.1fsb, %6.1fst, %6.1f++, %6.1fse", g_beg.sec, myKINE.step_total, myKINE.step_total / 10.0, g_beg.sec);
      ykine_exact_setbody (x_sec_end);
      rc  = yKINE_forward (a_leg, g_end.fd , g_end.pd , g_end.td );
      ykine_exact_copy2pure  (x_sec_end, a_leg, YKINE_FK, rc);
      ykine_exact_copy2adapt (x_sec_end, a_leg, YKINE_FK, rc);
      DEBUG_YKINE_SCRP  yLOG_value   ("forward"   , rc);
      /*> if (rc <  0)   ykine__legs_fallback ();                                  <*/
      break;
   default       :
      DEBUG_YKINE_SCRP   yLOG_complex ("timing"    , "%6.1fsb, %6.1fst, %6.1f++, %6.1fse", g_beg.sec, myKINE.step_total, myKINE.step_total / 10.0, g_beg.sec);
      ykine_exact_setbody (x_sec_end);
      rc = yKINE_adapt   (a_leg, g_end.ex , g_end.ez , g_end.ey );
      ykine_exact_copy2adapt (x_sec_end, a_leg, YKINE_IK, rc);
      rc = yKINE_inverse (a_leg, g_end.ex , g_end.ez , g_end.ey );
      ykine_exact_copy2pure  (x_sec_end, a_leg, YKINE_IK, rc);
      DEBUG_YKINE_SCRP  yLOG_complex ("kinematics", "inverse %3d, adapted %3d", g_pure.rc, g_adapt.rc);
      break;
   }
   ykine_step_show ("PURE AND ADAPT DONE");
   /*---(set the endpoint)------------*/
   switch (a_code) {
   case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI :
      rc = 0;
      break;
      /*> case YKINE_FK :                                                                                                      <* 
       *>    rc = yKINE_endpoint (a_leg, YKINE_FOOT, YKINE_FK, NULL, NULL, &g_end.ex , &g_end.ez , &g_end.ey , NULL);          <* 
       *>    DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", g_end.ex , g_end.ez , g_end.ey );   <* 
       *>    break;                                                                                                            <*/
   default       :
      ykine_step_copy (&g_end, &g_pure);
      ykine_step_show ("AFTER UPDATE");
      /*> rc = yKINE_angles   (a_leg, YKINE_IK, &g_end.cd , &g_end.fd , &g_end.pd , &g_end.td );   <*/
      /*> DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", g_end.fd , g_end.pd , g_end.td );   <*/
      break;
   }
   return rc;
}

char         /*--> handle tangent verbs ------------------[ ------ [ ------ ]-*/
ykine_exec_driver       (int a_line, uchar *a_verb, char a_code)
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
   char        x_pure      =    0;
   char        x_adapt     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("args"      , "%dn, %s, %dc", a_line, a_verb, a_code);
   /*---(prepare verb)----------------*/
   rc = ykine_scrp_verb (a_verb);
   DEBUG_YKINE_SCRP   yLOG_value   ("verb"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare servos)--------------*/
   rc = ykine_exec_servos (a_code);
   DEBUG_YKINE_SCRP   yLOG_value   ("servos"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark servers)----------------*/
   rc = ykine_exec_prepare (a_line, x_one, x_two, x_thr, x_label, x_mods, a_code, &x_seg);
   DEBUG_YKINE_SCRP   yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      rc = yKINE_servo_index (x_leg, x_seg);
      if (rc < 0)                     continue;
      if (myKINE.servos [rc] == '_')  continue;
      DEBUG_YKINE_SCRP   yLOG_complex ("LEG"       , "%d, %s, %s", x_leg, yKINE_legtwo (x_leg), yKINE_legfull (x_leg));
      /*---(get positions)---------------*/
      rc = ykine_exec__begpoint (a_code, x_leg, x_one, x_two, x_thr);
      /*---(inverse kinematics)----------*/
      rc = ykine_exec__endpoint (a_code, x_leg);
      /*---(if step, create raise)-------*/
      rc = ykine_accel_reset (x_leg);
      rc = ykine_step_wait   (a_code, x_leg);
      rc = ykine_step_raise  (a_code);
      /*---(get distance)----------------*/
      ykine_exact_dist_route (a_code);
      DEBUG_YKINE_SCRP  yLOG_double  ("distance"  , g_end.len);
      /*---(process moves)---------------*/
      DEBUG_YKINE_SCRP   yLOG_complex ("beg"       , "%6.1fx, %6.1fz, %6.1fy", g_beg.ex , g_beg.ez , g_beg.ey );
      DEBUG_YKINE_SCRP   yLOG_complex ("end"       , "%6.1fx, %6.1fz, %6.1fy", g_end.ex , g_end.ez , g_end.ey );
      DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", g_end.fd , g_end.pd , g_end.td );
      if (g_timing.beats >= 0)  ykine_accel_immediate (a_code, x_pure, x_leg, g_timing.beats    , x_label);
      else                      ykine_accel_append    (a_code, 'm', g_timing.a_middle);
      /*---(if step, create plant)-------*/
      rc = ykine_step_plant    (a_code);
      rc = ykine_accel_execute (x_label);
      rc = ykine_step_fill   (a_code, x_leg);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       partial moves                          ----===*/
/*====================------------------------------------====================*/
static void      o___PARTIAL_________________o (void) {;}

char
ykine_exec_partial      (char a_verb, char a_leg, char a_ik)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   float       s, x, z, y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_complex ("params"    , "verb %d, leg %d, ik %c", a_verb, a_leg, a_ik);
   /*---(calculate)----------------------*/
   if (a_verb == YKINE_OR || a_verb == YKINE_TI) {
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_verb == YKINE_ZE || a_verb == YKINE_PO) {
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(forward)------------------------*/
   s = g_cur.sec;
   ykine_exact_setbody (s);
   /*---(forward)------------------------*/
   if (a_verb == YKINE_FK) {
      DEBUG_YKINE_EXACT   yLOG_note    ("forward positioning");
      DEBUG_YKINE_EXACT   yLOG_complex ("input"     , "%8.2ff, %8.2fp, %8.2ft", g_cur.fd , g_cur.pd , g_cur.td );
      rc = yKINE_forward  (a_leg, g_cur.fd , g_cur.pd , g_cur.td );
      yKINE_endpoint (a_leg, YKINE_FOOT, YKINE_FK, NULL, NULL, &g_cur.ex , &g_cur.ez , &g_cur.ey , NULL);
      ykine_exact_copy2pure  (s, a_leg, YKINE_FK, rc);
      ykine_exact_copy2adapt (s, a_leg, YKINE_FK, rc);
      ykine_step_copy        (&g_cur, &g_pure);
      DEBUG_YKINE_EXACT   yLOG_complex ("output"    , "%8.2fx, %8.2fz, %8.2fy", g_cur.ex , g_cur.ez , g_cur.ey );
   }
   /*---(inverse)------------------------*/
   else {
      DEBUG_YKINE_EXACT   yLOG_note    ("inverse positioning");
      DEBUG_YKINE_EXACT   yLOG_complex ("input"     , "%8.2fx, %8.2fz, %8.2fy", g_cur.ex , g_cur.ez , g_cur.ey );
      x = g_cur.ex;
      z = g_cur.ez;
      y = g_cur.ey;
      switch (a_ik) {
      case 'e' :
         DEBUG_YKINE_EXACT   yLOG_note    ("ticker/progress, end with adapted");
         rc = yKINE_inverse     (a_leg, x , z , y );
         ykine_exact_copy2pure  (s, a_leg, YKINE_IK, rc);
         ykine_step_copy        (&g_cur, &g_pure);
         DEBUG_YKINE_EXACT   yLOG_complex ("inverse"   , "rc %3d, %8.2ff, %8.2fp, %8.2ft  copied  rc %3d, %8.2ff, %8.2fp, %8.2ft", rc, g_cur.fd , g_cur.pd , g_cur.td , g_pure.rc , g_pure.fd , g_pure.pd , g_pure.td );
         rc = yKINE_adapt       (a_leg, x , z , y );
         ykine_exact_copy2adapt (s, a_leg, YKINE_IK, rc);
         ykine_step_copy        (&g_cur, &g_adapt);
         DEBUG_YKINE_EXACT   yLOG_complex ("apapted"   , "rc %3d, %8.2ff, %8.2fp, %8.2ft  copied  rc %3d, %8.2ff, %8.2fp, %8.2ft", rc, g_cur.fd , g_cur.pd , g_cur.td , g_adapt.rc, g_adapt.fd, g_adapt.pd, g_adapt.td);
         break;
      case 's' :
         DEBUG_YKINE_EXACT   yLOG_note    ("script reading, end with pure/inverse");
         rc = yKINE_adapt   (a_leg, x , z , y );
         ykine_exact_copy2adapt (s, a_leg, YKINE_IK, rc);
         DEBUG_YKINE_EXACT   yLOG_complex ("apapted"   , "rc %3d, %8.2ff, %8.2fp, %8.2ft  copied  rc %3d, %8.2ff, %8.2fp, %8.2ft", rc, g_cur.fd , g_cur.pd , g_cur.td , g_adapt.rc, g_adapt.fd, g_adapt.pd, g_adapt.td);
         rc = yKINE_inverse (a_leg, x , z , y );
         ykine_step_copy        (&g_cur, &g_adapt);
         ykine_exact_copy2pure  (s, a_leg, YKINE_IK, rc);
         ykine_step_copy        (&g_cur, &g_pure);
         DEBUG_YKINE_EXACT   yLOG_complex ("inverse"   , "rc %3d, %8.2ff, %8.2fp, %8.2ft  copied  rc %3d, %8.2ff, %8.2fp, %8.2ft", rc, g_cur.fd , g_cur.pd , g_cur.td , g_pure.rc , g_pure.fd , g_pure.pd , g_pure.td );
         break;
      }
      /*> yKINE_angles   (a_leg, YKINE_IK, NULL, &g_cur.fd , &g_cur.pd , &g_cur.td );   <*/
      DEBUG_YKINE_EXACT   yLOG_complex ("output"    , "%8.2ff, %8.2fp, %8.2ft", g_cur.fd , g_cur.pd , g_cur.td );
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("rc"        , rc);
   /*---(handle errors)------------------*/
   --rce;  if (rc < 0)  {
      DEBUG_YKINE_MOVE  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

