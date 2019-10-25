/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"

/*===[[ DESIGN NOTES ]]=======================================================*/

/*  accel takes the distance and calculates a acceleration/deceleration table
 *  to smoothly travel the length.  besides distance, three factors effect the
 *  final table -- accel type, max speed, and decel type.
 *
 *     accel type      [ accel from zero, < no acceleraction required
 *     max speed       t/turtle, s/slow, m/moderate, f/fast, x/extra fast
 *       double time   T/turtle, S/slow, M/moderate, F/fast, X/extra fast
 *     decel type      ] decel to zero, > no deceleration
 *
 *  integration
 *     ykine_exact_dist_route    total distance calc (based on verb type)
 *     ykine_exact_pct_route     position for percent (based on verb type)
 *
 *
 *
 *
 *
 *
 *
 */



tACCEL g_accel_info [10] = {
   { 't', "turtle"     ,  2.0,  0.0,  0.0,  0.0 },
   { 's', "slow"       ,  4.0,  0.0,  0.0,  0.0 },
   { 'm', "moderate"   ,  8.0,  0.0,  0.0,  0.0 },
   { 'f', "fast"       , 12.0,  0.0,  0.0,  0.0 },  /* 12, 14, or 16 ? */
   { 'x', "extra fast" , 16.0,  0.0,  0.0,  0.0 },  /* 16, 25, or 32 ? */
   { 'f', "fast"       , 12.0,  0.0,  0.0,  0.0 },  /* 12, 14, or 16 ? */
   { 'm', "moderate"   ,  8.0,  0.0,  0.0,  0.0 },
   { 's', "slow"       ,  4.0,  0.0,  0.0,  0.0 },
   { 't', "turtle"     ,  2.0,  0.0,  0.0,  0.0 },
   { '/', "noop"       ,  0.0,  0.0,  0.0,  0.0 },
};

static char    s_accel    = '-';
static char    s_decel    = '-';
static char    s_speed    = '-';
static float   s_exact    =  0.0;
static float   s_step     =  1.0;



/*====================------------------------------------====================*/
/*===----                        calculations                          ----===*/
/*====================------------------------------------====================*/
static void      o___CALCULATION_____________o (void) {;};

