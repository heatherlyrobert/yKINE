/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"





char        yKINE_ver     [500];






tSEG      fk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* forward kinematics        */
tSEG      ik [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* inverse kinematics        */
tSEG      gk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* opengl kinematics         */













/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void      o___UTILITY_________________o (void) {;}

char*      /* ---- : return library versioning information -------------------*/
yKINE_version      (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strncpy (t, "[tcc built  ]", 15);
#elif  __GNUC__  > 0
   strncpy (t, "[gnu gcc    ]", 15);
#elif  __HEPH__  > 0
   strncpy (t, "[hephaestus ]", 15);
#else
   strncpy (t, "[unknown    ]", 15);
#endif
   snprintf (yKINE_ver, 100, "%s   %s : %s", t, P_VERNUM, P_VERTXT);
   return yKINE_ver;
}



/*====================------------------------------------====================*/
/*===----                        initialization                        ----===*/
/*====================------------------------------------====================*/
static void      o___INIT____________________o (void) {;}

char       /*----: set segment kimematics to defaults ------------------------*/
ykine__clear       (tSEG *a_curr, char *a_name, int a_leg, int a_seg, char a_type)
{
   /*---(defenses)-----------------------*/
   if (strlen(a_name) != 2)                   return -1;
   if (a_curr  == NULL)                       return -2;
   /*---(set name)-----------------------*/
   snprintf (a_curr->n, 11, "%s.%s", g_leg_data [a_leg].caps, g_seg_data [a_seg].four);
   /*---(self-knowledge)-----------------*/
   a_curr->leg    = a_leg;
   a_curr->seg    = a_seg;
   /*---(set lengths)--------------------*/
   switch (a_type) {
   case '1' :  a_curr->l  = a_curr->sl = g_seg_data [a_seg].test1;  break;
   case '2' :  a_curr->l  = a_curr->sl = g_seg_data [a_seg].test2;  break;
   default  :  a_curr->l  = a_curr->sl = g_seg_data [a_seg].len;    break;
   }
   a_curr->fl     =   0.0;
   /*---(angles)-------------------------*/
   a_curr->d      =   0.0;
   if (a_seg == YKINE_THOR)  a_curr->d    =  g_leg_data [a_leg].deg;
   a_curr->h      =   0.0;
   a_curr->v      =   0.0;
   a_curr->cd     =   0.0;
   a_curr->ch     =   0.0;
   a_curr->cv     =   0.0;
   /*---(flags)--------------------------*/
   a_curr->u      =   '-';
   /*---(coordinates)--------------------*/
   a_curr->x      =   0.0;
   a_curr->z      =   0.0;
   a_curr->xz     =   0.0;
   a_curr->y      =   0.0;
   a_curr->cx     =   0.0;
   a_curr->cz     =   0.0;
   a_curr->cy     =   0.0;
   /*---(control)------------------------*/
   a_curr->p      =   'n';
   a_curr->m      =   'i';
   a_curr->c      =   'n';
   return 0;
}

char
yKINE_sizing            (char a_type)
{
   int      x_leg = 0;              /* iterator         */
   int      x_seg = 0;              /* iterator         */
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      for (x_seg = 0; x_seg < YKINE_MAX_SEGS; ++x_seg) {
         ykine__clear ( &(fk [x_leg][x_seg]), "fk", x_leg, x_seg, a_type);
         ykine__clear ( &(ik [x_leg][x_seg]), "ik", x_leg, x_seg, a_type);
         ykine__clear ( &(gk [x_leg][x_seg]), "gk", x_leg, x_seg, a_type);
      }
   }
}

