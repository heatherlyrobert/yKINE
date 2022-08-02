/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"

/*===[[ DESIGN NOTES ]]=======================================================*/

/*  accel takes the distance and calculates a acceleration/deceleration table
 *  to smooth then travel over the length.  besides distance, three factors
 *  effect the final table -- accel type, max speed, and decel type.
 *
 *  eventually this will evolve to wonderous acceleration curves and perfect
 *  harmony with physics, but for now this version is quite adequate and
 *  testable.  it is also more in-line with periodic servo instructions rather
 *  than continuous updates.  do the best with the current technology.
 *
 *  there are four parts to a acceleration string...
 *
 *     1) accel/start
 *        [         start at zero
 *        <         start at, and assume already at or close to, max speed
 *        tsmfx     start at specific speed, assume existing speed or close
 *
 *     2) exact limit
 *        6.1f      float or integer value in beats (usually 1/10 sec)
 *        full accel, travel, and decel will be scaled to fit exactly
 *        *         means same duration as last action
 *
 *     3) max speed allowed
 *        speeds used up to and including this level, no higher
 *        normal        t/turtle, s/slow, m/moderate, f/fast, x/extra fast
 *        double time   T/turtle, S/slow, M/moderate, F/fast, X/extra fast
 *
 *     4) decel/finish
 *        ]         end at zero
 *        >         finsh at, and assume next action starts close to, max speed
 *        tsmfx     end at specific speed, assume next action can use this
 *
 *
 *  examples...
 *     [m]    = tmt
 *     <m]    = mmt
 *     [m>    = tmm
 *     [ms    = tms
 *     mxf    = mxf
 *     [15f]  = tft fit to 15 beats
 *     m*xs   = mxs fit to the last exact (15 in this case)
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



tACCEL g_accel_info [MAX_ACCEL] = {
   { 't', "turtle"     ,  2.0 },
   { 's', "slow"       ,  4.0 },
   { 'm', "moderate"   ,  8.0 },
   { 'f', "fast"       , 12.0 },  /* 12, 14, or 16 ? */
   { 'x', "extra fast" , 16.0 },  /* 16, 25, or 32 ? */
   { 'f', "fast"       , 12.0 },  /* 12, 14, or 16 ? */
   { 'm', "moderate"   ,  8.0 },
   { 's', "slow"       ,  4.0 },
   { 't', "turtle"     ,  2.0 },
   { '/', "noop"       ,  0.0 },
};



tENDS    g_accel_ends [MAX_PARTS];



tTIMING  g_timing;



/*---(valid values)-------------------*/
static char    v_accels   [LEN_LABEL] = "tsmfx<[";
static char    v_decels   [LEN_LABEL] = "tsmfx>]";
static char    v_speeds   [LEN_LABEL] = "tsmfx/";





/*====================------------------------------------====================*/
/*===----                       clearing/wiping                        ----===*/
/*====================------------------------------------====================*/
static void      o___CLEARING________________o (void) {;};