char         /*--> clear the acceleration table ----------[ ------ [ ------ ]-*/
ykine_accel__clear  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*---(clear)--------------------------*/
   for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
      g_accel_info [i].dur  = 0.0;
      g_accel_info [i].pct  = 0.0;
      g_accel_info [i].dist = 0.0;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> calculate acceleration steps ----------[ ------ [ ------ ]-*/
ykine_accel__level  (char a_max, char a_level, char a_accel, char a_decel, float a_step, float *a_rem)
{  /*---(design notes)-------------------*/
   /* contains no globals to aid with isolation and unit testing
   */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       x_persec    =  0.0;
   char        x_maxes     [LEN_LABEL] = "tsmfx";
   char       *p           = NULL;
   int         x_max       =    0;
   float       a           =  0.0;
   char        x_lvl       =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_value   ("a_max"     , a_max);
   p    = strchr (x_maxes, a_max);
   DEBUG_YKINE_MOVE   yLOG_point   ("p"         , p);
   --rce;  if (a_max == '\0' || p == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_max    = p - x_maxes;
   DEBUG_YKINE_MOVE   yLOG_value   ("x_max"     , x_max);
   DEBUG_YKINE_MOVE   yLOG_value   ("a_level"   , a_level);
   --rce;  if (a_level < 0 || a_level > 4) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_lvl    = x_maxes [a_level];
   DEBUG_YKINE_MOVE   yLOG_char    ("x_lvl"     , x_lvl);
   if (a_level > x_max) {
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_YKINE_MOVE   yLOG_point   ("a_rem"     , a_rem);
   --rce;  if (a_rem == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (*a_rem <  0.1) {
      *a_rem = 0.0;
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(prepare)------------------------*/
   x_persec = g_accel_info [a_level].persec;
   /*---(max level)----------------------*/
   if (x_max == a_level && *a_rem >= 0.1) {
      DEBUG_YKINE_MOVE   yLOG_note    ("max, so all at this level");
      g_accel_info [0 + a_level].dist += *a_rem;
      *a_rem = 0.0;
   }
   if (*a_rem > 0.1 && *a_rem <= 2.0 * x_persec) {
      DEBUG_YKINE_MOVE   yLOG_note    ("all will fit (2x) at this level");
      g_accel_info [0 + a_level].dist += *a_rem;
      *a_rem = 0.0;
   }
   /*---(acceleration)-------------------*/
   if (a_accel == '[' && *a_rem >= 0.1 && a_level < ACCEL_EXTRA) {
      DEBUG_YKINE_MOVE   yLOG_note    ("adding to accel");
      if (*a_rem < x_persec)   *a_rem -= g_accel_info [0 + a_level].dist = *a_rem;
      else                     *a_rem -= g_accel_info [0 + a_level].dist = x_persec;
   }
   /*---(deceleration)-------------------*/
   if (a_decel == ']' && *a_rem >= 0.1 && a_level < ACCEL_EXTRA) {
      DEBUG_YKINE_MOVE   yLOG_note    ("adding to decel");
      if (*a_rem < x_persec)   *a_rem -= g_accel_info [8 - a_level].dist = *a_rem;
      else                     *a_rem -= g_accel_info [8 - a_level].dist = x_persec;
   }
   /*---(wrap up)------------------------*/
   if (*a_rem < 0.1)  *a_rem = 0.0;
   if (g_accel_info [0 + a_level].dist > 0.0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("set accel duration");
      a = (g_accel_info [0 + a_level].dist / x_persec) * a_step;
      g_accel_info [0 + a_level].dur = round (a * 10) / 10;
      if (g_accel_info [0 + a_level].dur < 0.1) g_accel_info [0 + a_level].dur = 0.1;
   }
   if (g_accel_info [8 - a_level].dist > 0.0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("set decel duration");
      a = (g_accel_info [8 - a_level].dist / x_persec) * a_step;
      g_accel_info [8 - a_level].dur = round (a * 10) / 10;
      if (g_accel_info [8 - a_level].dur < 0.1) g_accel_info [8 - a_level].dur = 0.1;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> manage acceleration process -----------[ ------ [ ------ ]-*/
ykine_accel_calc        (char a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       xd, zd, yd, dd, pd, ld, x_radius;
   float       x_dist, x_total, x_cum;
   float       x_dur, x_scale;
   float       d = 0, x = 0, z = 0, y = 0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(init)---------------------------*/
   ykine_accel__clear ();
   /*---(prepare distance)---------------*/
   ykine_exact_dist_route (a_verb);
   x_dist = x_total = myKINE.le;
   /*---(fill)---------------------------*/
   for (i = 0; i <= 4; ++i) {
      ykine_accel__level (s_speed, i, s_accel, s_decel, s_step, &x_dist);
   }
   /*---(percents)-----------------------*/
   x_cum = x_dur = 0.0;
   if (x_total > 0.0) {
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         if (g_accel_info [i].dist == 0.0)  continue;
         g_accel_info [i].pct = x_cum += g_accel_info [i].dist / x_total;
         x_dur += g_accel_info [i].dur;
         DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fs %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
      }
   }
   /*---(exact/limited)------------------*/
   DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fs %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
   /*> printf ("s_exact %6.1f, x_total %6.1f\n", s_exact, x_total);                   <*/
   if (s_exact > 0.00) {
      /*---(scale)-------------*/
      if (x_dur >= 0.10) {
         x_scale = s_exact / x_dur;
         for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
            if (g_accel_info [i].dist == 0.0)  continue;
            g_accel_info [i].dur *= x_scale;
            DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fs %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
         }
      }
      /*---(fill)--------------*/
      else {
         g_accel_info [DECEL_NOOP].dur   = s_exact;
         g_accel_info [DECEL_NOOP].pct   = 1.00;
         g_accel_info [DECEL_NOOP].dist  = 0.00;
      }
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel__defaults   (void)
{
   s_accel = '-';
   s_decel = '-';
   s_speed = '-';
   s_exact = 0.0;
   s_step  = 1.0;
   return 0;
}

char
ykine_accel_dur         (cchar *a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char        t           [LEN_LABEL];
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   ykine_accel__defaults ();
   /*> printf ("s_accel %c, s_decel %c, s_speed %c, s_step %6.1f, s_exact %6.1f\n", s_accel, s_decel, s_speed, s_step, s_exact);   <*/
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_point   ("a_dur"     , a_dur);
   --rce;  if (a_dur   == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(filter)-------------------------*/
   x_len = strlen (a_dur);
   DEBUG_YKINE_MOVE   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len < 3) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accel)--------------------------*/
   s_accel = a_dur [0];
   DEBUG_YKINE_MOVE   yLOG_char    ("s_accel"   , s_accel);
   --rce;  if (strchr ("<["        , s_accel) == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(decel)--------------------------*/
   s_decel = a_dur [x_len - 1];
   DEBUG_YKINE_MOVE   yLOG_char    ("decel"     , s_decel);
   --rce;  if (strchr ("]>"        , s_decel) == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(pace)---------------------------*/
   s_speed = tolower (a_dur [x_len - 2]);
   DEBUG_YKINE_MOVE   yLOG_char    ("s_speed"   , s_speed);
   --rce;  if (strchr ("tsmfx", s_speed) == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (s_speed != a_dur [x_len - 2])  s_step = 0.5;
   DEBUG_YKINE_MOVE   yLOG_double  ("s_step"    , s_step);
   /*---(exact timing)-------------------*/
   if (x_len > 3) {
      DEBUG_YKINE_MOVE   yLOG_note    ("exact version");
      sprintf (t, "%*.*s", x_len - 3, x_len - 3, a_dur + 1);
      /*> strncpy (t, a_dur + 1, x_len - 3);                                          <*/
      DEBUG_YKINE_MOVE   yLOG_info    ("t"         , t);
      /*> printf ("a_dur %s, t %s\n", a_dur, t);                                      <*/
      x_len = strlen (t);
      DEBUG_YKINE_MOVE   yLOG_value   ("x_len"     , x_len);
      --rce;  for (i = 0; i < x_len; ++i) {
         if (strchr ("0123456789.", t [i]) != NULL)  continue;
         ykine_accel__defaults ();
         DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      s_exact = atof (t);
      DEBUG_YKINE_MOVE   yLOG_double  ("s_exact"   , s_exact);
      --rce;  if (s_exact < 0.1) {
         ykine_accel__defaults ();
         DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*> printf ("s_accel %c, s_decel %c, s_speed %c, s_step %6.1f, s_exact %6.1f\n", s_accel, s_decel, s_speed, s_step, s_exact);   <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_timing      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(check accel)-----------------*/
   yPARSE_top      (myKINE.accel);
   DEBUG_YKINE_SCRP  yLOG_info    ("accel"     , myKINE.accel);
   rc  = ykine_accel_dur (myKINE.accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);
   /*---(check normal)----------------*/
   yPARSE_popval (0.0, &myKINE.b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , myKINE.b);
   /*---(if normal)-------------------*/
   if (rc < 0)   strlcpy (myKINE.accel, "", LEN_LABEL);
   /*---(if accelerated)--------------*/
   else          myKINE.b = -1.0;
   DEBUG_YKINE_SCRP  yLOG_info    ("accel'"    , myKINE.accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("b'"        , myKINE.b);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        create moves                          ----===*/
/*====================------------------------------------====================*/
static void      o___CREATION________________o (void) {;};

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_accel__zero      (char a_verb, float x, float z, float y, float a_beat, char *a_label, char a_cell)
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
ykine_accel__servo     (char a_verb, char a_leg, int a_seg, float a_deg, float a_beat, char *a_label, char a_cell)
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
ykine_accel__single     (char a_verb, char a_leg, float f, float p, float t, float b, char *a_label, char a_cell)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(handle)-------------------------*/
   ykine_accel__servo (a_verb, a_leg, YKINE_FEMU, f, b, a_label, a_cell);
   ykine_accel__servo (a_verb, a_leg, YKINE_PATE, p, b, a_label, a_cell);
   ykine_accel__servo (a_verb, a_leg, YKINE_TIBI, t, b, a_label, a_cell);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_create      (char a_verb, char a_leg, float a_beats, char *a_accel, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i           =    0;
   float       x_pct       =  0.0;
   float       x_dur       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(normal)-------------------------*/
   if (a_beats >= 0)  {
      DEBUG_YKINE_SCRP   yLOG_note    ("unaccelerated move");
      switch (a_verb) {
      case YKINE_ZE : case YKINE_PO :
         ykine_accel__zero   (a_verb, myKINE.xe, myKINE.ze, myKINE.ye, a_beats, a_label, YKINE_NONE);
         break;
      default  :
         ykine_accel__single (a_verb, a_leg, myKINE.fe, myKINE.pe, myKINE.te, a_beats, a_label, YKINE_NONE);
         break;
      }
   }
   /*---(accelerated)--------------------*/
   else {
      DEBUG_YKINE_SCRP   yLOG_note    ("accelerated move");
      rc = ykine_accel_calc (a_verb);
      --rce;  if (rc <  0) {
         DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fd %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
         if (g_accel_info [i].dur  < 0.1)   continue;
         x_pct = g_accel_info [i].pct;
         x_dur = g_accel_info [i].dur;
         rc = ykine_exact_pct_route (a_verb, a_leg, x_pct);
         rc = ykine_legs_partial    (a_verb, a_leg, 's');
         if (a_verb == YKINE_ZE || a_verb == YKINE_PO) {
            ykine_accel__zero   (a_verb, myKINE.xc, myKINE.zc, myKINE.yc, x_dur, a_label, g_accel_info [i].abbr);
         } else {
            ykine_accel__single (a_verb, a_leg, myKINE.fc, myKINE.pc, myKINE.tc, x_dur, a_label, g_accel_info [i].abbr);
         }
         /*---(done)------------------------*/
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine_accel__unit       (char *a_question, int a_num)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        t           [LEN_LABEL];
   char        x_msg       [LEN_STR  ];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "MOVE unit      : question not understood", LEN_STR);
   if      (strcmp (a_question, "dist"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].dist == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dist);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL dist     :%s", x_msg);
   }
   else if (strcmp (a_question, "durs"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].dur  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dur);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL durs     :%s", x_msg);
   }
   else if (strcmp (a_question, "cums"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].pct  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);
         else                          sprintf (t, " %4.2f", g_accel_info [i].pct);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL cums     :%s", x_msg);
   }
   else if (strcmp (a_question, "parse"     ) == 0) {
      sprintf (ykine__unit_answer, "ACCEL parse    : %c %c %c %6.1f %6.1f", s_accel, s_decel, s_speed, s_exact, s_step);
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}





