/*============================----beg-of-source---============================*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


/*---1----- -----2----- -----3----- -----4-----  ---------comments------------*/
#define     MAX_TICK    3000
typedef     struct      cTICK       tTICK;
struct      cTICK {
   /*---(special)--------------*/
   char        used;
   char        exact;
   char       *label;
   char        cell;
   /*---(planned angles)-------*/
   float       o_femu;
   float       o_pate;
   float       o_tibi;
   /*---(planned endpoint)-----*/
   float       o_xpos;
   float       o_zpos;
   float       o_ypos;
   /*---(revised endpoint)-----*/
   float       r_xpos;
   float       r_zpos;
   float       r_ypos;
   /*---(revised angles)-------*/
   float       r_femu;
   float       r_pate;
   float       r_tibi;
   /*---(opengl endpoint)------*/
   float       g_xpos;
   float       g_zpos;
   float       g_ypos;
   /*---(success)--------------*/
   char        rc_exact;
   char        rc_pure;
   char        rc_adapt;
   /*---(done)-----------------*/
};
static tTICK  *s_ticks [YKINE_MAX_LEGS] = { NULL };
static int     s_ntick  =    0;



/*====================------------------------------------====================*/
/*===----                        simple helpers                        ----===*/
/*====================------------------------------------====================*/
static void      o___HELPERS_________________o (void) {;};