char       /*----: set all segments to defaults ------------------------------*/
yKINE_init         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int      x_leg = 0;              /* iterator         */
   int      x_seg = 0;              /* iterator         */
   char        x_mute      =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   /*---(cut log volume)-----------------*/
   /*> yLOGS_mute  ();                                                                <*/
   x_mute = yLOGS_mute_check ();
   if (x_mute == 1)   yLOGS_mute ();
   /*---(set body)-----------------------*/
   /*> kine_center       (0.0f, 0.0f);                                                <* 
    *> kine_height       (segs_len [YKINE_TIBI]);                                           <* 
    *> kine_pivot        (0.0f, 0.0f);                                                <* 
    *> kine_attitude     (0.0f, 0.0f, 0.0f);                                          <*/
   /*---(clean legs)---------------------*/
   yKINE_sizing (0);
   g_center.yaw    = g_center.pitch   = g_center.roll   =  0.0;
   g_center.xpos   = g_center.zpos    = g_center.ypos   =  0.0;
   g_center.height = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   /*---(initialize)---------------------*/
   rc = ykine_servo_init ();
   DEBUG_YKINE  yLOG_value   ("servo"     , rc);
   rc = ykine_move_init  ();
   DEBUG_YKINE  yLOG_value   ("move"      , rc);
   rc = ykine_scrp_begin ();
   DEBUG_YKINE  yLOG_value   ("scrp"      , rc);
   rc = yPARSE_init      (YPARSE_NOREAD, ykine_scrp_popverb, YPARSE_REUSING);
   DEBUG_YKINE  yLOG_value   ("yparse"    , rc);
   rc = yPARSE_delimiters(YPARSE_FUNCTION);
   DEBUG_YKINE  yLOG_value   ("delimiters", rc);
   rc = ykine_verb_init  ();
   DEBUG_YKINE  yLOG_value   ("verb"      , rc);
   rc = ykine_hint_init  ();
   DEBUG_YKINE  yLOG_value   ("hint"      , rc);
   rc = ykine_turtle_init    ();
   DEBUG_YKINE  yLOG_value   ("turtle"    , rc);
   rc = ykine_step_init      ();
   DEBUG_YKINE  yLOG_value   ("step"      , rc);
   ykine_stance_verify  ();
   myKINE.s_pace  = YKINE_PACE;
   ykine_tick_init  ();
   /*---(ready to return)----------------*/
   if (x_mute == 1)   yLOGS_unmute ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE_reinit            (void)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   rc = ykine_servo_init ();
   DEBUG_YKINE  yLOG_value   ("servo"     , rc);
   rc = ykine_move_init  ();
   DEBUG_YKINE  yLOG_value   ("move"      , rc);
   rc = ykine_scrp_begin ();
   DEBUG_YKINE  yLOG_value   ("scrp"      , rc);
   rc = ykine_hint_init  ();
   DEBUG_YKINE  yLOG_value   ("hint"      , rc);
   rc = ykine_turtle_init    ();
   DEBUG_YKINE  yLOG_value   ("turtle"    , rc);
   myKINE.s_pace  = YKINE_PACE;
   /*---(complete)-----------------------*/
   DEBUG_YKINE  yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char       /+----: change the center of gravity ------------------------------+/   <* 
 *> yKINE_center       (double a_x, double a_z, double a_y)                            <* 
 *> {                                                                                  <* 
 *>    /+---(locals)-----------+-----------+-+/                                        <* 
 *>    int         x_legnum = 0;              /+ iterator         +/                   <* 
 *>    tSEG       *x_leg       = NULL;                                                 <* 
 *>    int         i           =   0;                                                  <* 
 *>    /+---(header)-------------------------+/                                        <* 
 *>    DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);                                 <* 
 *>    DEBUG_YKINE_CALC   yLOG_double  ("a_x"       , a_x);                            <* 
 *>    DEBUG_YKINE_CALC   yLOG_double  ("a_z"       , a_z);                            <* 
 *>    DEBUG_YKINE_CALC   yLOG_double  ("a_y"       , a_y);                            <* 
 *>    /+---(kinematics)---------------------+/                                        <* 
 *>    for (x_legnum = 0; x_legnum < YKINE_MAX_LEGS; ++x_legnum) {                     <* 
 *>       DEBUG_YKINE_CALC   yLOG_value   ("legnum"    , x_legnum);                    <* 
 *>       for (i = 0; i < 2; ++i) {                                                    <* 
 *>          DEBUG_YKINE_CALC   yLOG_value   ("fk/ik"     , i);                        <* 
 *>          /+---(test and set)----------------+/                                     <* 
 *>          if (i == 0)  x_leg = ((tSEG *) fk) + (x_legnum * YKINE_MAX_SEGS);         <* 
 *>          if (i == 1)  x_leg = ((tSEG *) ik) + (x_legnum * YKINE_MAX_SEGS);         <* 
 *>          /+---(save basics)-----------------+/                                     <* 
 *>          x_leg [YKINE_CORE].cx   =  x_leg [YKINE_CORE].x   = a_x;                  <* 
 *>          x_leg [YKINE_CORE].cz   =  x_leg [YKINE_CORE].z   = a_z;                  <* 
 *>          x_leg [YKINE_CORE].cy   =  x_leg [YKINE_CORE].y   = a_y;                  <* 
 *>          /+---(done)------------------------+/                                     <* 
 *>       }                                                                            <* 
 *>    }                                                                               <* 
 *>    /+---(complete)-----------------------+/                                        <* 
 *>    DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);                                 <* 
 *>    return 0;                                                                       <* 
 *> }                                                                                  <*/


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char          ykine__unit_answer [LEN_RECD];

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__getter      (char *a_question, int a_leg,  int a_seg)
{
   float       x, z, y, e;
   /*---(defense)-----------------------------------------*/
   if (a_leg < 0 || a_leg > YKINE_MAX_LEGS) {
      sprintf(ykine__unit_answer, "leg value out of range");
      return ykine__unit_answer;
   }
   if (a_seg < 0 || a_seg > YKINE_MAX_SEGS) {
      sprintf(ykine__unit_answer, "segment value out of range");
      return ykine__unit_answer;
   }
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "BASE unit        : question not understood", LEN_RECD);
   /*---(answer)------------------------------------------*/
   if (strcmp(a_question, "FK_final") == 0) {
      sprintf(ykine__unit_answer, "FK %1d/final     : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, fk[a_leg][YKINE_CALC].x, fk[a_leg][YKINE_CALC].z, fk[a_leg][YKINE_CALC].y);
   } else if (strcmp(a_question, "IK_target") == 0) {
      sprintf(ykine__unit_answer, "IK %1d/target    : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, ik[a_leg][YKINE_TARG].x, ik[a_leg][YKINE_TARG].z, ik[a_leg][YKINE_TARG].y);
   } else if (strcmp(a_question, "IK_match") == 0) {
      if (  ik[a_leg][YKINE_CALC].x == ik[a_leg][YKINE_TARG].x &&
            ik[a_leg][YKINE_CALC].z == ik[a_leg][YKINE_TARG].z &&
            ik[a_leg][YKINE_CALC].y == ik[a_leg][YKINE_TARG].y )
         sprintf(ykine__unit_answer, "IK %1d/success", a_leg);
      else
         sprintf(ykine__unit_answer, "IK %1d/FAILURE", a_leg);
   } else if (strcmp(a_question, "IK_lower")  == 0) {
      sprintf(ykine__unit_answer, "IK %1d/lower     : %8.3fp,%8.3ft", 
            a_leg, ik[a_leg][YKINE_PATE].v, ik[a_leg][YKINE_TIBI].v);
   } else if (strcmp(a_question, "final")     == 0) {
      sprintf(ykine__unit_answer, "%1d/%1d %4.4s final  %8.1fl,%8.1fx,%8.1fz,%8.1fy", 
            a_leg, a_seg, g_seg_data [a_seg].full, fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].x, fk[a_leg][a_seg].z, fk[a_leg][a_seg].y);
   }
   /*---(forward kinematics)------------------------------*/
   else if (strcmp(a_question, "FK_seg_angle"     ) == 0) {
      sprintf(ykine__unit_answer, "FK-%-7.7s ang : %6.1fd,%6.1fcd,%6.3fcv,%6.3fch,%6.3ffv,%6.3ffh", 
            fk [a_leg][a_seg].n , fk [a_leg][a_seg].d , fk [a_leg][a_seg].cd,
            fk [a_leg][a_seg].cv, fk [a_leg][a_seg].ch,
            fk [a_leg][a_seg].fv, fk [a_leg][a_seg].fh);
   }
   else if (strcmp(a_question, "FK_seg_size"      ) == 0) {
      sprintf(ykine__unit_answer, "FK-%-7.7s siz :%8.1fm,%8.1fx,%8.1fz,%8.1fy,%8.1fxz", 
            fk [a_leg][a_seg].n , fk [a_leg][a_seg].l ,
            fk [a_leg][a_seg].x , fk [a_leg][a_seg].z ,
            fk [a_leg][a_seg].y , fk [a_leg][a_seg].xz);
   }
   else if (strcmp(a_question, "FK_seg_end"       ) == 0) {
      sprintf(ykine__unit_answer, "FK-%-7.7s end :%8.1fm,%8.1fx,%8.1fz,%8.1fy,%8.1fxz", 
            fk [a_leg][a_seg].n , fk [a_leg][a_seg].fl,
            fk [a_leg][a_seg].cx, fk [a_leg][a_seg].cz,
            fk [a_leg][a_seg].cy, fk [a_leg][a_seg].cxz);
   }
   /*---(inverse kinematics)------------------------------*/
   else if (strcmp (a_question, "IK_seg_angle"     ) == 0) {
      sprintf(ykine__unit_answer, "IK-%-7.7s ang : %6.1fd,%6.1fcd,%6.3fcv,%6.3fch,%6.3ffv,%6.3ffh", 
            ik [a_leg][a_seg].n , ik [a_leg][a_seg].d , ik [a_leg][a_seg].cd,
            ik [a_leg][a_seg].cv, ik [a_leg][a_seg].ch,
            ik [a_leg][a_seg].fv, ik [a_leg][a_seg].fh);
   }
   else if (strcmp (a_question, "IK_seg_size"      ) == 0) {
      sprintf(ykine__unit_answer, "IK-%-7.7s siz :%8.1fm,%8.1fx,%8.1fz,%8.1fy,%8.1fxz", 
            ik [a_leg][a_seg].n , ik [a_leg][a_seg].l ,
            ik [a_leg][a_seg].x , ik [a_leg][a_seg].z ,
            ik [a_leg][a_seg].y , ik [a_leg][a_seg].xz);
   }
   else if (strcmp (a_question, "IK_seg_end"       ) == 0) {
      sprintf(ykine__unit_answer, "IK-%-7.7s end :%8.1fm,%8.1fx,%8.1fz,%8.1fy,%8.1fxz", 
            ik [a_leg][a_seg].n , ik [a_leg][a_seg].fl,
            ik [a_leg][a_seg].cx, ik [a_leg][a_seg].cz,
            ik [a_leg][a_seg].cy, ik [a_leg][a_seg].cxz);
   }
   else if (strcmp (a_question, "IK_angles"    ) == 0) {
      sprintf(ykine__unit_answer, "IK-%-2.2s/angles   : %8.1fc, %8.1ff, %8.1fp, %8.1ft", 
            ik [a_leg][a_seg].n ,
            ik [a_leg][YKINE_THOR].d, ik [a_leg][YKINE_FEMU].d,
            ik [a_leg][YKINE_PATE].d, ik [a_leg][YKINE_TIBI].d);
   }
   else if (strcmp (a_question, "IK_final") == 0) {
      x = ik [a_leg][YKINE_TARG].x - ik [a_leg][YKINE_CALC].x;
      z = ik [a_leg][YKINE_TARG].z - ik [a_leg][YKINE_CALC].z;
      y = ik [a_leg][YKINE_TARG].y - ik [a_leg][YKINE_CALC].y;
      e = sqrt ((x * x) + (z * z) + (y * y));
      sprintf(ykine__unit_answer, "IK-%-2.2s/final    : %8.1fx, %8.1fz, %8.1fy, %8.3fe", 
            ik [a_leg][a_seg].n, ik [a_leg][YKINE_CALC].x,
            ik [a_leg][YKINE_CALC].z, ik [a_leg][YKINE_CALC].y, e);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}

char       /*----: set up program urgents/debugging --------------------------*/
ykine__unit_quiet  (void)
{
   char       *x_args [1]  = { "yKINE"      , ""        };
   yURG_logger (1, x_args);
   yURG_urgs   (1, x_args);
   yKINE_init  ();
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
ykine__unit_loud   (void)
{
   char       *x_args [9]  = { "yKINE_unit" , "@@ykine", "@@ykine_calc", "@@ykine_tick", "@@ykine_data", "@@ykine_scrp", "@@ykine_move", "@@ykine_exact", "@@yparse" };
   yURG_logger (9, x_args);
   yURG_urgs   (9, x_args);
   DEBUG_YKINE  yLOG_info     ("yKINE" , yKINE_version   ());
   yKINE_init  ();
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
ykine__unit_end    (void)
{
   yLOGS_end     ();
   return 0;
}

