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
   char        rc_kine;
   float       r_vs_g;
   /*---(done)-----------------*/
};
static tTICK  *s_ticks [YKINE_MAX_LEGS] = { NULL };
static int     s_ntick  =    0;

/*> static tTICK  s_ticks  [YKINE_MAX_LEGS][MAX_TICK];                                 <*/
/*> static int   s_ntick  = 0;                                                        <*/




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
   a_tick->rc_kine     =   0;
   a_tick->r_vs_g      = 0.0;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_tick__create      (void)
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
      for (k = 0; k < s_ntick; ++k) {
         ykine_tick__clear ('-', (s_ticks [i]) + k);
      }
      free (s_ticks [i]);
      s_ticks [i] = NULL;
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
   float       x_pos, z_pos, y_pos;
   float       x_yaw, x_pitch, x_roll;
   /*---(header)-------------------------*/
   DEBUG_YKINE_TICK  yLOG_enter   (__FUNCTION__);
   /*---(point in time)------------------*/
   x_tick  = a_sec * 10;
   DEBUG_YKINE_TICK  yLOG_complex ("tick"      , "%8.2fs, %5dt", a_sec, x_tick);
   /*---(position)-----------------------*/
   /*> x_pos   = s_ticks [YKINE_BODY][x_tick].o_xpos;                                 <* 
    *> z_pos   = s_ticks [YKINE_BODY][x_tick].o_zpos;                                 <* 
    *> y_pos   = s_ticks [YKINE_BODY][x_tick].o_ypos;                                 <*/
   rc = yKINE_zero   (x_pos, z_pos, y_pos);
   DEBUG_YKINE_TICK  yLOG_complex ("position"  , "%4drc, %8.2fx, %8.2fz, %8.2fy", rc, x_pos, z_pos, y_pos);
   /*---(orientation)--------------------*/
   /*> x_yaw   = s_ticks [YKINE_BODY][x_tick].o_femu;                                 <* 
    *> x_pitch = s_ticks [YKINE_BODY][x_tick].o_pate;                                 <* 
    *> x_roll  = s_ticks [YKINE_BODY][x_tick].o_tibi;                                 <*/
   rc = yKINE_orient (x_yaw, x_pitch, x_roll);
   DEBUG_YKINE_TICK  yLOG_complex ("orient"    , "%4drc, %8.2fy, %8.2fp, %8.2fr", rc, x_yaw, x_pitch, x_roll);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_TICK  yLOG_enter   (__FUNCTION__);
   return 0;
}

char
ykine_tick_fill             (char a_leg)
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
   /*---(header)-------------------------*/
   DEBUG_YKINE_TICK  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   x_beg = (myKINE.sb / 10.0);  /*   * my.p_scale;         */
   x_end = (myKINE.se / 10.0);  /*   * my.p_scale;         */
   /*---(fill)---------------------------*/
   for (x_pos = x_beg; x_pos <= x_end; ++x_pos) {
      /*> if (x_pos > my.p_len)  break;                                               <*/
      if (x_pos < 0.0     )  break;
      rc = yKINE_exact_all (x_pos);
      if (rc < 0)            continue;
      rc = yKINE_exact_leg (a_leg, 0.10, &x_exact, x_label, &x_cell, &fo, &po, &to, &xo, &zo, &yo, &fr, &pr, &tr, &xr, &zr, &yr);
      ++a_leg;

      --a_leg;
      if   (a_leg < 0)  strlcpy (x_leg, "ce", LEN_TERSE);
      else              strlcpy (x_leg, yKINE_legtwo (x_leg), LEN_TERSE);
      printf ("%8.2f %2d %2s   %6.1f %6.1f %6.1f   %6.1f %6.1f %6.1f    %3d   %6.1f %6.1f %6.1f   %6.1f %6.1f %6.1f    %c %-15.15s %c\n", x_pos, a_leg, x_leg, fo, po, to, xo, zo, yo, rc, fr, pr, tr, xr, zr, yr, x_exact, x_label, x_cell);
   }

   /*---(complete)-----------------------*/
   DEBUG_YKINE_TICK  yLOG_enter   (__FUNCTION__);
   return 0;
}


