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
 *  [ and ] mean turtle
 *  < and > mean same as middle
 *
 *  [m]  = tmt
 *  <m]  = mmt
 *  [m>  = tmm
 *  [ms  = tms
 *  mxf
 *
 *  [15f]  = tft fit to 15
 *  m*xs   = mxs fit to the last exact (15)
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

tENDS  g_accel_ends [MAX_PARTS];



static char    s_accel    = '-';
static char    s_decel    = '-';
static char    s_speed    = '-';
static float   s_step     =  1.0;

static char    s_acceln   =   0;
static char    s_deceln   =   0;
static char    s_speedn   =   0;
static float   s_exact    = 0.0;

static char    s_accels   [LEN_LABEL] = "tsmfx<[";
static char    s_decels   [LEN_LABEL] = "tsmfx>]";
static char    s_speeds   [LEN_LABEL] = "tsmfx";

static int     s_cpart    =  0;


/*====================------------------------------------====================*/
/*===----                        calculations                          ----===*/
/*====================------------------------------------====================*/
static void      o___CALCULATION_____________o (void) {;};

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
      g_accel_ends [a_part].sb = g_accel_ends [a_part].se = 0.0;
      g_accel_ends [a_part].xb = g_accel_ends [a_part].xe = 0.0;
      g_accel_ends [a_part].zb = g_accel_ends [a_part].ze = 0.0;
      g_accel_ends [a_part].yb = g_accel_ends [a_part].ye = 0.0;
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
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_complex ("a_part"    , "p %d, m %d, l %d, a %d, d %d, s %3.1f", a_part, a_max, a_level, a_accel, a_decel, a_step);
   --rce;  if (a_part  < 0 || a_part  >= MAX_PARTS) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_max   < 0 || a_max   > 4) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_level < 0 || a_level > a_max) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_lvl    = s_speeds [a_level];
   DEBUG_YKINE_MOVE   yLOG_char    ("x_lvl"     , x_lvl);
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
   DEBUG_YKINE_MOVE   yLOG_double  ("*a_rem"    , *a_rem);
   /*---(prepare)------------------------*/
   x_persec = g_accel_info [a_level].persec;
   /*---(max level)----------------------*/
   if (a_max == a_level && *a_rem >= 0.1) {
      DEBUG_YKINE_MOVE   yLOG_note    ("max, so all at this level");
      g_accel_info [0 + a_level].dist [a_part] += *a_rem;
      *a_rem = 0.0;
   }
   /*---(last needed level)--------------*/
   if (*a_rem > 0.1 && *a_rem <= 2.0 * x_persec) {
      DEBUG_YKINE_MOVE   yLOG_note    ("all will fit (2x) at this level");
      g_accel_info [0 + a_level].dist [a_part] += *a_rem;
      *a_rem = 0.0;
   }
   /*---(acceleration)-------------------*/
   if (a_accel <= a_level && *a_rem >= 0.1 && a_level < ACCEL_EXTRA) {
      DEBUG_YKINE_MOVE   yLOG_double  ("accel was" , g_accel_info [0 + a_level].dist [a_part]);
      if (*a_rem < x_persec)   *a_rem -= g_accel_info [0 + a_level].dist [a_part] = *a_rem;
      else                     *a_rem -= g_accel_info [0 + a_level].dist [a_part] = x_persec;
      DEBUG_YKINE_MOVE   yLOG_double  ("accel now" , g_accel_info [0 + a_level].dist [a_part]);
   }
   /*---(deceleration)-------------------*/
   if (a_decel <= a_level && *a_rem >= 0.1 && a_level < ACCEL_EXTRA) {
      DEBUG_YKINE_MOVE   yLOG_double  ("decel was" , g_accel_info [8 - a_level].dist [a_part]);
      if (*a_rem < x_persec)   *a_rem -= g_accel_info [8 - a_level].dist [a_part] = *a_rem;
      else                     *a_rem -= g_accel_info [8 - a_level].dist [a_part] = x_persec;
      DEBUG_YKINE_MOVE   yLOG_double  ("decel now" , g_accel_info [8 - a_level].dist [a_part]);
   }
   /*---(wrap up)------------------------*/
   if (*a_rem < 0.1)  *a_rem = 0.0;
   if (g_accel_info [0 + a_level].dist [a_part] > 0.0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("set accel duration");
      a = (g_accel_info [0 + a_level].dist [a_part] / x_persec) * a_step;
      g_accel_info [0 + a_level].dur [a_part] = round (a * 10) / 10;
      if (g_accel_info [0 + a_level].dur [a_part] < 0.1) g_accel_info [0 + a_level].dur [a_part] = 0.1;
   }
   if (g_accel_info [8 - a_level].dist [a_part] > 0.0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("set decel duration");
      a = (g_accel_info [8 - a_level].dist [a_part] / x_persec) * a_step;
      g_accel_info [8 - a_level].dur [a_part] = round (a * 10) / 10;
      if (g_accel_info [8 - a_level].dur [a_part] < 0.1) g_accel_info [8 - a_level].dur [a_part] = 0.1;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
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
      ykine_accel__level (a_part, s_speedn, i, s_acceln, s_deceln, s_step, &a_dist);
   }
   /*---(complete)-----------------------*/
   return 0;
}

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
   if (s_exact <= 0.00)  {
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         for (j = 0; j < MAX_PARTS; ++j) {
            g_accel_info [i].adj [j] = g_accel_info [i].dur [j];
         }
      }
   }
   /*---(scale)-------------*/
   else if (a_dur >= 0.10) {
      x_scale = s_exact / a_dur;
      for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {
         for (j = 0; j < MAX_PARTS; ++j) {
            if (g_accel_info [i].dur  [j] == 0.0)  continue;
            g_accel_info [i].adj [j] = g_accel_info [i].dur [j] * x_scale;
            DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%d %c %5.1fd %5.1fs %4.2fp %5.1fa", j, g_accel_info [i].abbr, g_accel_info [i].dist [j], g_accel_info [i].dur [j], g_accel_info [i].pct [j], g_accel_info [i].adj [j]);
         }
      }
   }
   /*---(fill)--------------*/
   else {
      g_accel_info [DECEL_NOOP].dist [0] = 0.00;
      g_accel_info [DECEL_NOOP].dur  [0] = s_exact;
      g_accel_info [DECEL_NOOP].pct  [0] = 1.00;
      g_accel_info [DECEL_NOOP].adj  [0] = s_exact;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_accel__save       (char a_verb, char a_part)
{
   g_accel_ends [a_part].verb = a_verb;
   g_accel_ends [a_part].sb   = myKINE.sb;
   g_accel_ends [a_part].se   = myKINE.se;
   g_accel_ends [a_part].xb   = myKINE.xb;
   g_accel_ends [a_part].xe   = myKINE.xe;
   g_accel_ends [a_part].zb   = myKINE.zb;
   g_accel_ends [a_part].ze   = myKINE.ze;
   g_accel_ends [a_part].yb   = myKINE.yb;
   g_accel_ends [a_part].ye   = myKINE.ye;
   g_accel_ends [a_part].fb   = myKINE.fb;
   g_accel_ends [a_part].fe   = myKINE.fe;
   g_accel_ends [a_part].pb   = myKINE.pb;
   g_accel_ends [a_part].pe   = myKINE.pe;
   g_accel_ends [a_part].tb   = myKINE.tb;
   g_accel_ends [a_part].te   = myKINE.te;
   return 0;
}

char
ykine_accel__restore    (char a_part)
{
   /*> myKINE.verb = g_accel_ends [a_part].verb;                                      <*/
   myKINE.sb   = g_accel_ends [a_part].sb;
   myKINE.se   = g_accel_ends [a_part].se;
   myKINE.xb   = g_accel_ends [a_part].xb;
   myKINE.xe   = g_accel_ends [a_part].xe;
   myKINE.zb   = g_accel_ends [a_part].zb;
   myKINE.ze   = g_accel_ends [a_part].ze;
   myKINE.yb   = g_accel_ends [a_part].yb;
   myKINE.ye   = g_accel_ends [a_part].ye;
   myKINE.fb   = g_accel_ends [a_part].fb;
   myKINE.fe   = g_accel_ends [a_part].fe;
   myKINE.pb   = g_accel_ends [a_part].pb;
   myKINE.pe   = g_accel_ends [a_part].pe;
   myKINE.tb   = g_accel_ends [a_part].tb;
   myKINE.te   = g_accel_ends [a_part].te;
   return 0;
}

char         /*--> manage acceleration process -----------[ ------ [ ------ ]-*/
ykine_accel_calc        (char a_part, char a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_dist, x_dur, x_cum;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(init)---------------------------*/
   if (a_part == 0)  ykine_accel__clear ();
   ykine_exact_dist_route (a_verb);
   ykine_accel__alloc  (a_part, myKINE.le);
   ykine_accel__dist   (&x_dist, &x_dur);
   ykine_accel__pcts   (x_dist);
   ykine_accel__adjust (x_dur);
   ykine_accel__save   (a_verb, a_part);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel__defaults   (void)
{
   s_accel  = '-';
   s_acceln = 0;
   s_decel  = '-';
   s_deceln = 0;
   s_speed  = '-';
   s_speedn = 0;
   s_exact  = 0.0;
   s_step   = 1.0;
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
   char       *p           = NULL;
   float       x_exact     =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   x_exact = s_exact;
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
   /*---(pace)---------------------------*/
   s_speed = tolower (a_dur [x_len - 2]);
   DEBUG_YKINE_MOVE   yLOG_char    ("s_speed"   , s_speed);
   p = strchr (s_speeds, s_speed);
   --rce;  if (p == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_speedn = p - s_speeds;
   DEBUG_YKINE_MOVE   yLOG_value   ("a_speedn"  , s_speedn);
   if (s_speed != a_dur [x_len - 2])  s_step = 0.5;
   DEBUG_YKINE_MOVE   yLOG_double  ("s_step"    , s_step);
   /*---(accel)--------------------------*/
   s_accel = a_dur [0];
   DEBUG_YKINE_MOVE   yLOG_char    ("s_accel"   , s_accel);
   p = strchr (s_accels, s_accel);
   --rce;  if (p == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (s_accel) {
   case '[' : s_acceln = 0;                 break;
   case '<' : s_acceln = s_speedn;          break;
   default  : s_acceln = p - s_accels;
              if (s_acceln > s_speedn)  s_acceln = s_speedn;
              break;
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("a_acceln"  , s_acceln);
   /*---(decel)--------------------------*/
   s_decel = a_dur [x_len - 1];
   DEBUG_YKINE_MOVE   yLOG_char    ("decel"     , s_decel);
   p = strchr (s_decels, s_decel);
   --rce;  if (p == NULL) {
      ykine_accel__defaults ();
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (s_decel) {
   case ']' : s_deceln = 0;                 break;
   case '>' : s_deceln = s_speedn;          break;
   default  : s_deceln = p - s_decels;
              if (s_deceln > s_speedn)  s_deceln = s_speedn;
              break;
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("a_deceln"  , s_deceln);
   /*---(exact timing)-------------------*/
   if (x_len > 3) {
      DEBUG_YKINE_MOVE   yLOG_note    ("exact version");
      /*---(same as last)------*/
      if (x_len == 4 && a_dur [1] == '*') {
         s_exact = x_exact;
      }
      /*---(new exact)---------*/
      else {
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
         /*---(check)-------------*/
         DEBUG_YKINE_MOVE   yLOG_double  ("a_exact"   , s_exact);
         --rce;  if (s_exact < 0.1) {
            ykine_accel__defaults ();
            DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      }
      /*---(done)--------------*/
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
   else  {
      myKINE.b = -1.0;
      myKINE.a_acceln = s_acceln;
      myKINE.a_deceln = s_deceln;
      myKINE.a_speedn = s_speedn;
      myKINE.a_exact  = s_exact;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("accel'"    , myKINE.accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("b'"        , myKINE.b);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_make        (char a_acceln, float a_exact, char a_speedn, char a_deceln, char *a_out)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_exact     [LEN_LABEL] = "";
   char        x_accel, x_speed, x_decel;
   /*---(defense)------------------------*/
   --rce;  if (a_out == NULL)                 return rce;
   strlcpy (a_out, "", LEN_LABEL);
   --rce;  if (a_acceln < 0 || a_acceln > 4)  return rce;
   --rce;  if (a_speedn < 0 || a_speedn > 4)  return rce;
   --rce;  if (a_deceln < 0 || a_deceln > 4)  return rce;
   --rce;  if (a_exact  < 0.0)                return rce;
   /*---(limits)-------------------------*/
   if      (a_acceln >  a_speedn)  a_acceln = a_speedn;
   if      (a_deceln >  a_speedn)  a_deceln = a_speedn;
   /*---(format)-------------------------*/
   x_speed = s_speeds [a_speedn];
   if      (a_acceln == 0)         x_accel  = '[';
   else if (a_acceln == a_speedn)  x_accel  = '<';
   else                            x_accel  = s_accels [a_acceln];
   if      (a_deceln == 0)         x_decel  = ']';
   else if (a_deceln == a_speedn)  x_decel  = '>';
   else                            x_decel  = s_decels [a_deceln];
   if (a_exact >= 0.10) {
      if (round (a_exact) == a_exact)    sprintf (x_exact, "%.0f", roundf (a_exact));
      else                               sprintf (x_exact, "%.1f", a_exact);
   }
   sprintf (a_out, "%c%s%c%c", x_accel, x_exact, x_speed, x_decel);
   /*---(complete)-----------------------*/
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
ykine_accel_reset       (char a_leg)
{
   ykine_accel__clear ();
   s_cpart = 0;
   myKINE.leg = a_leg;
   return 0;
}

char
ykine_accel_append      (char a_verb, char *a_accel)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i, j;
   float       x_pct       =  0.0;
   float       x_adj       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   rc = ykine_accel_dur  (a_accel);
   rc = ykine_accel_calc (s_cpart, a_verb);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare for next)---------------*/
   ++s_cpart;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
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
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   --rce;  if (myKINE.b >= 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   for (j = 0; j < MAX_PARTS; ++j) {
      ykine_accel__restore (j);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         DEBUG_YKINE_MOVE   yLOG_complex ("level"     , "%c %5.1fd %5.1fd %4.2fp", g_accel_info [i].abbr, g_accel_info [i].dist [j], g_accel_info [i].dur [j], g_accel_info [i].pct [j]);
         if (g_accel_info [i].dur [j]  < 0.1)   continue;
         x_pct  = g_accel_info [i].pct [j];
         x_adj  = g_accel_info [i].adj [j];
         x_verb = g_accel_ends [j].verb;
         rc = ykine_exact_pct_route (x_verb, x_pct);
         rc = ykine_legs_partial    (x_verb, myKINE.leg, 's');
         if (x_verb == YKINE_ZE || x_verb == YKINE_PO) {
            ykine_accel__zero   (x_verb, myKINE.xc, myKINE.zc, myKINE.yc, x_adj, a_label, g_accel_info [i].abbr);
         } else {
            ykine_accel__single (x_verb, myKINE.leg, myKINE.fc, myKINE.pc, myKINE.tc, x_adj, a_label, g_accel_info [i].abbr);
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_accel_immediate   (char a_verb, char a_leg, float a_beats, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i, j;
   float       x_pct       =  0.0;
   float       x_adj       =  0.0;
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
         if (g_accel_info [i].dist [a_num] == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dist [a_num]);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL dist (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "durs"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].dur [a_num]  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].dur [a_num]);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL durs (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "adjs"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].adj [a_num]  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);
         else                          sprintf (t, " %4.1f", g_accel_info [i].adj [a_num]);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL adjs (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "pcts"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].pct [a_num]  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);
         else                          sprintf (t, " %4.2f", g_accel_info [i].pct [a_num]);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL pcts (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "cums"      ) == 0) {
      strlcpy (x_msg, "", LEN_STR);
      for (i = ACCEL_TURTLE; i <= DECEL_NOOP; ++i) {
         if (g_accel_info [i].cum [a_num]  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);
         else                          sprintf (t, " %4.2f", g_accel_info [i].cum [a_num]);
         strlcat  (x_msg, t, LEN_STR);
      }
      sprintf (ykine__unit_answer, "ACCEL cums (%d) :%s", a_num, x_msg);
   }
   else if (strcmp (a_question, "parse"     ) == 0) {
      sprintf (ykine__unit_answer, "ACCEL parse    : %c%1d   %c%1d   %c%1d %6.1f %6.1f", s_accel, s_acceln, s_decel, s_deceln, s_speed, s_speedn, s_exact, s_step);
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