char
ykine_tick__clear       (char a_init, tTICK *a_tick)
{
   /*---(special)------------------------*/
   a_tick->used       = '-';
   a_tick->exact      = '-';
   if (a_init != 'y' && a_tick->label != NULL)   free (a_tick->label);
   a_tick->label      = NULL;
   a_tick->cell       = '-';
   /*---(planned angles)-----------------*/
   a_tick->o_femu      = 0.0;
   a_tick->o_pate      = 0.0;
   a_tick->o_tibi      = 0.0;
   /*---(planned endpoint)---------------*/
   a_tick->o_xpos      = 0.0;
   a_tick->o_zpos      = 0.0;
   a_tick->o_ypos      = 0.0;
   /*---(revised angles)-----------------*/
   a_tick->r_femu      = 0.0;
   a_tick->r_pate      = 0.0;
   a_tick->r_tibi      = 0.0;
   /*---(revised endpoint)---------------*/
   a_tick->r_xpos      = 0.0;
   a_tick->r_zpos      = 0.0;
   a_tick->r_ypos      = 0.0;
   /*---(opengl endpoint)----------------*/
   a_tick->g_xpos      = 0.0;
   a_tick->g_zpos      = 0.0;
   a_tick->g_ypos      = 0.0;
   /*---(success)------------------------*/
   a_tick->rc_exact    =   0;
   a_tick->rc_pure     =   0;
   a_tick->rc_adapt    =   0;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void      o___MEMORY__________________o (void) {;};

char
ykine_tick_init         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(clear and free)-----------------*/
   for (i = 0; i < YKINE_MAX_LEGS; ++i)   s_ticks [i] = NULL;
   s_ntick = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_tick__new         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         k           =    0;
   /*---(defense)------------------------*/
   --rce;  if (s_ticks [0] != NULL || s_ntick > 0) {
      return rce;
   }
   /*---(prepare)------------------------*/
   s_ntick = myKINE.scrp_len * 10;
   /*---(malloc and clear)---------------*/
   for (i = 0; i < YKINE_MAX_LEGS; ++i) {
      s_ticks [i] = (tTICK *)  malloc (sizeof (tTICK) * s_ntick);
      for (k = 0; k < s_ntick; ++k) {
         ykine_tick__clear ('y', (s_ticks [i]) + k);
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_tick__free        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         k           =    0;
   /*---(defense)------------------------*/
   --rce;  if (s_ticks [0] == NULL || s_ntick <= 0) {
      return rce;
   }
   /*---(clear and free)-----------------*/
   for (i = 0; i < YKINE_MAX_LEGS; ++i) {
      if (s_ticks [i] != NULL) {
         for (k = 0; k < s_ntick; ++k) {
            ykine_tick__clear ('-', (s_ticks [i]) + k);
         }
         free (s_ticks [i]);
         s_ticks [i] = NULL;
      }
   }
   /*---(globals)------------------------*/
   s_ntick = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_tick_setbody      (float a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tick      =    0;
   float       x_pos   = 0, z_pos   = 0, y_pos   = 0;
   float       x_yaw   = 0, x_pitch = 0, x_roll  = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_TICK  yLOG_enter   (__FUNCTION__);
   /*---(first pass)---------------------*/
   /*> if (myKINE.s_pass == 1) {                                                                                                                          <* 
    *>    rc = yKINE_exact_all (a_sec);                                                                                                                   <* 
    *>    /+> myKINE.vb = YKINE_ZE;                                                       <+/                                                             <* 
    *>    rc = yKINE_exact_leg (YKINE_CENTER, 0.10, NULL, NULL, NULL, NULL, NULL, NULL, &x_pos, &z_pos, &y_pos, NULL, NULL, NULL, NULL, NULL, NULL);      <* 
    *>    /+> myKINE.vb = YKINE_OR;                                                       <+/                                                             <* 
    *>    rc = yKINE_exact_leg (YKINE_BODY  , 0.10, NULL, NULL, NULL, &x_yaw, &x_pitch, &x_roll, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);   <* 
    *> }                                                                                                                                                  <* 
    *> else {                                                                                                                                             <*/
   /*---(point in time)--------*/
   x_tick  = a_sec * 10;
   DEBUG_YKINE_TICK  yLOG_complex ("tick"      , "%8.2fs, %5dt", a_sec, x_tick);
   /*---(position)-------------*/
   if (x_tick >= 0 && x_tick < s_ntick) {
      x_pos   = s_ticks [YKINE_BODY][x_tick].o_xpos;
      z_pos   = s_ticks [YKINE_BODY][x_tick].o_zpos;
      y_pos   = s_ticks [YKINE_BODY][x_tick].o_ypos;
   }
   rc = yKINE_zero   (x_pos, z_pos, y_pos);
   /*---(orientation)----------*/
   if (x_tick >= 0 && x_tick < s_ntick) {
      x_yaw   = s_ticks [YKINE_BODY][x_tick].o_femu;
      x_pitch = s_ticks [YKINE_BODY][x_tick].o_pate;
      x_roll  = s_ticks [YKINE_BODY][x_tick].o_tibi;
   }
   rc = yKINE_orient (x_yaw, x_pitch, x_roll);
   /*> }                                                                              <*/
   /*---(report-out)---------------------*/
   DEBUG_YKINE_TICK  yLOG_complex ("position"  , "%4drc, %8.2fx, %8.2fz, %8.2fy", rc, x_pos, z_pos, y_pos);
   DEBUG_YKINE_TICK  yLOG_complex ("orient"    , "%4drc, %8.2fy, %8.2fp, %8.2fr", rc, x_yaw, x_pitch, x_roll);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_TICK  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_tick_fill         (char a_leg, float a_beg, float a_end)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_beg       =    0;
   int         x_end       =    0;
   int         x_pos       =    0;
   char        x_exact     =  '-';
   char        x_label     [LEN_LABEL];
   char        x_cell      =  '-';
   float       fo, po, to, xo, yo, zo;
   float       fr, pr, tr, xr, yr, zr;
   char        x_leg       [LEN_TERSE];
   tTICK      *x_base      = NULL;
   tTICK      *x_tick      = NULL;
   char        x_pure      =    0;
   char        x_adapt     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_TICK  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   /*> x_beg = g_beg.sec * 10.0;  /+   * my.p_scale;         +/                       <* 
    *> x_end = g_end.sec * 10.0;  /+   * my.p_scale;         +/                       <*/
   x_beg = a_beg * 10;
   x_end = a_end * 10;
   DEBUG_YKINE_TICK  yLOG_complex ("span"      , "%4db to %4de", x_beg, x_end);
   /*---(set leg)------------------------*/
   switch (a_leg) {
   case YKINE_CENTER :  x_base = s_ticks [0];            break;
   case YKINE_BODY   :  x_base = s_ticks [1];            break;
   default           :  x_base = s_ticks [a_leg + 1];    break;
   }
   DEBUG_YKINE_TICK  yLOG_point   ("x_base"    , x_base);
   /*---(fill)---------------------------*/
   for (x_pos = x_beg; x_pos <= x_end; ++x_pos) {
      /*> if (x_pos > my.p_len)  break;                                               <*/
      if (x_pos < 0.0     )  break;
      rc = yKINE_exact_all (x_pos / 10.0);
      if (rc < 0)            continue;
      rc = yKINE_exact_leg (a_leg, 0.10, &x_exact, x_label, &x_cell, &x_pure, &fo, &po, &to, &xo, &zo, &yo, &x_adapt, &fr, &pr, &tr, &xr, &zr, &yr);
      x_tick = x_base + x_pos;
      DEBUG_YKINE_TICK  yLOG_complex ("feedback"  , "%4d pos, %4d rc, %p", x_pos, rc, x_tick);
      DEBUG_YKINE_TICK  yLOG_complex ("results"   , "%ce, %pl, %cc", x_exact, x_label, x_cell);
      DEBUG_YKINE_TICK  yLOG_complex ("original"  , "%6.1ff, %6.1fp, %6.1ft, %6.1fx, %6.1fz, %6.1fy", fo, po, to, xo, zo, yo);
      DEBUG_YKINE_TICK  yLOG_complex ("revised"   , "%6.1ff, %6.1fp, %6.1ft, %6.1fx, %6.1fz, %6.1fy", fr, pr, tr, xr, zr, yr);
      /*---(header)----------------*/
      x_tick->used     = 'y';
      x_tick->exact    = x_exact;
      if (x_label != NULL && x_label [0] != '-')  x_tick->label  = strdup (x_label);
      x_tick->cell     = x_cell;
      /*---(original endpoint)-----*/
      x_tick->o_xpos   = xo;
      x_tick->o_zpos   = zo;
      x_tick->o_ypos   = yo;
      /*---(original angles)-------*/
      x_tick->o_femu   = fo;
      x_tick->o_pate   = po;
      x_tick->o_tibi   = to;
      /*---(revised angles)--------*/
      x_tick->r_femu   = fr;
      x_tick->r_pate   = pr;
      x_tick->r_tibi   = tr;
      /*---(revised endpoint)------*/
      x_tick->r_xpos   = xr;
      x_tick->r_zpos   = zr;
      x_tick->r_ypos   = yr;
      /*---(results)---------------*/
      x_tick->rc_exact = rc;
      x_tick->rc_pure  = x_pure;
      x_tick->rc_adapt = x_adapt;

      /*> if   (a_leg < 0)  strlcpy (x_leg, "ce", LEN_TERSE);                         <* 
       *> else              strlcpy (x_leg, yKINE_legtwo (x_leg), LEN_TERSE);         <*/
      /*> printf ("%8.2f %2d %2s   %6.1f %6.1f %6.1f   %6.1f %6.1f %6.1f    %3d   %6.1f %6.1f %6.1f   %6.1f %6.1f %6.1f    %c %-15.15s %c\n", x_pos, a_leg, x_leg, fo, po, to, xo, zo, yo, rc, fr, pr, tr, xr, zr, yr, x_exact, x_label, x_cell);   <*/
   }

   /*---(complete)-----------------------*/
   DEBUG_YKINE_TICK  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE_tick_load         (void)
{
   char        x_leg       =    0;
   ykine_tick__free ();
   ykine_tick__new  ();
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      ykine_tick_fill (x_leg, 0, myKINE.scrp_len);
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine_tick__unit        (char *a_question, char a_leg, int a_tick)
{
   int         i           =    0;
   char        t           [LEN_RECD]  = "";
   tTICK      *x_tick      = NULL;
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "TICK unit        : question not understood", LEN_RECD);
   /*---(answer)------------------------------------------*/
   if      (strcmp (a_question, "alloc"   ) == 0) {
      strlcpy (t, "  body ", LEN_RECD);
      for (i = 0; i < YKINE_MAX_LEGS; ++i) {
         if (i ==  2)              strlcat (t, "   bigs ", LEN_RECD);
         if (i ==  8)              strlcat (t, "   smls ", LEN_RECD);
         if (i == 12)              strlcat (t, "   oths ", LEN_RECD);
         if (s_ticks [i] == NULL)  strlcat (t, " -", LEN_RECD);
         else                      strlcat (t, " y", LEN_RECD);
      }
      sprintf (ykine__unit_answer, "TICK alloc       : %4d %s", s_ntick, t);
   }
   else if (strcmp (a_question, "pos"     ) == 0) {
      x_tick = s_ticks [a_leg + 1] + a_tick;
      sprintf (ykine__unit_answer, "TICK pos (%3d) : %c %c   %4d %6.1lfx %6.1lfz %6.1lfy   %4d %6.1lfx %6.1lfz %6.1lfy", a_tick, x_tick->used, x_tick->exact, x_tick->rc_pure, x_tick->o_xpos, x_tick->o_zpos, x_tick->o_ypos, x_tick->rc_adapt, x_tick->r_xpos, x_tick->r_zpos, x_tick->r_ypos);
   }
   else if (strcmp (a_question, "deg"     ) == 0) {
      x_tick = s_ticks [a_leg + 1] + a_tick;
      sprintf (ykine__unit_answer, "TICK deg (%3d) : %c %c   %4d %6.1lff %6.1lfp %6.1lft   %4d %6.1lff %6.1lfp %6.1lft", a_tick, x_tick->used, x_tick->exact, x_tick->rc_pure, x_tick->o_femu, x_tick->o_pate, x_tick->o_tibi, x_tick->rc_adapt, x_tick->r_femu, x_tick->r_pate, x_tick->r_tibi);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}