/*> char         /+--> draw texture for progress ticker ------[ ------ [ ------ ]-+/                                                                                                                                                         <* 
 *> TICK_load_exact         (tPANEL *a_panel)                                                                                                                                                                                                <* 
 *> {                                                                                                                                                                                                                                        <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                                                                                                                              <* 
 *>    char        rc          =    0;                                                                                                                                                                                                       <* 
 *>    int         i;                             /+ loop iterator                  +/                                                                                                                                                       <* 
 *>    int         j;                             /+ loop iterator                  +/                                                                                                                                                       <* 
 *>    float       x_pos       =  0.0;                                                                                                                                                                                                       <* 
 *>    float       f, p, t, fr, pr, tr;                                                                                                                                                                                                      <* 
 *>    float       x, y, z, xr, yr, zr;                                                                                                                                                                                                      <* 
 *>    float       x_yaw       =    0;                                                                                                                                                                                                       <* 
 *>    float       x_pitch     =    0;                                                                                                                                                                                                       <* 
 *>    float       x_roll      =    0;                                                                                                                                                                                                       <* 
 *>    char        x_exact     =  '-';                                                                                                                                                                                                       <* 
 *>    char        x_leg       [LEN_TERSE];                                                                                                                                                                                                  <* 
 *>    char        x_label     [LEN_LABEL];                                                                                                                                                                                                  <* 
 *>    char        x_cell      =  '-';                                                                                                                                                                                                       <* 
 *>    DEBUG_GRAF   yLOG_enter   (__FUNCTION__);                                                                                                                                                                                             <* 
 *>    DEBUG_GRAF   yLOG_point   ("panel"     , a_panel);                                                                                                                                                                                    <* 
 *>    DEBUG_GRAF   yLOG_double  ("beg"       , a_panel->beg);                                                                                                                                                                               <* 
 *>    DEBUG_GRAF   yLOG_double  ("my.p_len"  , my.p_len);                                                                                                                                                                                   <* 
 *>    /+---(load original points)-----------+/                                                                                                                                                                                              <* 
 *>    tick_panel_wipe (a_panel, '-');                                                                                                                                                                                                       <* 
 *>    for (i = 0; i < MAX_WIDE; ++i) {                                                                                                                                                                                                      <* 
 *>       x_pos       = a_panel->beg + ((i / 10.0) * my.p_scale);                                                                                                                                                                            <* 
 *>       if (x_pos > my.p_len)  break;                                                                                                                                                                                                      <* 
 *>       if (x_pos < 0.0     )  break;                                                                                                                                                                                                      <* 
 *>       rc = yKINE_exact_all (x_pos);                                                                                                                                                                                                      <* 
 *>       if (rc < 0)  continue;                                                                                                                                                                                                             <* 
 *>       printf ("\n");                                                                                                                                                                                                                     <* 
 *>       if (i % 3 == 0)  printf ("____secs ## CALC __femu __pate __tibi   __xpos __zpos __ypos    _IK   __femu __pate __tibi   __xpos __zpos __ypos\n\n");                                                                                 <* 
 *>       for (j = YKINE_CENTER; j <= YKINE_LR; ++j) {                                                                                                                                                                                       <* 
 *>          rc = yKINE_exact_leg   (j, 0.10, &x_exact, x_label, &x_cell, &f, &p, &t, &x, &z, &y, &fr, &pr, &tr, &xr, &zr, &yr);                                                                                                             <* 
 *>          ++j;                                                                                                                                                                                                                            <* 
 *>          /+> rc = 0;                                                                  <+/                                                                                                                                                <* 
 *>          /+---(special)---------------+/                                                                                                                                                                                                 <* 
 *>          a_panel->detail [j][i].used   = 'y';                                                                                                                                                                                            <* 
 *>          a_panel->detail [j][i].exact  = x_exact;                                                                                                                                                                                        <* 
 *>          if (x_label [0] != '-')  a_panel->detail [j][i].label  = strdup (x_label);                                                                                                                                                      <* 
 *>          a_panel->detail [j][i].cell   = x_cell;                                                                                                                                                                                         <* 
 *>          /+---(original endpoint)-----+/                                                                                                                                                                                                 <* 
 *>          a_panel->detail [j][i].o_xpos = x;                                                                                                                                                                                              <* 
 *>          a_panel->detail [j][i].o_zpos = z;                                                                                                                                                                                              <* 
 *>          a_panel->detail [j][i].o_ypos = y;                                                                                                                                                                                              <* 
 *>          /+---(original angles)-------+/                                                                                                                                                                                                 <* 
 *>          a_panel->detail [j][i].o_femu = f;                                                                                                                                                                                              <* 
 *>          a_panel->detail [j][i].o_pate = p;                                                                                                                                                                                              <* 
 *>          a_panel->detail [j][i].o_tibi = t;                                                                                                                                                                                              <* 
 *>          /+---(revised angles)--------+/                                                                                                                                                                                                 <* 
 *>          a_panel->detail [j][i].r_femu = fr;                                                                                                                                                                                             <* 
 *>          a_panel->detail [j][i].r_pate = pr;                                                                                                                                                                                             <* 
 *>          a_panel->detail [j][i].r_tibi = tr;                                                                                                                                                                                             <* 
 *>          /+---(revised endpoint)------+/                                                                                                                                                                                                 <* 
 *>          a_panel->detail [j][i].r_xpos = xr;                                                                                                                                                                                             <* 
 *>          a_panel->detail [j][i].r_zpos = zr;                                                                                                                                                                                             <* 
 *>          a_panel->detail [j][i].r_ypos = yr;                                                                                                                                                                                             <* 
 *>          /+---(results)---------------+/                                                                                                                                                                                                 <* 
 *>          a_panel->detail [j][i].r_rc = rc;                                                                                                                                                                                               <* 
 *>          /+---(done)------------------+/                                                                                                                                                                                                 <* 
 *>          --j;                                                                                                                                                                                                                            <* 
 *>          if      (j < 0)  strlcpy (x_leg, "ce", LEN_TERSE);                                                                                                                                                                              <* 
 *>          else             strlcpy (x_leg, yKINE_legtwo (j), LEN_TERSE);                                                                                                                                                                  <* 
 *>          printf ("%8.2f %2d %2s   %6.1f %6.1f %6.1f   %6.1f %6.1f %6.1f    %3d   %6.1f %6.1f %6.1f   %6.1f %6.1f %6.1f    %c %-15.15s %c\n", x_pos, j, x_leg, f, p, t, x, z, y, rc, fr, pr, tr, xr, zr, yr, x_exact, x_label, x_cell);   <* 
 *>       }                                                                                                                                                                                                                                  <* 
 *>    }                                                                                                                                                                                                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                                                                                                                              <* 
 *>    DEBUG_GRAF   yLOG_exit    (__FUNCTION__);                                                                                                                                                                                             <* 
 *>    return 0;                                                                                                                                                                                                                             <* 
 *> }                                                                                                                                                                                                                                        <*/




/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine_tick__unit        (char *a_question, int a_num)
{
   int         i           =    0;
   char        t           [LEN_RECD]  = "";
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
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}





