/*============================----beg-of-source---============================*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


/*===[[ METIS ]]==============================================================*/
/*
 * metis  dm8··  add foot size logic to match actual leg size at various angles
 *
 *
 *
 */



const    float    DEG2RAD   = M_PI / 180.0;
const    float    RAD2DEG   = 180.0 / M_PI;


static tSEG   *s_leg       = NULL;
static int     s_meth       = 0;



/*====================------------------------------------====================*/
/*===----                    configuration functions                   ----===*/
/*====================------------------------------------====================*/
static void      o___CONFIG__________________o (void) {;}

char
ykine__setleg      (int a_num, int a_meth)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_sint    (a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_snote   ("leg/seg out of range");
      DEBUG_YKINE_CALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(set leg)------------------------*/
   DEBUG_YKINE_CALC   yLOG_sint    (a_meth);
   DEBUG_YKINE_CALC   yLOG_snote   ((a_meth == YKINE_FK) ? "FK" : "IK");
   --rce;  switch (a_meth) {
   case YKINE_FK :  s_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);  break;
   case YKINE_IK :  s_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);  break;
   case YKINE_GK :  s_leg = ((tSEG *) gk) + (a_num * YKINE_MAX_SEGS);  break;
   default       :
                    DEBUG_YKINE_CALC   yLOG_snote   ("meth not legal");
                    DEBUG_YKINE_CALC   yLOG_sexitr  (__FUNCTION__, rce);
                    return rce;
                    break;
   }
   s_meth = a_meth;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine__unsetleg    (void)
{
   s_leg = NULL;
   return 0;
}


