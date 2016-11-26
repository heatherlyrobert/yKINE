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
   snprintf (yKINE_ver, 100, "%s   %s : %s", t, YKINE_VER_NUM, YKINE_VER_TXT);
   return yKINE_ver;
}

char         /*--> set debugging mode --------------------[ ------ [ ------ ]-*/
yKINE_debug        (char a_flag)
{
   /*---(set debug flag)-----------------*/
   if        (a_flag == 'A') {
      yKINE_its.debug_data   = 'y';
      yKINE_its.debug_calc   = 'y';
      yKINE_its.debug_scrp   = 'y';
      yKINE_its.unit         = 'y';
   } else if (a_flag == 'd') {
      yKINE_its.debug_data   = 'y';
   } else if (a_flag == 'c')  {
      yKINE_its.debug_calc   = 'y';
   } else if (a_flag == 's')  {
      yKINE_its.debug_scrp   = 'y';
   } else if (a_flag == 'u')  {
      yKINE_its.unit         = 'y';
   } else {
      yKINE_its.debug_data   = '-';
      yKINE_its.debug_calc   = '-';
      yKINE_its.debug_scrp   = '-';
      yKINE_its.unit         = '-';
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        initialization                        ----===*/
/*====================------------------------------------====================*/
static void      o___INIT____________________o (void) {;}

char       /*----: set all segments to defaults ------------------------------*/
yKINE_init         (char a_type)
{
   /*---(locals)-------------------------*/
   int      x_leg = 0;              /* iterator         */
   int      x_seg = 0;              /* iterator         */
   /*---(header)-------------------------*/
   yLOG_enter   (__FUNCTION__);
   /*---(set body)-----------------------*/
   /*> kine_center       (0.0f, 0.0f);                                                <* 
    *> kine_height       (segs_len [YKINE_TIBI]);                                           <* 
    *> kine_pivot        (0.0f, 0.0f);                                                <* 
    *> kine_attitude     (0.0f, 0.0f, 0.0f);                                          <*/
   /*---(clean legs)---------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      for (x_seg = 0; x_seg < YKINE_MAX_SEGS; ++x_seg) {
         yKINE__clear ( &(fk [x_leg][x_seg]), "fk", x_leg, x_seg, a_type);
         yKINE__clear ( &(ik [x_leg][x_seg]), "ik", x_leg, x_seg, a_type);
         yKINE__clear ( &(gk [x_leg][x_seg]), "gk", x_leg, x_seg, a_type);
      }
   }
   /*---(complete)-----------------------*/
   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: set segment kimematics to defaults ------------------------*/
yKINE__clear       (tSEG *a_curr, char *a_name, int a_leg, int a_seg, char a_type)
{
   /*---(defenses)-----------------------*/
   if (strlen(a_name) != 2)                   return -1;
   if (a_curr  == NULL)                       return -2;
   /*---(set name)-----------------------*/
   snprintf (a_curr->n, 11, "%s.%s", leg_data [a_leg].caps, seg_data [a_seg].four);
   /*---(self-knowledge)-----------------*/
   a_curr->leg    = a_leg;
   a_curr->seg    = a_seg;
   /*---(set lengths)--------------------*/
   switch (a_type) {
   case '1' :  a_curr->l  = a_curr->sl = seg_data [a_seg].test1;  break;
   case '2' :  a_curr->l  = a_curr->sl = seg_data [a_seg].test2;  break;
   default  :  a_curr->l  = a_curr->sl = seg_data [a_seg].len;    break;
   }
   a_curr->fl     =   0.0;
   /*---(angles)-------------------------*/
   a_curr->d      =   0.0;
   if (a_seg == YKINE_THOR)  a_curr->d    =  leg_data [a_leg].deg;
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

char       /*----: change the center of gravity ------------------------------*/
yKINE_center       (double a_x, double a_z, double a_y)
{
   /*---(locals)-----------+-----------+-*/
   int         x_legnum = 0;              /* iterator         */
   tSEG       *x_leg       = NULL;
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_double  ("a_x"       , a_x);
   DEBUG_YKINE_CALC   yLOG_double  ("a_z"       , a_z);
   DEBUG_YKINE_CALC   yLOG_double  ("a_y"       , a_y);
   /*---(kinematics)---------------------*/
   for (x_legnum = 0; x_legnum < YKINE_MAX_LEGS; ++x_legnum) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (x_legnum * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (x_legnum * YKINE_MAX_SEGS);
      /*---(save basics)-----------------*/
      x_leg [YKINE_CORE].cx   =  x_leg [YKINE_CORE].x   = a_x;
      x_leg [YKINE_CORE].cz   =  x_leg [YKINE_CORE].z   = a_z;
      x_leg [YKINE_CORE].cy   =  x_leg [YKINE_CORE].y   = a_y;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

#define       LEN_TEXT  2000
char          unit_answer [ LEN_TEXT ];

char*      /*----: unit testing accessor for clean validation interface ------*/
yKINE__getter      (char *a_question, int a_leg,  int a_seg)
{
   /*---(defense)-----------------------------------------*/
   if (a_leg < 0 || a_leg > YKINE_MAX_LEGS) {
      sprintf(unit_answer, "leg value out of range");
      return unit_answer;
   }
   if (a_seg < 0 || a_seg > YKINE_MAX_SEGS) {
      sprintf(unit_answer, "segment value out of range");
      return unit_answer;
   }
   strncpy (unit_answer, "unknown request", 100);
   /*---(answer)------------------------------------------*/
   if (strcmp(a_question, "IK_final") == 0) {
      sprintf(unit_answer, "IK %1d/final     : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, ik[a_leg][YKINE_CALC].x, ik[a_leg][YKINE_CALC].z, ik[a_leg][YKINE_CALC].y);
   } else if (strcmp(a_question, "FK_final") == 0) {
      sprintf(unit_answer, "FK %1d/final     : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, fk[a_leg][YKINE_CALC].x, fk[a_leg][YKINE_CALC].z, fk[a_leg][YKINE_CALC].y);
   } else if (strcmp(a_question, "IK_target") == 0) {
      sprintf(unit_answer, "IK %1d/target    : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, ik[a_leg][YKINE_TARG].x, ik[a_leg][YKINE_TARG].z, ik[a_leg][YKINE_TARG].y);
   } else if (strcmp(a_question, "IK_match") == 0) {
      if (  ik[a_leg][YKINE_CALC].x == ik[a_leg][YKINE_TARG].x &&
            ik[a_leg][YKINE_CALC].z == ik[a_leg][YKINE_TARG].z &&
            ik[a_leg][YKINE_CALC].y == ik[a_leg][YKINE_TARG].y )
         sprintf(unit_answer, "IK %1d/success", a_leg);
      else
         sprintf(unit_answer, "IK %1d/FAILURE", a_leg);
   } else if (strcmp(a_question, "IK_angles_OLD") == 0) {
      sprintf(unit_answer, "IK %1d/angles    : %8.2fc,%8.2ff,%8.2fp,%8.2ft", 
            a_leg, ik[a_leg][YKINE_COXA].d, ik[a_leg][YKINE_FEMU].d,
            ik[a_leg][YKINE_PATE].d, ik[a_leg][YKINE_TIBI].d);
   } else if (strcmp(a_question, "IK_lower")  == 0) {
      sprintf(unit_answer, "IK %1d/lower     : %8.3fp,%8.3ft", 
            a_leg, ik[a_leg][YKINE_PATE].v, ik[a_leg][YKINE_TIBI].v);
   } else if (strcmp(a_question, "final")     == 0) {
      sprintf(unit_answer, "%1d/%1d %4.4s final  %8.1fl,%8.1fx,%8.1fz,%8.1fy", 
            a_leg, a_seg, seg_data [a_seg].full, fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].x, fk[a_leg][a_seg].z, fk[a_leg][a_seg].y);
   }
   /*---(leg values)--------------------------------------*/
   else if (strcmp(a_question, "seg_angle"     ) == 0) {
      sprintf(unit_answer, "%-7.7s angle  :%8.1fm,%8.1fd,%8.3fv,%8.3fh", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].d , fk[a_leg][a_seg].cv, fk[a_leg][a_seg].ch);
   }
   else if (strcmp(a_question, "seg_size"      ) == 0) {
      sprintf(unit_answer, "%-7.7s size   :%8.1fm,%8.1fx,%8.1fz,%8.1fy", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].x , fk[a_leg][a_seg].z, fk[a_leg][a_seg].y);
   }
   else if (strcmp(a_question, "seg_end"       ) == 0) {
      sprintf(unit_answer, "%-7.7s end    :%8.1fm,%8.1fx,%8.1fz,%8.1fy", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].cx, fk[a_leg][a_seg].cz, fk[a_leg][a_seg].cy);
   }
   /*---(NEW TESTS)---------------------------------------*/
   else if (strcmp(a_question, "FK_seg_angle"     ) == 0) {
      sprintf(unit_answer, "FK-%-7.7s ang :%8.0fd,%8.2fv,%8.2fh", 
            fk[a_leg][a_seg].n ,
            fk[a_leg][a_seg].d , fk[a_leg][a_seg].cv, fk[a_leg][a_seg].ch);
   }
   else if (strcmp(a_question, "FK_seg_size"      ) == 0) {
      sprintf(unit_answer, "FK-%-7.7s siz :%8.0fm,%8.0fx,%8.0fz,%8.0fy", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].x , fk[a_leg][a_seg].z, fk[a_leg][a_seg].y);
   }
   else if (strcmp(a_question, "FK_seg_end"       ) == 0) {
      sprintf(unit_answer, "FK-%-7.7s end :%8.0fm,%8.0fx,%8.0fz,%8.0fy", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].fl,
            fk[a_leg][a_seg].cx, fk[a_leg][a_seg].cz, fk[a_leg][a_seg].cy);
   }
   else if (strcmp(a_question, "IK_seg_angle"     ) == 0) {
      sprintf(unit_answer, "IK-%-7.7s ang :%8.0fd,%8.2fv,%8.2fh", 
            ik[a_leg][a_seg].n ,
            ik[a_leg][a_seg].d , ik[a_leg][a_seg].cv, ik[a_leg][a_seg].ch);
   }
   else if (strcmp(a_question, "IK_seg_size"      ) == 0) {
      sprintf(unit_answer, "IK-%-7.7s siz :%8.0fm,%8.0fx,%8.0fz,%8.0fy", 
            ik[a_leg][a_seg].n , ik[a_leg][a_seg].l,
            ik[a_leg][a_seg].x , ik[a_leg][a_seg].z, ik[a_leg][a_seg].y);
   }
   else if (strcmp(a_question, "IK_seg_end"       ) == 0) {
      sprintf(unit_answer, "IK-%-7.7s end :%8.0fm,%8.0fx,%8.0fz,%8.0fy", 
            ik[a_leg][a_seg].n , ik[a_leg][a_seg].fl,
            ik[a_leg][a_seg].cx, ik[a_leg][a_seg].cz, ik[a_leg][a_seg].cy);
   }




   else if (strcmp(a_question, "FK_angles"     ) == 0) {
      sprintf(unit_answer, "%-10.10s deg :%8.1fm,%8.1fd,%8.3fv,%8.3fh", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].d , fk[a_leg][a_seg].cv, fk[a_leg][a_seg].ch);
   }
   else if (strcmp(a_question, "FK_segment"    ) == 0) {
      sprintf(unit_answer, "%-10.10s seg :%8.1fm,%8.1fx,%8.1fz,%8.1fy", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].x , fk[a_leg][a_seg].z, fk[a_leg][a_seg].y);
   }
   else if (strcmp(a_question, "FK_endpoint"   ) == 0) {
      sprintf(unit_answer, "%-10.10s end :%8.1fm,%8.1fx,%8.1fz,%8.1fy", 
            fk[a_leg][a_seg].n , fk[a_leg][a_seg].l,
            fk[a_leg][a_seg].cx, fk[a_leg][a_seg].cz, fk[a_leg][a_seg].cy);
   }
   else if (strcmp(a_question, "IK_angles"     ) == 0) {
      sprintf(unit_answer, "%-10.10s deg :%8.1fm,%8.1fd,%8.3fv,%8.3fh", 
            ik[a_leg][a_seg].n , ik[a_leg][a_seg].l,
            ik[a_leg][a_seg].d , ik[a_leg][a_seg].cv, ik[a_leg][a_seg].ch);
   }
   else if (strcmp(a_question, "IK_segment"    ) == 0) {
      sprintf(unit_answer, "%-10.10s seg :%8.1fm,%8.1fx,%8.1fz,%8.1fy", 
            ik[a_leg][a_seg].n , ik[a_leg][a_seg].l,
            ik[a_leg][a_seg].x , ik[a_leg][a_seg].z, ik[a_leg][a_seg].y);
   }
   else if (strcmp(a_question, "IK_endpoint"   ) == 0) {
      sprintf(unit_answer, "%-10.10s end :%8.1fm,%8.1fx,%8.1fz,%8.1fy", 
            ik[a_leg][a_seg].n , ik[a_leg][a_seg].l,
            ik[a_leg][a_seg].cx, ik[a_leg][a_seg].cz, ik[a_leg][a_seg].cy);
   }
   /*---(complete)----------------------------------------*/
   return unit_answer;
}

char       /*----: set up program urgents/debugging --------------------------*/
yKINE__testquiet   (void)
{
   yKINE_debug ('-');
   yKINE_its.logger = yLOG_begin    ("yKINE" , yLOG_SYSTEM, yLOG_QUIET);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yKINE__testloud    (void)
{
   yKINE_debug ('A');
   yKINE_its.logger = yLOG_begin    ("yKINE" , yLOG_SYSTEM, yLOG_NOISE);
   DEBUG_TOPS   yLOG_info     ("yKINE" , yKINE_version   ());
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yKINE__testend     (void)
{
   yKINE_debug ('-');
   yLOG_end      ();
   return 0;
}