char         /*--> clear the acceleration table ----------[ ------ [ ------ ]-*/
ykine_accel__wipe        (char a_part)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i;
   /*---(clear)--------------------------*/
   for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
      /*---(info)--------------*/
      g_accel_info [i].dist [a_part]  = 0.0;
      g_accel_info [i].dur  [a_part]  = 0.0;
      g_accel_info [i].pct  [a_part]  = 0.0;
      g_accel_info [i].cum  [a_part]  = 0.0;
      g_accel_info [i].adj  [a_part]  = 0.0;
      /*---(ends)--------------*/
      g_accel_ends [a_part].verb = -1;
      g_accel_ends [a_part].part = '-';
      g_accel_ends [a_part].sb = g_accel_ends [a_part].se = 0.0;
      g_accel_ends [a_part].xb = g_accel_ends [a_part].xe = 0.0;
      g_accel_ends [a_part].zb = g_accel_ends [a_part].ze = 0.0;
      g_accel_ends [a_part].yb = g_accel_ends [a_part].ye = 0.0;
      g_accel_ends [a_part].xzlen  = 0.0;
      g_accel_ends [a_part].len    = 0.0;
      g_accel_ends [a_part].fb = g_accel_ends [a_part].fe = 0.0;
      g_accel_ends [a_part].pb = g_accel_ends [a_part].pe = 0.0;
      g_accel_ends [a_part].tb = g_accel_ends [a_part].te = 0.0;
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> clear the acceleration table ----------[ ------ [ ------ ]-*/
ykine_accel__clear       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i;
   /*---(clear)--------------------------*/
   for (i = 0; i < MAX_PARTS; ++i) {
      ykine_accel__wipe (i);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_accel__defaults   (void)
{
   g_timing.accel  = '-';
   g_timing.acceln = 0;
   g_timing.decel  = '-';
   g_timing.deceln = 0;
   g_timing.speed  = '-';
   g_timing.speedn = 0;
   g_timing.exact  = 0.0;
   g_timing.step   = 1.0;
   return 0;
}

char
ykine_accel_reset       (char a_leg)
{
   ykine_accel__clear ();
   g_timing.cpart = 0;
   myKINE.leg = a_leg;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      allocation to levels                    ----===*/
/*====================------------------------------------====================*/
static void      o___ALLOCATION______________o (void) {;};

char         /*--> calculate acceleration steps ----------[ ------ [ ------ ]-*/
ykine_accel__level  (char a_part, char a_max, char a_level, char a_accel, char a_decel, float a_step, float *a_rem)
{  /*---(design notes)-------------------*/
   /* contains no globals to aid with isolation and unit testing
   */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       x_persec    =  0.0;
   float       a           =  0.0;
   char        x_lvl       =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE   yLOG_complex ("a_part"    , "p %d, m %d, l %d, a %d, d %d, s %3.1f", a_part, a_max, a_level, a_accel, a_decel, a_step);
   --rce;  if (a_part  < 0 || a_part  >= MAX_PARTS) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_max   < 0 || a_max   > 4) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_level < 0 || a_level > a_max) {
      DEBUG_YKINE   yLOG_note    ("level beyond max requested");
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_lvl    = v_speeds [a_level];
   DEBUG_YKINE   yLOG_char    ("x_lvl"     , x_lvl);
   DEBUG_YKINE   yLOG_point   ("a_rem"     , a_rem);
   --rce;  if (a_rem == NULL) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (*a_rem <  0.1) {
      *a_rem = 0.0;
      DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_YKINE   yLOG_double  ("*a_rem"    , *a_rem);
   /*---(prepare)------------------------*/
   x_persec = g_accel_info [a_level].persec;
   /*---(max level)----------------------*/
   if (a_max == a_level && *a_rem >= 0.1) {
      DEBUG_YKINE   yLOG_note    ("max, so all at this level");
      g_accel_info [0 + a_level].dist [a_part] += *a_rem;
      *a_rem = 0.0;
   }
   /*---(last needed level)--------------*/
   if (*a_rem > 0.1 && *a_rem <= 2.0 * x_persec) {
      DEBUG_YKINE   yLOG_note    ("all will fit (2x) at this level");
      g_accel_info [0 + a_level].dist [a_part] += *a_rem;
      *a_rem = 0.0;
   }
   /*---(acceleration)-------------------*/
   if (a_accel <= a_level && *a_rem >= 0.1 && a_level < ACCEL_EXTRA) {
      DEBUG_YKINE   yLOG_double  ("accel was" , g_accel_info [0 + a_level].dist [a_part]);
      if (*a_rem < x_persec)   *a_rem -= g_accel_info [0 + a_level].dist [a_part] = *a_rem;
      else                     *a_rem -= g_accel_info [0 + a_level].dist [a_part] = x_persec;
      DEBUG_YKINE   yLOG_double  ("accel now" , g_accel_info [0 + a_level].dist [a_part]);
   }
   /*---(deceleration)-------------------*/
   if (a_decel <= a_level && *a_rem >= 0.1 && a_level < ACCEL_EXTRA) {
      DEBUG_YKINE   yLOG_double  ("decel was" , g_accel_info [8 - a_level].dist [a_part]);
      if (*a_rem < x_persec)   *a_rem -= g_accel_info [8 - a_level].dist [a_part] = *a_rem;
      else                     *a_rem -= g_accel_info [8 - a_level].dist [a_part] = x_persec;
      DEBUG_YKINE   yLOG_double  ("decel now" , g_accel_info [8 - a_level].dist [a_part]);
   }
   /*---(wrap up)------------------------*/
   if (*a_rem < 0.1)  *a_rem = 0.0;
   if (g_accel_info [0 + a_level].dist [a_part] > 0.0) {
      DEBUG_YKINE   yLOG_note    ("set accel duration");
      a = (g_accel_info [0 + a_level].dist [a_part] / x_persec) * a_step;
      g_accel_info [0 + a_level].dur [a_part] = round (a * 10) / 10;
      if (g_accel_info [0 + a_level].dur [a_part] < 0.1) g_accel_info [0 + a_level].dur [a_part] = 0.1;
   }
   if (g_accel_info [8 - a_level].dist [a_part] > 0.0) {
      DEBUG_YKINE   yLOG_note    ("set decel duration");
      a = (g_accel_info [8 - a_level].dist [a_part] / x_persec) * a_step;
      g_accel_info [8 - a_level].dur [a_part] = round (a * 10) / 10;
      if (g_accel_info [8 - a_level].dur [a_part] < 0.1) g_accel_info [8 - a_level].dur [a_part] = 0.1;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> partition move into table -------------[ ------ [ ------ ]-*/
ykine_accel__alloc      (char a_part, float a_dist)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i;
   /*---(allocate)-----------------------*/
   ykine_accel__wipe  (a_part);
   for (i = 0; i <= 4; ++i) {
      ykine_accel__level (a_part, g_timing.speedn, i, g_timing.acceln, g_timing.deceln, g_timing.step, &a_dist);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      adjusting to exact                      ----===*/
/*====================------------------------------------====================*/
static void      o___ADJUSTING_______________o (void) {;};

char         /*--> determine total distance --------------[ ------ [ ------ ]-*/
ykine_accel__dist       (float *a_dist, float *a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i, j;
   float       x_dist      =  0.0;
   float       x_dur       =  0.0;
   /*---(sum)----------------------------*/
   for (j = 0; j < MAX_PARTS; ++j) {
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         x_dist  += g_accel_info [i].dist [j];
         x_dur   += g_accel_info [i].dur  [j];
      }
   }
   if (a_dist != NULL)  *a_dist = x_dist;
   if (a_dur  != NULL)  *a_dur  = x_dur;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> calculate all percents ----------------[ ------ [ ------ ]-*/
ykine_accel__pcts       (float a_dist)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i, j;
   float       x_run       =  0.0;
   float       x_cum       =  0.0;
   float       x_part      =  0.0;
   /*---(defense)------------------------*/
   if (a_dist <= 0.0) {
      return 0;
   }
   /*---(calculate)----------------------*/
   for (j = 0; j < MAX_PARTS; ++j) {
      x_part = 0.0;
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         x_part  += g_accel_info [i].dist [j];
      }
      if (x_part == 0.0)  continue;
      x_run = 0.0;
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].dist [j] == 0.0)  continue;
         g_accel_info [i].pct [j] = x_run += g_accel_info [i].dist [j] / x_part;
         g_accel_info [i].cum [j] = x_cum += g_accel_info [i].dist [j] / a_dist;
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

int          /*--> calculate all percents ----------------[ ------ [ ------ ]-*/
ykine_accel__adjust     (float a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       x_scale;
   int         i, j;
   /*---(not exact)---------*/
   if (g_timing.exact <= 0.00)  {
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         for (j = 0; j < MAX_PARTS; ++j) {
            g_accel_info [i].adj [j] = g_accel_info [i].dur [j];
         }
      }
   }
   /*---(scale)-------------*/
   else if (a_dur >= 0.10) {
      x_scale = g_timing.exact / a_dur;
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         for (j = 0; j < MAX_PARTS; ++j) {
            if (g_accel_info [i].dur  [j] == 0.0)  continue;
            g_accel_info [i].adj [j] = g_accel_info [i].dur [j] * x_scale;
            DEBUG_YKINE   yLOG_complex ("level"     , "%d %c %5.1fd %5.1fs %4.2fp %5.1fa", j, g_accel_info [i].abbr, g_accel_info [i].dist [j], g_accel_info [i].dur [j], g_accel_info [i].pct [j], g_accel_info [i].adj [j]);
         }
      }
   }
   /*---(fill)--------------*/
   else {
      g_accel_info [DECEL_NOOP].dist [0] = 0.00;
      g_accel_info [DECEL_NOOP].dur  [0] = g_timing.exact;
      g_accel_info [DECEL_NOOP].pct  [0] = 1.00;
      g_accel_info [DECEL_NOOP].adj  [0] = g_timing.exact;
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      support execution                       ----===*/
/*====================------------------------------------====================*/
static void      o___EXECUTION_______________o (void) {;};

char
ykine_accel__save       (char a_verb, char a_part, char a_action)
{
   g_accel_ends [a_part].verb  = a_verb;
   g_accel_ends [a_part].part  = a_action;
   g_accel_ends [a_part].sb    = g_beg.sec;
   g_accel_ends [a_part].se    = g_end.sec;
   g_accel_ends [a_part].xb    = g_beg.ex ;
   g_accel_ends [a_part].xe    = g_end.ex ;
   g_accel_ends [a_part].zb    = g_beg.ez ;
   g_accel_ends [a_part].ze    = g_end.ez ;
   g_accel_ends [a_part].yb    = g_beg.ey ;
   g_accel_ends [a_part].ye    = g_end.ey ;
   g_accel_ends [a_part].xzlen = myKINE.lxz;
   g_accel_ends [a_part].len   = g_end.len;
   g_accel_ends [a_part].fb    = g_beg.fd ;
   g_accel_ends [a_part].fe    = g_end.fd ;
   g_accel_ends [a_part].pb    = g_beg.pd ;
   g_accel_ends [a_part].pe    = g_end.pd ;
   g_accel_ends [a_part].tb    = g_beg.td ;
   g_accel_ends [a_part].te    = g_end.td ;
   return 0;
}

char
ykine_accel__restore    (char a_part)
{
   /*> myKINE.verb = g_accel_ends [a_part].verb;                                      <*/
   g_beg.sec   = g_accel_ends [a_part].sb;
   g_end.sec   = g_accel_ends [a_part].se;
   g_beg.ex    = g_accel_ends [a_part].xb;
   g_end.ex    = g_accel_ends [a_part].xe;
   g_beg.ez    = g_accel_ends [a_part].zb;
   g_end.ez    = g_accel_ends [a_part].ze;
   g_beg.ey    = g_accel_ends [a_part].yb;
   g_end.ey    = g_accel_ends [a_part].ye;
   myKINE.lxz  = g_accel_ends [a_part].xzlen;
   g_end.len   = g_accel_ends [a_part].len;
   g_beg.fd    = g_accel_ends [a_part].fb;
   g_end.fd    = g_accel_ends [a_part].fe;
   g_beg.pd    = g_accel_ends [a_part].pb;
   g_end.pd    = g_accel_ends [a_part].pe;
   g_beg.td    = g_accel_ends [a_part].tb;
   g_end.td    = g_accel_ends [a_part].te;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      mini-driver functions                   ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVERS_________________o (void) {;};

char         /*--> manage acceleration process -----------[ ------ [ ------ ]-*/
ykine_accel__calc       (char a_part, char a_verb, char a_action)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_dist, x_dur, x_cum;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(init)---------------------------*/
   if (a_part == 0)  ykine_accel__clear ();
   /*---(handle)-------------------------*/
   ykine_exact_dist_route (a_verb);
   ykine_accel__alloc  (a_part, g_end.len);
   ykine_accel__dist   (&x_dist, &x_dur);
   ykine_accel__pcts   (x_dist);
   ykine_accel__adjust (x_dur);
   ykine_accel__save   (a_verb, a_part, a_action);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel__dur        (cchar *a_dur)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char        t           [LEN_LABEL];
   int         i           =    0;
   char       *p           = NULL;
   float       x_exact     =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   x_exact = g_timing.exact;
   ykine_accel__defaults ();
   /*---(defense)------------------------*/
   DEBUG_YKINE   yLOG_point   ("a_dur"     , a_dur);
   --rce;  if (a_dur   == NULL) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(filter)-------------------------*/
   x_len = strlen (a_dur);
   DEBUG_YKINE   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len < 3) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(pace)---------------------------*/
   g_timing.speed = tolower (a_dur [x_len - 2]);
   DEBUG_YKINE   yLOG_char    ("speed"     , g_timing.speed);
   p = strchr (v_speeds, g_timing.speed);
   --rce;  if (p == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   g_timing.speedn = p - v_speeds;
   DEBUG_YKINE   yLOG_value   ("a_speedn"  , g_timing.speedn);
   if (g_timing.speed != a_dur [x_len - 2])  g_timing.step = 0.5;
   DEBUG_YKINE   yLOG_double  ("step"      , g_timing.step);
   /*---(accel)--------------------------*/
   g_timing.accel = a_dur [0];
   DEBUG_YKINE   yLOG_char    ("accel"     , g_timing.accel);
   p = strchr (v_accels, g_timing.accel);
   --rce;  if (p == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (g_timing.accel) {
   case '[' : g_timing.acceln = 0;                 break;
   case '<' : g_timing.acceln = g_timing.speedn;          break;
   default  : g_timing.acceln = p - v_accels;
              if (g_timing.acceln > g_timing.speedn)  g_timing.acceln = g_timing.speedn;
              break;
   }
   DEBUG_YKINE   yLOG_value   ("a_acceln"  , g_timing.acceln);
   /*---(decel)--------------------------*/
   g_timing.decel = a_dur [x_len - 1];
   DEBUG_YKINE   yLOG_char    ("decel"     , g_timing.decel);
   p = strchr (v_decels, g_timing.decel);
   --rce;  if (p == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (g_timing.decel) {
   case ']' : g_timing.deceln = 0;                 break;
   case '>' : g_timing.deceln = g_timing.speedn;          break;
   default  : g_timing.deceln = p - v_decels;
              if (g_timing.deceln > g_timing.speedn)  g_timing.deceln = g_timing.speedn;
              break;
   }
   DEBUG_YKINE   yLOG_value   ("a_deceln"  , g_timing.deceln);
   /*---(exact timing)-------------------*/
   if (x_len > 3) {
      DEBUG_YKINE   yLOG_note    ("exact version");
      /*---(same as last)------*/
      if (x_len == 4 && a_dur [1] == '*') {
         g_timing.exact = x_exact;
      }
      /*---(new exact)---------*/
      else {
         sprintf (t, "%*.*s", x_len - 3, x_len - 3, a_dur + 1);
         /*> strncpy (t, a_dur + 1, x_len - 3);                                          <*/
         DEBUG_YKINE   yLOG_info    ("t"         , t);
         /*> printf ("a_dur %s, t %s\n", a_dur, t);                                      <*/
         x_len = strlen (t);
         DEBUG_YKINE   yLOG_value   ("x_len"     , x_len);
         --rce;  for (i = 0; i < x_len; ++i) {
            if (strchr ("0123456789.", t [i]) != NULL)  continue;
            ykine_accel__defaults ();
            DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         g_timing.exact = atof (t);
         /*---(check)-------------*/
         DEBUG_YKINE   yLOG_double  ("exact"     , g_timing.exact);
         --rce;  if (g_timing.exact < 0.1) {
            ykine_accel__defaults ();
            DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      }
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_append      (char a_verb, char a_part, char *a_accel)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i, j;
   float       x_pct       =  0.0;
   float       x_adj       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   rc = ykine_accel__dur  (a_accel);
   rc = ykine_accel__calc (g_timing.cpart, a_verb, a_part);
   --rce;  if (rc <  0) {
      DEBUG_YKINE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare for next)---------------*/
   ++g_timing.cpart;
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      parsing support                         ----===*/
/*====================------------------------------------====================*/
static void      o___PARSING_________________o (void) {;};

char
ykine_accel__timing_save (void)
{
   DEBUG_YKINE   yLOG_senter  (__FUNCTION__);
   g_timing.a_acceln = g_timing.acceln;
   g_timing.a_deceln = g_timing.deceln;
   g_timing.a_speedn = g_timing.speedn;
   g_timing.a_exact  = g_timing.exact;
   DEBUG_YKINE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_accel_timing      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_enter   (__FUNCTION__);
   /*---(check accel)-----------------*/
   yPARSE_top      (g_timing.request);
   DEBUG_YKINE   yLOG_info    ("accel"     , g_timing.request);
   rc  = ykine_accel__dur (g_timing.request);
   DEBUG_YKINE   yLOG_value   ("accel_dur" , rc);
   /*---(check normal)----------------*/
   yPARSE_popval (0.0, &g_timing.beats);
   DEBUG_YKINE   yLOG_value   ("b"         , g_timing.beats);
   /*---(if normal)-------------------*/
   if (rc < 0)   strlcpy (g_timing.request, "", LEN_LABEL);
   /*---(if accelerated)--------------*/
   else  {
      g_timing.beats = -1.0;
      ykine_accel__timing_save ();
   }
   DEBUG_YKINE   yLOG_info    ("accel'"    , g_timing.request);
   DEBUG_YKINE   yLOG_value   ("b'"        , g_timing.beats);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      support functions                       ----===*/
/*====================------------------------------------====================*/
static void      o___SUPPORT_________________o (void) {;};

char
ykine_accel_make        (char a_acceln, float a_exact, char a_speedn, char a_deceln, char *a_out)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_exact     [LEN_LABEL] = "";
   char        x_accel, x_speed, x_decel;
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE   yLOG_spoint  (a_out);
   --rce;  if (a_out == NULL) {
      DEBUG_YKINE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (a_out, "", LEN_LABEL);
   DEBUG_YKINE   yLOG_sint    (a_acceln);
   --rce;  if (a_acceln < 0 || a_acceln > 4) {
      DEBUG_YKINE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE   yLOG_sint    (a_speedn);
   --rce;  if (a_speedn < 0 || a_speedn > 4) {
      DEBUG_YKINE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE   yLOG_sint    (a_deceln);
   --rce;  if (a_deceln < 0 || a_deceln > 4) {
      DEBUG_YKINE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE   yLOG_sdouble (a_exact);
   --rce;  if (a_exact  < 0.0) {
      DEBUG_YKINE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(limits)-------------------------*/
   if      (a_acceln >  a_speedn)  a_acceln = a_speedn;
   if      (a_deceln >  a_speedn)  a_deceln = a_speedn;
   /*---(format)-------------------------*/
   x_speed = v_speeds [a_speedn];
   if      (a_acceln == 0)         x_accel  = '[';
   else if (a_acceln == a_speedn)  x_accel  = '<';
   else                            x_accel  = v_accels [a_acceln];
   if      (a_deceln == 0)         x_decel  = ']';
   else if (a_deceln == a_speedn)  x_decel  = '>';
   else                            x_decel  = v_decels [a_deceln];
   if (a_exact >= 0.10) {
      if (round (a_exact) == a_exact)    sprintf (x_exact, "%.0f", roundf (a_exact));
      else                               sprintf (x_exact, "%.1f", a_exact);
   }
   sprintf (a_out, "%c%s%c%c", x_accel, x_exact, x_speed, x_decel);
   DEBUG_YKINE   yLOG_snote   (a_out);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_accel_body_adj    (float a_pct, char *a_out)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_exact     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE   yLOG_sdouble (a_pct);
   DEBUG_YKINE   yLOG_spoint  (a_out);
   /*---(defense)------------------------*/
   --rce;  if (a_out == NULL) {
      DEBUG_YKINE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   myKINE.step_total = 0.0;
   if (g_timing.exact <  0.1) {
      DEBUG_YKINE   yLOG_snote   ("no exact");
      sprintf (a_out, "%c%c%c", g_timing.accel, g_timing.speed, g_timing.decel);
   } else if (a_pct   <= 0.0) {
      DEBUG_YKINE   yLOG_snote   ("no pecent");
      myKINE.step_total = g_timing.exact;
      sprintf (a_out, "%c%.1f%c%c", g_timing.accel, g_timing.exact, g_timing.speed, g_timing.decel);
   } else {
      DEBUG_YKINE   yLOG_snote   ("exact and percent");
      myKINE.step_total = g_timing.exact * a_pct;
      sprintf (x_exact, "%.1f", g_timing.exact * a_pct);
      sprintf (a_out, "%c%s%c%c", g_timing.accel, x_exact, g_timing.speed, g_timing.decel);
   }
   DEBUG_YKINE   yLOG_snote   (a_out);
   /*---(complete)-----------------------*/
   DEBUG_YKINE   yLOG_sexit   (__FUNCTION__);
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
   char        x_msg       [LEN_RECD ];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "MOVE unit      : question not understood", LEN_RECD);
   if      (strcmp (a_question, "dist"      ) == 0) {
      strlcpy (x_msg, "", LEN_RECD);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].dist [a_num] == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dist [a_num]);
         strlcat  (x_msg, t, LEN_RECD);
      }
      sprintf (ykine__unit_answer, "ACCEL dist (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "globals"   ) == 0) {
      sprintf (ykine__unit_answer, "ACCEL globals    : %6.1fb  [%s]", g_timing.beats, g_timing.request);
   }
   else if (strcmp (a_question, "durs"      ) == 0) {
      strlcpy (x_msg, "", LEN_RECD);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].dur [a_num]  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dur [a_num]);
         strlcat  (x_msg, t, LEN_RECD);
      }
      sprintf (ykine__unit_answer, "ACCEL durs (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "adjs"      ) == 0) {
      strlcpy (x_msg, "", LEN_RECD);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].adj [a_num]  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].adj [a_num]);
         strlcat  (x_msg, t, LEN_RECD);
      }
      sprintf (ykine__unit_answer, "ACCEL adjs (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "pcts"      ) == 0) {
      strlcpy (x_msg, "", LEN_RECD);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].pct [a_num]  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);
         else                          sprintf (t, " %4.2f", g_accel_info [i].pct [a_num]);
         strlcat  (x_msg, t, LEN_RECD);
      }
      sprintf (ykine__unit_answer, "ACCEL pcts (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "cums"      ) == 0) {
      strlcpy (x_msg, "", LEN_RECD);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].cum [a_num]  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);
         else                          sprintf (t, " %4.2f", g_accel_info [i].cum [a_num]);
         strlcat  (x_msg, t, LEN_RECD);
      }
      sprintf (ykine__unit_answer, "ACCEL cums (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "parse"     ) == 0) {
      sprintf (ykine__unit_answer, "ACCEL parse      : %c%1d   %c%1d   %c%1d %6.1f %6.1f", g_timing.accel, g_timing.acceln, g_timing.decel, g_timing.deceln, g_timing.speed, g_timing.speedn, g_timing.exact, g_timing.step);
   }
   else if (strcmp (a_question, "saved_b"   ) == 0) {
      sprintf (ykine__unit_answer, "ACCEL beg  (%d) : %2d %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f", a_num, g_accel_ends [a_num].verb, g_accel_ends [a_num].sb, g_accel_ends [a_num].xb, g_accel_ends [a_num].zb, g_accel_ends [a_num].yb, g_accel_ends [a_num].fb, g_accel_ends [a_num].pb, g_accel_ends [a_num].tb);
   }
   else if (strcmp (a_question, "saved_e"   ) == 0) {
      sprintf (ykine__unit_answer, "ACCEL end  (%d) : %2d %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f", a_num, g_accel_ends [a_num].verb, g_accel_ends [a_num].se, g_accel_ends [a_num].xe, g_accel_ends [a_num].ze, g_accel_ends [a_num].ye, g_accel_ends [a_num].fe, g_accel_ends [a_num].pe, g_accel_ends [a_num].te);
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}