char       /*----: clear most recent kinematics working data -----------------*/
ykine__wipe        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_senter  (__FUNCTION__);
   /*---(clear)--------------------------*/
   for (i = 0; i < YKINE_MAX_SEGS; ++i) {
      DEBUG_YKINE_CALC   yLOG_sint    (i);
      /*---(angles)---------------*/
      if (i > YKINE_THOR)  s_leg [i].d      =   0.0;
      s_leg [i].h      =   0.0;
      s_leg [i].v      =   0.0;
      s_leg [i].cd     =   0.0;
      s_leg [i].ch     =   0.0;
      s_leg [i].cv     =   0.0;
      s_leg [i].fh     =   0.0;
      s_leg [i].fv     =   0.0;
      /*---(flags)----------------*/
      s_leg [i].u      =   '-';
      /*---(coordinates)----------*/
      s_leg [i].x      =   0.0;
      s_leg [i].z      =   0.0;
      s_leg [i].xz     =   0.0;
      s_leg [i].y      =   0.0;
      s_leg [i].cx     =   0.0;
      s_leg [i].cz     =   0.0;
      s_leg [i].cy     =   0.0;
      /*---(lengths)--------------*/
      s_leg [i].fl     =   0.0;
      s_leg [i].sl     =   0.0;
      /*---(control)--------------*/
      s_leg [i].p      =   'n';
      s_leg [i].m      =   'i';
      s_leg [i].c      =   'n';
      /*---(done)-----------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     shared calculations                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char
ykine__chcv             (int a_seg)
{
   /*---(locals)-----------+-----+-----+-*/
   double      x_full      = 2 * M_PI;
   tSEG       *x_curr      = NULL;
   /*---(prep)---------------------------*/
   x_curr  = &(s_leg [a_seg]);
   /*---(determine ch/cv)----------------*/
   switch (a_seg) {
   case YKINE_THOR :
      x_curr->cd = x_curr->d;
      x_curr->ch = x_curr->cd * DEG2RAD;
      break;
   case YKINE_COXA : case YKINE_TROC :
      x_curr->cd = s_leg [YKINE_THOR].cd;
      x_curr->ch = s_leg [YKINE_THOR].ch;
      break;
   case YKINE_FEMU :
      x_curr->cd = s_leg [YKINE_THOR].cd +  x_curr->d;
      x_curr->ch = x_curr->cd  * DEG2RAD;
      break;
   case YKINE_LOWR :
      x_curr->cd = s_leg [YKINE_FEMU].cd;
      x_curr->ch = s_leg [YKINE_FEMU].ch;
      break;
   case YKINE_PATE :
      x_curr->cd = x_curr->d;
      x_curr->ch = s_leg [YKINE_FEMU].ch;
      x_curr->cv = x_curr->cd  * DEG2RAD;
      break;
   case YKINE_TIBI :
      x_curr->cd = s_leg [YKINE_PATE].cd + (x_curr->d + 90.0);
      x_curr->ch = s_leg [YKINE_PATE].ch;
      x_curr->cv = x_curr->cd  * DEG2RAD;
      break;
   case YKINE_META : case YKINE_TARS :
      x_curr->cd = s_leg [YKINE_TIBI].cd;
      x_curr->ch = s_leg [YKINE_TIBI].ch;
      x_curr->cv = s_leg [YKINE_TIBI].cv;
   case YKINE_FOOT :
      x_curr->cd = s_leg [YKINE_TIBI].cd + x_curr->d;
      x_curr->ch = s_leg [YKINE_TIBI].ch;
      x_curr->cv = x_curr->cd  * DEG2RAD;
      break;
   }
   /*---(calc fh)------------------------*/
   x_curr->fh   = atan2 (-x_curr->cz, x_curr->cx);
   while (x_curr->fh <  0     )  x_curr->fh += x_full;
   while (x_curr->fh >= x_full)  x_curr->fh -= x_full;
   if    (x_curr->fh >= x_full - 0.001)  x_curr->fh = 0.0;
   /*---(calc fv)------------------------*/
   x_curr->fv   = atan2 (-x_curr->cy, x_curr->cxz);
   while (x_curr->fv <  0     )  x_curr->fv += x_full;
   while (x_curr->fv >= x_full)  x_curr->fv -= x_full;
   if    (x_curr->fv >= x_full - 0.001)  x_curr->fv = 0.0;
   /*---(debug)--------------------------*/
   DEBUG_YKINE_CALC   yLOG_complex ("endangle" , "%6.3fch, %6.3fcv, %6.3ffh, %6.3ffv", x_curr->ch, x_curr->cv, x_curr->fh, x_curr->fv);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine__cums             (int a_seg)
{
   /*---(locals)-----------+-----+-----+-*/
   double      x_full      = 2 * M_PI;
   tSEG       *x_curr      = NULL;
   tSEG       *x_prev      = NULL;
   tSEG       *x_calc      = NULL;
   /*---(prep)---------------------------*/
   x_curr  = &(s_leg [a_seg]);
   x_prev  = &(s_leg [a_seg - 1]);
   x_calc  = &(s_leg [YKINE_CALC]);
   /*---(coords)-------------------------*/
   x_curr->cx   =  x_prev->cx + x_curr->x;
   x_curr->cz   =  x_prev->cz + x_curr->z;
   x_curr->cy   =  x_prev->cy + x_curr->y;
   DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", x_curr->cx, x_curr->cz, x_curr->cy);
   /*---(lengths)------------------------*/
   x_curr->cxz  =  sqrt ((x_curr->cx * x_curr->cx) + (x_curr->cz * x_curr->cz));
   x_curr->sl   =  sqrt ((x_curr->x  * x_curr->x ) + (x_curr->z  * x_curr->z ) + (x_curr->y  * x_curr->y ));
   x_curr->fl   =  sqrt ((x_curr->cx * x_curr->cx) + (x_curr->cz * x_curr->cz) + (x_curr->cy * x_curr->cy));
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", x_curr->xz, x_curr->sl, x_curr->fl);
   /*---(calc ch)------------------------*/
   ykine__chcv (a_seg);
   /*---(save to calc)-------------------*/
   x_calc->x   += x_curr->x;
   x_calc->z   += x_curr->z;
   x_calc->y   += x_curr->y;
   DEBUG_YKINE_CALC   yLOG_complex ("calc vals", "%6.3fx , %6.3fz , %6.3fy", x_calc->x, x_calc->z, x_calc->y);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine__cums_copy        (int a_seg)
{
   /*---(locals)-----------+-----+-----+-*/
   double      x_full      = 2 * M_PI;
   tSEG       *x_curr      = NULL;
   tSEG       *x_prev      = NULL;
   /*---(prep)---------------------------*/
   x_curr  = &(s_leg [a_seg]);
   x_prev  = &(s_leg [a_seg - 1]);
   /*---(coords)-------------------------*/
   x_curr->cx   =  x_prev->cx;
   x_curr->cz   =  x_prev->cz;
   x_curr->cy   =  x_prev->cy;
   DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", x_curr->cx, x_curr->cz, x_curr->cy);
   /*---(lengths)------------------------*/
   x_curr->cxz  =  x_prev->cxz;
   x_curr->fl   =  x_prev->fl;
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", x_curr->xz, x_curr->sl, x_curr->fl);
   /*---(chcv)---------------------------*/
   x_curr->ch   =  x_prev->ch;
   x_curr->cv   =  x_prev->cv;
   x_curr->fh   =  x_prev->fh;
   x_curr->fv   =  x_prev->fv;
   DEBUG_YKINE_CALC   yLOG_complex ("endangle" , "%6.3fch, %6.3fcv, %6.3ffh, %6.3ffv", x_curr->ch, x_curr->cv, x_curr->fh, x_curr->fv);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    zero and orientation                      ----===*/
/*====================------------------------------------====================*/
static void      o___BODY____________________o (void) {;}

char         /*--: establish body zero-point -------------[ leaf   [ ------ ]-*/
yKINE_zero              (float a_x, float a_z, float a_y)
{
   myKINE.s_xcenter = a_x;
   myKINE.s_zcenter = a_z;
   myKINE.s_ycenter = a_y;
   return 0;
}

char         /*--: establish body orientation ------------[ leaf   [ ------ ]-*/
yKINE_orient            (float a_yaw, float a_pitch, float a_roll)
{
   myKINE.s_yaw     = a_yaw;
   myKINE.s_pitch   = a_pitch;
   myKINE.s_roll    = a_roll;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       fixed body parts                       ----===*/
/*====================------------------------------------====================*/
static void      o___FIXED___________________o (void) {;}
/*
 *  fx, ik, etc makes no difference to thorax, coxa, and trocanter (at this time)
 *
 */

char         /*--: establish thorax endpoint -------------[ leaf   [ ------ ]-*/
ykine__thor        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   double      x,  y,  z, xz;          /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz, cxz;        /* coordintates                        */
   double      sl, fl;                 /* lengths in xz, seg, and full        */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(save basics)-----------------*/
   l     =  s_leg [YKINE_THOR].l;                        /* set during ykine__clear */
   d     =  s_leg [YKINE_THOR].cd   =  s_leg [YKINE_THOR].d;
   v     =  s_leg [YKINE_THOR].v    =  s_leg [YKINE_THOR].cv  =  0.0f;
   h     =  s_leg [YKINE_THOR].h    =  s_leg [YKINE_THOR].ch  =  d * DEG2RAD;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)-------------*/
   x     =  s_leg [YKINE_THOR].x    =  l * cos (h);
   z     =  s_leg [YKINE_THOR].z    = -l * sin (h);
   xz    =  s_leg [YKINE_THOR].xz   =  sqrt (( x *  x) + ( z *  z));
   y     =  s_leg [YKINE_THOR].y    =  0.0f;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(cumulatives)--------------------*/
   ykine__cums (YKINE_THOR);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine__coxa        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz, cxz;        /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(save basics)-----------------*/
   l     =  s_leg [YKINE_COXA].l;
   d     =  s_leg [YKINE_COXA].cd   =  s_leg [YKINE_THOR].d;
   v     =  s_leg [YKINE_COXA].cv   =  0.0f;
   h     =  s_leg [YKINE_COXA].ch   =  d * DEG2RAD;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)-------------*/
   x     =  s_leg [YKINE_COXA].x    =  l * cos (h);
   z     =  s_leg [YKINE_COXA].z    = -l * sin (h);
   xz    =  s_leg [YKINE_COXA].xz   =  sqrt (( x *  x) + ( z *  z));
   y     =  s_leg [YKINE_COXA].y    =  0.0f;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(cumulatives)--------------------*/
   ykine__cums (YKINE_COXA);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine__troc        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(save basics)-----------------*/
   s_leg [YKINE_TROC].cd   =  s_leg [YKINE_COXA].cd;
   DEBUG_YKINE_CALC   yLOG_note    ("save basics");
   /*---(cumulatives)--------------------*/
   ykine__cums_copy (YKINE_TROC);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     calculation drivers                      ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVERS_________________o (void) {;}

char
ykine__femu        (float a_deg)
{  /*---(design notes)-------------------*/
   /*
    *   expand degree range by +/-1 degree to accomodate IK appoximations
    *
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz, cxz;        /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   double      x_forgive   = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_double  ("a_deg"     , a_deg);
   /*---(defense)------------------------*/
   if (s_meth == YKINE_IK)   x_forgive = 0.5;
   DEBUG_YKINE_CALC   yLOG_double  ("IK forgive", x_forgive);
   DEBUG_YKINE_CALC   yLOG_double  ("min deg"   , g_seg_data [YKINE_FEMU].min);
   --rce;  if (a_deg <  g_seg_data [YKINE_FEMU].min - x_forgive) {
      DEBUG_YKINE_CALC   yLOG_note    ("degree is less than minimum");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_CALC   yLOG_double  ("max deg"   , g_seg_data [YKINE_FEMU].max);
   --rce;  if (a_deg >  g_seg_data [YKINE_FEMU].max + x_forgive) {
      DEBUG_YKINE_CALC   yLOG_note    ("degree is greater than maximum");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(save basics)--------------------*/
   l    =  s_leg [YKINE_FEMU].l;
   s_leg [YKINE_FEMU].d    =  a_deg;
   s_leg [YKINE_FEMU].v    =  0.0f;
   s_leg [YKINE_FEMU].h    =  a_deg * DEG2RAD;
   /*---(calc basics)--------------------*/
   d    =  s_leg [YKINE_FEMU].cd   =  s_leg [YKINE_TROC].cd + a_deg;
   v    =  s_leg [YKINE_FEMU].cv   =  0.0f;
   h    =  s_leg [YKINE_FEMU].ch   =  d * DEG2RAD;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)----------------*/
   x    =  s_leg [YKINE_FEMU].x    =  l * cos (h);
   z    =  s_leg [YKINE_FEMU].z    = -l * sin (h);
   xz   =  s_leg [YKINE_FEMU].xz   =  sqrt (( x *  x) + ( z *  z));
   y    =  s_leg [YKINE_FEMU].y    =  0.0f;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(cumulatives)--------------------*/
   ykine__cums (YKINE_FEMU);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine__lowr        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz, cxz;        /* coordintates                        */
   double      cv, ch;                 /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(cumulatives)--------------------*/
   cx   =  s_leg [YKINE_LOWR].cx   =  s_leg [YKINE_TARG].cx;
   cz   =  s_leg [YKINE_LOWR].cz   =  s_leg [YKINE_TARG].cz;
   cxz  =  s_leg [YKINE_LOWR].cxz  =  s_leg [YKINE_TARG].cxz;
   cy   =  s_leg [YKINE_LOWR].cy   =  s_leg [YKINE_TARG].cy + s_leg [YKINE_FOOT].l;
   fl   =  s_leg [YKINE_LOWR].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_YKINE_CALC   yLOG_complex ("target"   , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz, %6.1ffl",  cx, cz, cy, cxz, fl);
   /*---(angles)-------------------------*/
   ykine__chcv             (YKINE_LOWR);
   /*---(patella/tibia calcs)------------*/
   x    =  s_leg [YKINE_LOWR].x    =  cx - s_leg [YKINE_FEMU].cx;
   z    =  s_leg [YKINE_LOWR].z    =  cz - s_leg [YKINE_FEMU].cz;
   xz   =  s_leg [YKINE_LOWR].xz   =  sqrt  ((x * x) + (z * z));
   y    =  s_leg [YKINE_LOWR].y    =  cy - s_leg [YKINE_FEMU].cy;
   s_leg [YKINE_LOWR].sl   =  sqrt  ((x * x) + (z * z) + (y * y));
   DEBUG_YKINE_CALC   yLOG_complex ("lower"    , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz, %6.1fsl",  x,  z,  y, xz, sl);
   s_leg [YKINE_LOWR].l    =  s_leg [YKINE_LOWR].sl;
   s_leg [YKINE_LOWR].v    =  atan2  (-y , xz);
   s_leg [YKINE_LOWR].h    =  atan2  ( z ,  x);
   s_leg [YKINE_LOWR].d    =  s_leg [YKINE_LOWR].v * RAD2DEG  - s_leg [YKINE_COXA].cv;
   DEBUG_YKINE_CALC   yLOG_complex ("radians"  , "%6.3fv , %6.3fh , %6.1fd ", s_leg [YKINE_LOWR].v, s_leg [YKINE_LOWR].h, s_leg [YKINE_LOWR].d);
   /*---(check tibia orientation)--------*/
   d    = (atan2 (-z, x) * RAD2DEG);
   DEBUG_YKINE_CALC   yLOG_double  ("atan2 d"   , d);
   d   -= s_leg [YKINE_THOR].d;
   DEBUG_YKINE_CALC   yLOG_double  ("- thor d"  , d);
   /*----(adjust direction)--------------*/
   while (d  <=  -360.0) d += 360.0;
   while (d  >=   360.0) d -= 360.0;
   DEBUG_YKINE_CALC   yLOG_double  ("fixed d"   , d);
   /*----(adjust direction)--------------*/
   if      (d >  270.0) { d = d - 360.0;  s_leg [YKINE_PATE].u     = '-'; }
   if      (d >   90.0) { d = d - 180.0;  s_leg [YKINE_PATE].u     = 'y'; }
   else if (d >    0.0) { d = d        ;  s_leg [YKINE_PATE].u     = '-'; }
   else if (d >  -90.0) { d = d        ;  s_leg [YKINE_PATE].u     = '-'; }
   else if (d > -270.0) { d = 180.0 + d;  s_leg [YKINE_PATE].u     = 'y'; }
   else                 { d = 360.0 + d;  s_leg [YKINE_PATE].u     = '-'; }     
   DEBUG_YKINE_CALC   yLOG_double  ("another d" , d);
   DEBUG_YKINE_CALC   yLOG_char    ("under pate", s_leg [YKINE_PATE].u);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine__pate        (float a_deg)
{  /*---(design notes)-------------------*/
   /*
    *   expand degree range by +/-1 degree to accomodate IK appoximations
    *
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz, cxz;        /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   double      x_forgive   = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_double  ("a_deg"     , a_deg);
   /*---(defense)------------------------*/
   if (s_meth == YKINE_IK)   x_forgive = 0.5;
   DEBUG_YKINE_CALC   yLOG_double  ("IK forgive", x_forgive);
   DEBUG_YKINE_CALC   yLOG_double  ("min deg"   , g_seg_data [YKINE_PATE].min);
   --rce;  if (a_deg <  g_seg_data [YKINE_PATE].min - x_forgive) {
      DEBUG_YKINE_CALC   yLOG_note    ("degree is less than minimum");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_CALC   yLOG_double  ("max deg"   , g_seg_data [YKINE_PATE].max);
   --rce;  if (a_deg >  g_seg_data [YKINE_PATE].max + x_forgive) {
      DEBUG_YKINE_CALC   yLOG_note    ("degree is greater than maximum");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(save basics)--------------------*/
   l    =  s_leg [YKINE_PATE].l;
   s_leg [YKINE_PATE].d    =  a_deg;
   s_leg [YKINE_PATE].v    =  a_deg * DEG2RAD; 
   s_leg [YKINE_PATE].h    =  0.0f;
   /*---(calc basics)--------------------*/
   d    =  s_leg [YKINE_PATE].cd   =  a_deg;
   v    =  s_leg [YKINE_PATE].cv   = -s_leg [YKINE_PATE].v;
   h    =  s_leg [YKINE_PATE].ch   =  s_leg [YKINE_FEMU].ch;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  s_leg [YKINE_PATE].y    = -l * sin (v);
   xz   =  s_leg [YKINE_PATE].xz   =  sqrt (( l *  l) - ( y *  y));
   x    =  s_leg [YKINE_PATE].x    =  xz * cos (h);
   z    =  s_leg [YKINE_PATE].z    = -xz * sin (h);
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(cumulatives)--------------------*/
   ykine__cums (YKINE_PATE);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine__tibi        (float a_deg)
{  /*---(design notes)-------------------*/
   /*
    *   expand degree range by +/-1 degree to accomodate IK appoximations
    *
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz, cxz;        /* coordintates                        */
   double      cv, ch;                 /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   double      x_forgive   =  0.0;
   double      x_cum       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_double  ("a_deg"     , a_deg);
   /*---(defense)------------------------*/
   if (s_meth == YKINE_IK)   x_forgive = 0.5;
   DEBUG_YKINE_CALC   yLOG_double  ("IK forgive", x_forgive);
   DEBUG_YKINE_CALC   yLOG_double  ("min deg"   , g_seg_data [YKINE_TIBI].min);
   --rce;  if (a_deg <  g_seg_data [YKINE_TIBI].min - x_forgive) {
      DEBUG_YKINE_CALC   yLOG_note    ("degree is less than minimum");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_CALC   yLOG_double  ("max deg"   , g_seg_data [YKINE_TIBI].max);
   --rce;  if (a_deg >  g_seg_data [YKINE_TIBI].max + x_forgive) {
      DEBUG_YKINE_CALC   yLOG_note    ("degree is greater than maximum");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(save basics)--------------------*/
   l    =  s_leg [YKINE_TIBI].l;
   d    =  s_leg [YKINE_TIBI].d    =  a_deg;
   v    =  s_leg [YKINE_TIBI].v    = (90.0 - a_deg) * DEG2RAD; 
   h    =  s_leg [YKINE_TIBI].h    =  0.0f;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc basics)--------------------*/
   d    =  s_leg [YKINE_TIBI].cd  +=  s_leg [YKINE_PATE].d;
   v    =  s_leg [YKINE_TIBI].cv   =  s_leg [YKINE_TIBI].v - s_leg [YKINE_PATE].cv;
   h    =  s_leg [YKINE_TIBI].ch   =  s_leg [YKINE_PATE].ch;
   DEBUG_YKINE_CALC   yLOG_complex ("cums"     , "%6.1fcd, %6.3fcv, %6.3fch", d, v, h);
   /*---(calc end coords)----------------*/
   y    =  s_leg [YKINE_TIBI].y    = -l * sin (v);
   xz   =  s_leg [YKINE_TIBI].xz   =  sqrt (( l *  l) - ( y *  y));
   if (s_leg [YKINE_TIBI].cv > 90.0 * DEG2RAD)  xz *= -1.0;
   x_cum = s_leg [YKINE_COXA].cv - s_leg [YKINE_PATE].cv;
   DEBUG_YKINE_CALC   yLOG_complex ("femu"     , "%6.1fd , %6.3fv , %6.3fcv", s_leg [YKINE_FEMU].d, s_leg [YKINE_FEMU].v, s_leg [YKINE_FEMU].cv);
   DEBUG_YKINE_CALC   yLOG_complex ("pate"     , "%6.1fd , %6.3fv , %6.3fcv", s_leg [YKINE_PATE].d, s_leg [YKINE_PATE].v, s_leg [YKINE_PATE].cv);
   DEBUG_YKINE_CALC   yLOG_complex ("tibi"     , "%6.1fd , %6.3fv , %6.3fcv", s_leg [YKINE_TIBI].d, s_leg [YKINE_TIBI].v, s_leg [YKINE_TIBI].cv);
   x    =  s_leg [YKINE_TIBI].x    =  xz * cos (h);
   z    =  s_leg [YKINE_TIBI].z    = -xz * sin (h);
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz, %6.1fsl",  x,  z,  y, xz, sl);
   /*---(cumulatives)--------------------*/
   ykine__cums (YKINE_TIBI);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     endpoint body parts                      ----===*/
/*====================------------------------------------====================*/
static void      o___ENDS____________________o (void) {;}
/*
 *  fx, ik, etc makes no difference to the thorax and coxa (at this time)
 *
 */

char
ykine__meta        (void)
{
   /*---(calc basics)-----------------*/
   s_leg [YKINE_META].d    =  0.0;
   s_leg [YKINE_META].cd   =  s_leg [YKINE_TIBI].cd;
   /*---(cumulatives)--------------------*/
   ykine__cums_copy (YKINE_META);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine__tars        (void)
{
   /*---(calc basics)-----------------*/
   s_leg [YKINE_TARS].d    =  0.0;
   s_leg [YKINE_TARS].cd   =  s_leg [YKINE_META].cd;
   /*---(cumulatives)--------------------*/
   ykine__cums_copy (YKINE_TARS);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine__foot        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   double      l           =  0.0;
   double      d,  v,  h;              /* angles                              */
   double      x,  y,  z, xz;          /* coordintates                        */
   double      cx, cy, cz, cxz;        /* cums                                */
   double      ch, cv;                 /* cums                                */
   double      sl, fl;                 /* lengths in seg, and full            */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(calc basics)-----------------*/
   l    =  s_leg [YKINE_FOOT].l;
   d    =  s_leg [YKINE_FOOT].d = -(s_leg [YKINE_PATE].d + s_leg [YKINE_TIBI].d);
   v    =  s_leg [YKINE_FOOT].v = s_leg [YKINE_FOOT].d * DEG2RAD;
   h    =  s_leg [YKINE_FOOT].h = 0.0;
   s_leg [YKINE_FOOT].cd   =  0.0;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)----------------*/
   x    =  s_leg [YKINE_FOOT].x  = 0.0;
   z    =  s_leg [YKINE_FOOT].z  = 0.0;
   xz   =  s_leg [YKINE_FOOT].xz = 0.0;
   y    =  s_leg [YKINE_FOOT].y  =  -l;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(cumulatives)--------------------*/
   ykine__cums (YKINE_FOOT);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      foreward kinematics                     ----===*/
/*====================------------------------------------====================*/
static void      o___FORWARD_________________o (void) {;}

char       /*----: set the leg target ----------------------------------------*/
ykine__FK_targ     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   double      ch, cv, fl;             /*                                     */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(get end-points)-----------------*/
   x  =  s_leg [YKINE_FOOT].cx;
   z  =  s_leg [YKINE_FOOT].cz;
   y  =  s_leg [YKINE_FOOT].cy;
   xz =  s_leg [YKINE_FOOT].cxz;
   fl =  s_leg [YKINE_FOOT].fl;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(save original)------------------*/
   s_leg [YKINE_ORIG].x   =  s_leg [YKINE_ORIG].cx  =  x;
   s_leg [YKINE_ORIG].z   =  s_leg [YKINE_ORIG].cz  =  z;
   s_leg [YKINE_ORIG].y   =  s_leg [YKINE_ORIG].cy  =  y;
   s_leg [YKINE_ORIG].xz  =  s_leg [YKINE_ORIG].cxz =  xz;
   s_leg [YKINE_ORIG].fl  =  s_leg [YKINE_ORIG].fl  =  fl;
   /*---(save target)--------------------*/
   s_leg [YKINE_TARG].x   =  s_leg [YKINE_TARG].cx  =  x;
   s_leg [YKINE_TARG].z   =  s_leg [YKINE_TARG].cz  =  z;
   s_leg [YKINE_TARG].y   =  s_leg [YKINE_TARG].cy  =  y;
   s_leg [YKINE_TARG].xz  =  s_leg [YKINE_TARG].cxz =  xz;
   s_leg [YKINE_TARG].fl  =  s_leg [YKINE_TARG].fl  =  fl;
   /*---(angles)-------------------------*/
   ykine__chcv             (YKINE_TARG);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     inverse kinematics                       ----===*/
/*====================------------------------------------====================*/
static void      o___INVERSE_________________o (void) {;};

char       /*----: set the leg target ----------------------------------------*/
ykine__IK_targ     (float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   double      ch, cv, fl;             /*                                     */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_double  ("x"         , a_x);
   DEBUG_YKINE_CALC   yLOG_double  ("z"         , a_z);
   DEBUG_YKINE_CALC   yLOG_double  ("y"         , a_y);
   /*---(save original target)-----------*/
   x    =  s_leg [YKINE_ORIG].x   =  s_leg [YKINE_ORIG].cx  = a_x;
   z    =  s_leg [YKINE_ORIG].z   =  s_leg [YKINE_ORIG].cz  = a_z;
   y    =  s_leg [YKINE_ORIG].y   =  s_leg [YKINE_ORIG].cy  = a_y;
   xz   =  s_leg [YKINE_ORIG].xz  =  s_leg [YKINE_ORIG].cxz = sqrt  ((x * x) + (z * z));
   fl   =  s_leg [YKINE_ORIG].sl  =  s_leg [YKINE_ORIG].fl  = sqrt  ((x * x) + (z * z) + (y * y));
   ykine__chcv             (YKINE_ORIG);
   DEBUG_YKINE_CALC   yLOG_complex ("original" , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(save adapted target)------------*/
   s_leg [YKINE_TARG].x   =  s_leg [YKINE_TARG].cx  = x;
   s_leg [YKINE_TARG].z   =  s_leg [YKINE_TARG].cz  = z;
   s_leg [YKINE_TARG].y   =  s_leg [YKINE_TARG].cy  = y;
   s_leg [YKINE_TARG].xz  =  s_leg [YKINE_TARG].cxz = xz;
   s_leg [YKINE_TARG].sl  =  s_leg [YKINE_TARG].fl  = fl;
   ykine__chcv             (YKINE_TARG);
   DEBUG_YKINE_CALC   yLOG_complex ("targent"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: set the leg target ----------------------------------------*/
ykine__IK_adapt         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      xz, fl;                 /* xz and xzy plane length             */
   double      x_degs      = 0.0;
   double      x_rads      = 0.0;
   double      x_dist      = 0.0;
   double      x_orig      = 0.0;
   double      x_vert      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(test and set)-------------------*/
   x       =  s_leg [YKINE_TARG].x;
   z       =  s_leg [YKINE_TARG].z;
   y       =  s_leg [YKINE_TARG].y;
   xz      =  s_leg [YKINE_TARG].xz;
   fl      =  s_leg [YKINE_TARG].fl;
   DEBUG_YKINE_CALC   yLOG_complex ("zero"     , "%8.2fx , %8.2fz , %8.2fy",  myKINE.s_xcenter,  myKINE.s_zcenter,  myKINE.s_ycenter);
   DEBUG_YKINE_CALC   yLOG_complex ("orient"   , "%8.2ff , %8.2fp , %8.2ft",  myKINE.s_yaw    ,  myKINE.s_pitch  ,  myKINE.s_roll   );
   DEBUG_YKINE_CALC   yLOG_complex ("original" , "%8.2fx , %8.2fz , %8.2fy , %8.2fxz, %8.2ffl",  x,  z,  y, xz, fl);
   /*---(yaw)----------------------------*/
   DEBUG_YKINE_CALC  yLOG_note    ("yaw calcs");
   x_dist  = sqrt  ((x * x) + (z * z));
   DEBUG_YKINE_CALC  yLOG_double  ("x_dist"    , x_dist);
   x_rads  = - atan2 (z , x);
   DEBUG_YKINE_CALC  yLOG_double  ("x_rads"    , x_rads);
   x_rads -= (myKINE.s_yaw * DEG2RAD);
   DEBUG_YKINE_CALC  yLOG_double  ("new x_rads", x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_CALC  yLOG_double  ("new x_degs", x_degs);
   x  =   x_dist * cos (x_rads);
   z  = -(x_dist * sin (x_rads));
   DEBUG_YKINE_CALC  yLOG_double  ("new x", x);
   DEBUG_YKINE_CALC  yLOG_double  ("new z", z);
   /*---(rotate)-------------------------*/
   DEBUG_YKINE_CALC  yLOG_note    ("rotate calcs");
   x_dist  = x;
   DEBUG_YKINE_CALC  yLOG_double  ("x_dist"    , x_dist);
   x_rads  = -myKINE.s_roll * DEG2RAD;
   DEBUG_YKINE_CALC  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_CALC  yLOG_double  ("x_degs"    , x_degs);
   x_vert  = x_dist * sin (x_rads);
   DEBUG_YKINE_CALC  yLOG_double  ("x_vert"    , x_vert);
   x_vert  = y + x_vert;
   DEBUG_YKINE_CALC  yLOG_double  ("new x_vert", x_vert);
   x  = x - (y * sin (x_rads));
   y  = x_vert * cos (x_rads);
   DEBUG_YKINE_CALC  yLOG_double  ("new x", x);
   DEBUG_YKINE_CALC  yLOG_double  ("new y", y);
   /*---(pitch)--------------------------*/
   DEBUG_YKINE_CALC  yLOG_note    ("pitch calcs");
   x_dist  = z;
   DEBUG_YKINE_CALC  yLOG_double  ("x_dist"    , x_dist);
   x_rads  =  myKINE.s_pitch * DEG2RAD;
   DEBUG_YKINE_CALC  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_CALC  yLOG_double  ("x_degs"    , x_degs);
   x_vert  = x_dist * sin (x_rads);
   DEBUG_YKINE_CALC  yLOG_double  ("x_vert"    , x_vert);
   x_vert  = y + x_vert;
   DEBUG_YKINE_CALC  yLOG_double  ("new x_vert", x_vert);
   z  = z - (y * sin (x_rads));
   y  = x_vert * cos (x_rads);
   DEBUG_YKINE_CALC  yLOG_double  ("new z", z);
   DEBUG_YKINE_CALC  yLOG_double  ("new y", y);
   /*---(zero-point)---------------------*/
   x   -=  myKINE.s_xcenter;
   DEBUG_YKINE_CALC  yLOG_complex ("x-zeroed"  , "zero = %8.2lf, revs = %8.2lf", myKINE.s_xcenter, x);
   z   -=  myKINE.s_zcenter;
   DEBUG_YKINE_CALC  yLOG_complex ("z-zeroed"  , "zero = %8.2lf, revs = %8.2lf", myKINE.s_zcenter, z);
   y   -=  myKINE.s_ycenter;
   DEBUG_YKINE_CALC  yLOG_complex ("y-zeroed"  , "zero = %8.2lf, revs = %8.2lf", myKINE.s_ycenter, y);
   /*---(save)---------------------------*/
   s_leg [YKINE_TARG].x   =  s_leg [YKINE_TARG].cx  =  x;
   s_leg [YKINE_TARG].z   =  s_leg [YKINE_TARG].cz  =  z;
   s_leg [YKINE_TARG].y   =  s_leg [YKINE_TARG].cy  =  y;
   xz      = s_leg [YKINE_TARG].xz  =  s_leg [YKINE_TARG].cxz = sqrt  ((x * x) + (z * z));
   fl      = s_leg [YKINE_TARG].sl  =  s_leg [YKINE_TARG].fl  = sqrt  ((x * x) + (z * z) + (y * y));
   DEBUG_YKINE_CALC   yLOG_complex ("adapted"  , "%8.2fx , %8.2fz , %8.2fy , %8.2fxz, %8.2ffl",  x,  z,  y, xz, fl);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: isolate the leg values ------------------------------------*/
ykine__IK_femu     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        rc          =   0;      /* generic return code                 */
   double      d;                      /* degrees                             */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*----(check femur orientation)-------*/
   DEBUG_YKINE_CALC   yLOG_complex ("troc"     , "%6.1fcx, %6.1fcz, %6.1fcd", s_leg [YKINE_TROC].cx, s_leg [YKINE_TROC].cz, s_leg [YKINE_TROC].cd);
   x    =  s_leg [YKINE_TARG].cx          - s_leg [YKINE_TROC].cx;
   z    =  s_leg [YKINE_TARG].cz          - s_leg [YKINE_TROC].cz;
   xz   =  sqrt  ((x * x) + (z * z));
   d    = (atan2 (-z, x) * RAD2DEG) - s_leg [YKINE_TROC].cd;
   DEBUG_YKINE_CALC   yLOG_complex ("femu"     , "%6.1fx , %6.1fz , %6.1fxz, %8.3fd ", x, z, xz, d);
   /*----(adjust direction)--------------*/
   while (d  <=  -360.0) d += 360.0;
   while (d  >=   360.0) d -= 360.0;
   DEBUG_YKINE_CALC   yLOG_double  ("fixed d"   , d);
   /*----(adjust direction)--------------*/
   if      (d >  270.0) { d = d - 360.0;  s_leg [YKINE_FEMU].u     = '-'; }
   if      (d >   90.0) { d = d - 180.0;  s_leg [YKINE_FEMU].u     = 'y'; }
   else if (d >    0.0) { d = d        ;  s_leg [YKINE_FEMU].u     = '-'; }
   else if (d >  -90.0) { d = d        ;  s_leg [YKINE_FEMU].u     = '-'; }
   else if (d > -270.0) { d = 180.0 + d;  s_leg [YKINE_FEMU].u     = 'y'; }
   else                 { d = 360.0 + d;  s_leg [YKINE_FEMU].u     = '-'; }     
   DEBUG_YKINE_CALC   yLOG_double  ("new d"     , d);
   DEBUG_YKINE_CALC   yLOG_char    ("under femu", s_leg [YKINE_FEMU].u);
   /*----(save)--------------------------*/
   rc   = ykine__femu     (d);
   DEBUG_YKINE_CALC   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_YKINE_CALC   yLOG_note    ("calc function failed");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}


char       /*----: isolate the leg values ------------------------------------*/
ykine__IK_pate     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        rc          =   0;      /* generic return code                 */
   double      dv;                     /* degrees of lower leg vertical       */
   double      sl;                     /* full length from femur to target    */
   double      pl;                     /* patella length                      */
   double      tl;                     /* tibia lenght                        */
   double      a;                      /* angle in radians                    */
   double      d;                      /* degrees                             */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(patella angle)------------------*/
   dv   =  s_leg [YKINE_LOWR].d;
   sl   =  s_leg [YKINE_LOWR].sl;
   pl   =  s_leg [YKINE_PATE].l;
   tl   =  s_leg [YKINE_TIBI].l;
   DEBUG_YKINE_CALC   yLOG_double  ("dv"        , dv);
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((sl * sl) + (pl * pl) - (tl * tl)) / (2.0 * sl * pl));
   if (isnan (a))    d = 0.0;
   else              d = a * RAD2DEG;
   DEBUG_YKINE_CALC   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*---(reorient as needed)-------------*/
   DEBUG_YKINE_CALC   yLOG_char    ("under femu", s_leg [YKINE_FEMU].u);
   DEBUG_YKINE_CALC   yLOG_char    ("under pate", s_leg [YKINE_PATE].u);
   DEBUG_YKINE_CALC   yLOG_double  ("lowr y"    , s_leg [YKINE_LOWR].y);
   if         (s_leg [YKINE_FEMU].u == 'y') {
      if      (s_leg [YKINE_PATE].u == 'y')   d = 180.0 - (d  + dv); /* 8.6000 */
      else                                    d =  d  - dv;
   } else {
      if      (s_leg [YKINE_LOWR].y >  0.0)   d =  dv - d;  /* fix 0.3750 */
      else if (s_leg [YKINE_PATE].u == 'y')   d =  180.0 - (dv + d); /* 7.2750 */
      else                                    d =  dv - d;  /* fix 0.0250 */
   }
   DEBUG_YKINE_CALC   yLOG_double  ("new d"     , d);
   /*----(save)--------------------------*/
   rc   = ykine__pate     (-d);
   DEBUG_YKINE_CALC   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_YKINE_CALC   yLOG_note    ("calc function failed");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: isolate the leg values ------------------------------------*/
ykine__IK_tibi     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        rc          =   0;      /* generic return code                 */
   double      dv;                     /* degrees of lower leg vertical       */
   double      sl;                     /* full length from femur to target    */
   double      pl;                     /* patella length                      */
   double      tl;                     /* tibia lenght                        */
   double      a;                      /* angle in radians                    */
   double      d;                      /* degrees                             */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(tibia angle)--------------------*/
   dv   =  s_leg [YKINE_LOWR].d;
   sl   =  s_leg [YKINE_LOWR].sl;
   pl   =  s_leg [YKINE_PATE].l;
   tl   =  s_leg [YKINE_TIBI].l;
   DEBUG_YKINE_CALC   yLOG_double  ("dv"        , dv);
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((tl * tl) + (pl * pl) - (sl * sl)  ) / (2.0 * pl * tl));
   a   -=  M_PI;
   d    =  -(a  * RAD2DEG);
   if (isnan (a))    d = 0.0;
   DEBUG_YKINE_CALC   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*----(save)--------------------------*/
   rc   = ykine__tibi     (90.0 - d);
   DEBUG_YKINE_CALC   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_YKINE_CALC   yLOG_note    ("calc function failed");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       opengl actuals                         ----===*/
/*====================------------------------------------====================*/
static void      o___OPENGL__________________o (void) {;};

char         /*--> set the opengl actual values ----------[ ------ [ ------ ]-*/
yKINE_opengl       (char a_leg, char a_seg, float a_deg, float a_x, float a_z, float a_y, float a_len)
{
   /*---(locals)-----------+-----------+-*/
   double      x_len       = 0.0;
   double      x_y         = 0.0;
   double      x, z, y, sl, fl;
   double      xz;
   double      x_full      = 2 * M_PI;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   ykine__setleg   (a_leg, YKINE_GK);
   /*---(target setting)-----------------*/
   DEBUG_YKINE_CALC   yLOG_complex ("object"    , "%1dl (%2.2s), %2ds (%4.4s), %8.3lfd", a_leg, g_leg_data [a_leg].caps, a_seg, g_seg_data [a_seg].four, a_deg);
   /*---(set cumulatives)----------------*/
   s_leg [a_seg].cx  = a_x;
   s_leg [a_seg].cz  = a_z;
   s_leg [a_seg].cy  = a_y;
   fl = s_leg [a_seg].fl  = sqrt ((a_x * a_x) + (a_z * a_z) + (a_y * a_y));
   x_len             = s_leg [a_seg].fl;
   s_leg [a_seg].cxz = sqrt ((x_len * x_len) - (a_y * a_y));
   DEBUG_YKINE_CALC   yLOG_complex ("cums"      , "%8.3lfcx, %8.3lfcz, %8.3lfcy, %8.3lffl", a_z, a_z, a_y, fl);
   /*---(set individuals)----------------*/
   s_leg [a_seg].d   = a_deg;
   s_leg [a_seg].l   = a_len;
   s_leg [a_seg].sl  = a_len;
   x  = s_leg [a_seg].x   = a_x - s_leg [a_seg - 1].cx;
   z  = s_leg [a_seg].z   = a_z - s_leg [a_seg - 1].cz;
   y  = s_leg [a_seg].y   = a_y - s_leg [a_seg - 1].cy;
   x_y               = s_leg [a_seg].y;
   if (x == 0.0 && z == 0.0)      s_leg [a_seg].xz = 0.0;
   else                           s_leg [a_seg].xz  = sqrt ((x * x) + (z * z));
   /*> s_leg [a_seg].xz  = sqrt ((a_len * a_len) - (x_y * x_y));                      <*/
   DEBUG_YKINE_CALC   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy , %8.3lfsl", x, z, y, a_len);
   /*---(calc ch)------------------------*/
   ykine__chcv (a_seg);
   /*> s_leg [a_seg].ch   = atan2 (-s_leg [a_seg].cz, s_leg [a_seg].cx);              <* 
    *> while (s_leg [a_seg].ch <  0     )  s_leg [a_seg].ch += x_full;                <* 
    *> while (s_leg [a_seg].ch >= x_full)  s_leg [a_seg].ch -= x_full;                <* 
    *> if    (s_leg [a_seg].ch >= x_full - 0.001)  s_leg [a_seg].ch = 0.0;            <*/
   /*---(calc cv)------------------------*/
   /*> s_leg [a_seg].cv   = atan2 (-s_leg [a_seg].cy, s_leg [a_seg].cxz);             <* 
    *> while (s_leg [a_seg].cv <  0     )  s_leg [a_seg].cv += x_full;                <* 
    *> while (s_leg [a_seg].cv >= x_full)  s_leg [a_seg].cv -= x_full;                <* 
    *> if    (s_leg [a_seg].cv >= x_full - 0.001)  s_leg [a_seg].cv = 0.0;            <*/
   /*---(target)-------------------------*/
   if (a_seg == YKINE_FOOT) {
      ykine__FK_targ  ();
      ykine__lowr     ();
   }
   /*---(wrapup)-------------------------*/
   ykine__unsetleg ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        full drivers                          ----===*/
/*====================------------------------------------====================*/
static void      o___MAIN____________________o (void) {;};

char         /*--> drive the leg position from angles ----[ ------ [ ------ ]-*/
yKINE_forward      (char a_leg, float a_femu, float a_pate, float a_tibi)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;      /* generic return code                 */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   ykine__setleg   (a_leg, YKINE_FK);
   ykine__wipe     ();
   /*---(fixed body)---------------------*/
   if (rc >= 0)  rc = ykine__thor     ();
   if (rc >= 0)  rc = ykine__coxa     ();
   if (rc >= 0)  rc = ykine__troc     ();
   /*---(movable)------------------------*/
   if (rc >= 0)  rc = ykine__femu     (a_femu);
   if (rc >= 0)  rc = ykine__pate     (a_pate);
   if (rc >= 0)  rc = ykine__tibi     (a_tibi);
   /*---(future)-------------------------*/
   if (rc >= 0)  rc = ykine__meta     ();
   if (rc >= 0)  rc = ykine__tars     ();
   if (rc >= 0)  rc = ykine__foot     ();
   /*---(target setting)-----------------*/
   if (rc >= 0)  rc = ykine__FK_targ  ();
   if (rc >= 0)  rc = ykine__lowr     ();
   /*---(wrapup)-------------------------*/
   ykine__unsetleg ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
ykine_inverse_detail    (char a_adapt, int a_leg, float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;      /* generic return code                 */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   ykine__setleg   (a_leg, YKINE_IK);
   ykine__wipe     ();
   /*---(target setting)-----------------*/
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_leg);
   DEBUG_YKINE_CALC   yLOG_value   ("a_x"       , a_x);
   DEBUG_YKINE_CALC   yLOG_value   ("a_z"       , a_z);
   DEBUG_YKINE_CALC   yLOG_value   ("a_y"       , a_y);
   if (rc >= 0)  rc = ykine__IK_targ  (a_x, a_z, a_y);
   if (rc >= 0 && a_adapt == 'y')  rc = ykine__IK_adapt ();
   /*---(fixed body)---------------------*/
   if (rc >= 0)  rc = ykine__thor     ();
   if (rc >= 0)  rc = ykine__coxa     ();
   if (rc >= 0)  rc = ykine__troc     ();
   /*---(movable)------------------------*/
   if (rc >= 0)  rc = ykine__IK_femu  ();
   if (rc >= 0)  rc = ykine__lowr     ();
   if (rc >= 0)  rc = ykine__IK_pate  ();
   if (rc >= 0)  rc = ykine__IK_tibi  ();
   /*---(future)-------------------------*/
   if (rc >= 0)  rc = ykine__meta     ();
   if (rc >= 0)  rc = ykine__tars     ();
   if (rc >= 0)  rc = ykine__foot     ();
   /*---(wrapup)-------------------------*/
   ykine__unsetleg ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
yKINE_adapt             (char a_leg, float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;      /* generic return code                 */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(run)----------------------------*/
   rc = ykine_inverse_detail ('y', a_leg, a_x, a_z, a_y);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
yKINE_inverse           (char a_leg, float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;      /* generic return code                 */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(run)----------------------------*/
   rc = ykine_inverse_detail ('-', a_leg, a_x, a_z, a_y);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*============================----end-of-source---============================*/
