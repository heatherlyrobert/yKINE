/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



tACCEL g_accel_info [10] = {
   { 't', "turtle"     ,  2.0,  0.0,  0.0,  0.0 },
   { 's', "slow"       ,  4.0,  0.0,  0.0,  0.0 },
   { 'm', "moderate"   ,  8.0,  0.0,  0.0,  0.0 },
   { 'f', "fast"       , 12.0,  0.0,  0.0,  0.0 },
   { 'x', "extra fast" , 16.0,  0.0,  0.0,  0.0 },
   { 'f', "fast"       , 12.0,  0.0,  0.0,  0.0 },
   { 'm', "moderate"   ,  8.0,  0.0,  0.0,  0.0 },
   { 's', "slow"       ,  4.0,  0.0,  0.0,  0.0 },
   { 't', "turtle"     ,  2.0,  0.0,  0.0,  0.0 },
};



char         /*--> clear the acceleration table ----------[ ------ [ ------ ]-*/
ykine_accel_clear  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*---(clear)--------------------------*/
   for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
      g_accel_info [i].dur  = 0.0;
      g_accel_info [i].pct  = 0.0;
      g_accel_info [i].dist = 0.0;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> calculate acceleration steps ----------[ ------ [ ------ ]-*/
ykine_accel_level  (char a_max, char a_level, char a_accel, char a_decel, float a_step, float *a_rem)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_persec    =  0.0;
   char        x_maxes     [LEN_LABEL] = "tsmfx";
   int         x_max       =    0;
   float       a           =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_complex ("params"    , "%c %d %c %c %4.1f %4.1f", a_max, a_level, a_accel, a_decel, a_step, *a_rem);
   x_max    = strchr (x_maxes, a_max) - x_maxes;
   /*> printf ("%c, %d, %d\n", a_max, x_max, a_level);                                <*/
   if (a_level > x_max) {
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return 0;
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
      g_accel_info [0 + a_level].dur = trunc (a * 10) / 10;
      if (g_accel_info [0 + a_level].dur < 0.1) g_accel_info [0 + a_level].dur = 0.1;
   }
   if (g_accel_info [8 - a_level].dist > 0.0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("set decel duration");
      a = (g_accel_info [8 - a_level].dist / x_persec) * a_step;
      g_accel_info [8 - a_level].dur = trunc (a * 10) / 10;
      if (g_accel_info [8 - a_level].dur < 0.1) g_accel_info [8 - a_level].dur = 0.1;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> manage acceleration process -----------[ ------ [ ------ ]-*/
ykine_accel_calc        (float xb, float zb, float yb, float xe, float ze, float ye, char a_speed, char a_accel, char a_decel)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       xd, zd, yd;
   float       x_dist, x_total, x_cum;
   float       x_step      =  1.0;
   char        x_speed;
   int         i           =    0;
   /*---(init)---------------------------*/
   ykine_accel_clear ();
   /*---(prepare)------------------------*/
   x_speed = tolower (a_speed);
   if (x_speed != a_speed)  x_step = 0.5;
   xd     = xe - xb;
   zd     = ze - zb;
   yd     = ye - yb;
   x_total = x_dist = sqrt ((xd * xd) + (zd * zd) + (yd * yd));
   /*---(fill)---------------------------*/
   ykine_accel_level (a_speed, 0, a_accel, a_decel, x_step, &x_dist);
   ykine_accel_level (a_speed, 1, a_accel, a_decel, x_step, &x_dist);
   ykine_accel_level (a_speed, 2, a_accel, a_decel, x_step, &x_dist);
   ykine_accel_level (a_speed, 3, a_accel, a_decel, x_step, &x_dist);
   ykine_accel_level (a_speed, 4, a_accel, a_decel, x_step, &x_dist);
   /*---(percents)-----------------------*/
   x_cum = 0.0;
   if (x_total > 0.0) {
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         if (g_accel_info [i].dist == 0.0)  continue;
         g_accel_info [i].pct = x_cum += g_accel_info [i].dist / x_total;
         DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fd %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist, g_accel_info [i].dur, g_accel_info [i].pct);
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_accel_dur         (char *a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   DEBUG_YKINE_MOVE   yLOG_point   ("a_dur"     , a_dur);
   --rce;  if (a_dur   == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("len"       , strlen (a_dur));
   --rce;  if (strlen (a_dur) != 3) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_char    ("accel"     , a_dur [0]);
   --rce;  if (strchr ("<["        , a_dur [0]) == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_char    ("speed"     , a_dur [1]);
   --rce;  if (strchr ("tsmfxTSMFX", a_dur [1]) == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_char    ("decel"     , a_dur [2]);
   --rce;  if (strchr ("]>"        , a_dur [2]) == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   return 0;
}

char
ykine_noaccel_servo     (tSERVO *a_servo, float s, float d, float x, float z, float y)
{
   ykine_move_create (YKINE_MOVE_SERVO, a_servo, myKINE.s_verb, myKINE.s_cline, d, s);
   ykine_move_addloc (a_servo, x, z, y);
   return 0;
}

char
ykine_accel_body        (char *a_dur, float xb, float zb, float yb, float xe, float ze, float ye)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tSERVO     *x_servo     = NULL;
   float       d , x , z , y , l , f , p , t;   /* single move                */
   float       x_pct;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_MOVE  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get before values)--------------*/
   DEBUG_YKINE_MOVE   yLOG_complex ("before"    , "%8.2fx, %8.2fz, %8.2fy", xb, zb, yb);
   DEBUG_YKINE_MOVE   yLOG_complex ("after"     , "%8.2fx, %8.2fz, %8.2fy", xe, ze, ye);
   /*---(calculate)----------------------*/
   rc = ykine_accel_calc (xb, zb, yb, xe, ze, ye, a_dur [1], a_dur [0], a_dur [2]);
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
      ykine_exact_calc ('-', xb, xe, x_pct, &x);
      ykine_exact_calc ('-', zb, ze, x_pct, &z);
      ykine_exact_calc ('-', yb, ye, x_pct, &y);
      DEBUG_YKINE_MOVE   yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
      /*---(caclulate polar)-------------*/
      rc = ykine__scrp_zero_deg  (xe, ze, &d, &l);
      DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);
      /*---(create moves)----------------*/
      if (myKINE.s_hidden != 'y') {
         rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_cline, d, g_accel_info [i].dur * myKINE.s_pace);
         rc = ykine_move_addloc (x_servo, x, z, y);
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_leg         (char a_meth, int a_leg, char *a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tSERVO     *x_servo     = NULL;
   float       db, xb, zb, yb, cb, fb, pb, tb;  /* before move                */
   float       de, xe, ze, ye, ce, fe, pe, te;  /* after move                 */
   float       d , x , z , y , l , f , p , t;   /* single move                */
   float       x_pct;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_point   ("a_dur"     , a_dur);
   /*---(get servo)----------------------*/
   x_servo   = ykine_servo_pointer (a_leg, YKINE_TIBI);
   DEBUG_YKINE_MOVE   yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get before values)--------------*/
   ykine_move_savedloc (x_servo, NULL, &db, &xb, &zb, &yb, NULL);
   DEBUG_YKINE_MOVE   yLOG_complex ("before"    , "%8.2fd, %8.2fx, %8.2fz, %8.2fy", db, xb, zb, yb);
   /*---(get after values)---------------*/
   rc  = yKINE_endpoint    (a_leg, YKINE_TIBI, a_meth, &de, NULL, &xe, &ze, &ye);
   rc  = yKINE_angles      (a_leg, a_meth, &ce, &fe, &pe, &te);
   de  = te;
   DEBUG_YKINE_MOVE   yLOG_complex ("after"     , "%8.2fd, %8.2fx, %8.2fz, %8.2fy", de, xe, ze, ye);
   /*---(calculate)----------------------*/
   rc = ykine_accel_calc (xb, zb, yb, xe, ze, ye, a_dur [1], a_dur [0], a_dur [2]);
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
      ykine_exact_calc ('d', db, de, x_pct, &d);
      DEBUG_YKINE_MOVE   yLOG_complex ("degrees"   , "%8.2fb, %8.2fe, %8.2f", db, de, d);
      ykine_exact_calc ('-', yb, ye, x_pct, &y);
      if (myKINE.s_style == YKINE_POLAR) {
         DEBUG_YKINE_MOVE   yLOG_note    ("polar verb");
         ykine_exact_calc_length (xb, xe, zb, ze, x_pct, &l);
         ykine_exact_calc_polar  (l, d, &x, &z);
      } else {
         DEBUG_YKINE_MOVE   yLOG_note    ("normal verb");
         ykine_exact_calc ('-', xb, xe, x_pct, &x);
         ykine_exact_calc ('-', zb, ze, x_pct, &z);
      }
      DEBUG_YKINE_MOVE   yLOG_complex ("position"  , "%8.3fd, %8.2fx, %8.2fz, %8.2fy", d, x, z, y);
      /*---(inverse on small move)-------*/
      rc = yKINE_inverse (a_leg, x, z, y - 6.3);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rce;
      }
      /*---(retrieve angles)-------------*/
      yKINE_angles   (a_leg, YKINE_IK, NULL, &f, &p, &t);
      DEBUG_YKINE_MOVE   yLOG_complex ("angles"    , "%8.3ff, %8.3fp, %8.3ft", f, p, t);
      /*---(create moves)----------------*/
      rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo - 0, myKINE.s_verb, myKINE.s_cline, t, g_accel_info [i].dur * myKINE.s_pace);
      rc = ykine_move_addloc (x_servo, x, z, y);
      rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo - 1, myKINE.s_verb, myKINE.s_cline, p, g_accel_info [i].dur * myKINE.s_pace);
      rc = yKINE_endpoint    (a_leg, YKINE_PATE, YKINE_IK, NULL, NULL, &x, &z, &y);
      rc = ykine_move_addloc (x_servo - 1, x, z, y);
      rc = ykine_move_create (YKINE_MOVE_SERVO, x_servo - 2, myKINE.s_verb, myKINE.s_cline, f, g_accel_info [i].dur * myKINE.s_pace);
      rc = yKINE_endpoint    (a_leg, YKINE_FEMU, YKINE_IK, NULL, NULL, &x, &z, &y);
      rc = ykine_move_addloc (x_servo - 2, x, z, y);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_accel       (char *a_question, int a_num)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        t           [LEN_LABEL];
   char        x_msg       [LEN_STR  ];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "MOVE unit      : question not understood", LEN_STR);
   if      (strcmp (a_question, "dist"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         if (g_accel_info [i].dist == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dist);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL dist     :%s", x_msg);
   }
   else if (strcmp (a_question, "durs"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         if (g_accel_info [i].dur  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dur);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL durs     :%s", x_msg);
   }
   else if (strcmp (a_question, "cums"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         if (g_accel_info [i].pct  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);
         else                          sprintf (t, " %4.2f", g_accel_info [i].pct);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL cums     :%s", x_msg);
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}





