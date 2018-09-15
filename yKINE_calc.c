/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"

const    double   DEG2RAD   = M_PI / 180.0;
const    double   RAD2DEG   = 180.0 / M_PI;


char       /*----: clear most recent kinematics working data -----------------*/
yKINE__wipe        (int  a_leg, int a_meth)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_leg);
   /*---(defense)------------------------*/
   --rce;  if (a_leg < 0 || a_leg > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);
   if (a_meth == YKINE_GK)  x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);
   /*---(clear)--------------------------*/
   for (i = 0; i < YKINE_MAX_SEGS; ++i) {
      /*---(distances)----------------------*/
      x_leg [i].fl     =   0.0;
      x_leg [i].sl     =   0.0;
      /*---(angles)-------------------------*/
      if (i > YKINE_THOR)  x_leg [i].d      =   0.0;
      x_leg [i].h      =   0.0;
      x_leg [i].v      =   0.0;
      x_leg [i].cd     =   0.0;
      x_leg [i].ch     =   0.0;
      x_leg [i].cv     =   0.0;
      /*---(flags)--------------------------*/
      x_leg [i].u      =   '-';
      /*---(coordinates)--------------------*/
      x_leg [i].x      =   0.0;
      x_leg [i].z      =   0.0;
      x_leg [i].xz     =   0.0;
      x_leg [i].y      =   0.0;
      x_leg [i].cx     =   0.0;
      x_leg [i].cz     =   0.0;
      x_leg [i].cy     =   0.0;
      /*---(control)------------------------*/
      x_leg [i].p      =   'n';
      x_leg [i].m      =   'i';
      x_leg [i].c      =   'n';
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    zero and orientation                      ----===*/
/*====================------------------------------------====================*/
static void      o___BODY____________________o (void) {;}

char         /*--: establish thorax endpoint -------------[ leaf   [ ------ ]-*/
yKINE__zero        (int   a_num)
{
}

char         /*--: establish thorax endpoint -------------[ leaf   [ ------ ]-*/
yKINE__orient      (int   a_num)
{
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
yKINE__thor        (int   a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz;             /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(kinematics)---------------------*/
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
      /*---(save basics)-----------------*/
      l     =  x_leg [YKINE_THOR].l;                        /* set during yKINE__clear */
      d     =  x_leg [YKINE_THOR].cd   =  x_leg [YKINE_THOR].d;   /* set during yKINE__clear */
      v     =  x_leg [YKINE_THOR].v    =  x_leg [YKINE_THOR].cv  =  0.0f;
      h     =  x_leg [YKINE_THOR].h    =  x_leg [YKINE_THOR].ch  =  d * DEG2RAD;
      DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
      /*---(calc end coords)-------------*/
      x     =  x_leg [YKINE_THOR].x    =  l * cos (h);
      z     =  x_leg [YKINE_THOR].z    = -l * sin (h);
      y     =  x_leg [YKINE_THOR].y    =  0.0f;
      DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
      /*---(calc cums)-------------------*/
      cx    =  x_leg [YKINE_THOR].cx   =  x_leg [YKINE_CORE].cx + x;
      cz    =  x_leg [YKINE_THOR].cz   =  x_leg [YKINE_CORE].cz + z;
      cy    =  x_leg [YKINE_THOR].cy   =  x_leg [YKINE_CORE].cy + y;
      DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
      /*---(calc extras)-----------------*/
      xz    =  x_leg [YKINE_THOR].xz   =  sqrt (( x *  x) + ( z *  z));
      sl    =  x_leg [YKINE_THOR].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
      x_leg [YKINE_THOR].cxz  =  sqrt ((cx * cx) + (cz * cz));
      fl    =  x_leg [YKINE_THOR].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
      DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
      /*---(add to leg values)-----------*/
      x_leg [YKINE_CALC].x   += x;
      x_leg [YKINE_CALC].z   += z;
      x_leg [YKINE_CALC].y   += y;
      DEBUG_YKINE_CALC   yLOG_note    ("add values to YKINE_CALC segment");
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__coxa        (int  a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz;             /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(kinematics)---------------------*/
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
      /*---(save basics)-----------------*/
      l     =  x_leg [YKINE_COXA].l;
      d     =  x_leg [YKINE_COXA].d    =  x_leg [YKINE_THOR].d;
      d     =  x_leg [YKINE_COXA].cd   =  x_leg [YKINE_THOR].d;
      v     =  x_leg [YKINE_COXA].cv   =  0.0f;
      h     =  x_leg [YKINE_COXA].ch   =  d * DEG2RAD;
      DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
      /*---(calc end coords)-------------*/
      x     =  x_leg [YKINE_COXA].x    =  l * cos (h);
      z     =  x_leg [YKINE_COXA].z    = -l * sin (h);
      y     =  x_leg [YKINE_COXA].y    =  0.0f;
      DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
      /*---(calc cums)-------------------*/
      cx    =  x_leg [YKINE_COXA].cx   =  x_leg [YKINE_THOR].cx + x;
      cz    =  x_leg [YKINE_COXA].cz   =  x_leg [YKINE_THOR].cz + z;
      cy    =  x_leg [YKINE_COXA].cy   =  x_leg [YKINE_THOR].cy + y;
      DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
      /*---(calc extras)-----------------*/
      xz    =  x_leg [YKINE_COXA].xz   =  sqrt (( x *  x) + ( z *  z));
      sl    =  x_leg [YKINE_COXA].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
      x_leg [YKINE_COXA].cxz           =  sqrt ((cx * cx) + (cz * cz));
      fl    =  x_leg [YKINE_COXA].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
      DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
      /*---(add to leg values)-----------*/
      x_leg [YKINE_CALC].x   += x;
      x_leg [YKINE_CALC].z   += z;
      x_leg [YKINE_CALC].y   += y;
      DEBUG_YKINE_CALC   yLOG_note    ("add values to YKINE_CALC segment");
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__troc        (int  a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(kinematics)---------------------*/
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
      /*---(save basics)-----------------*/
      x_leg [YKINE_TROC].d    =  x_leg [YKINE_COXA].cd;
      x_leg [YKINE_TROC].cd   =  x_leg [YKINE_COXA].cd;
      x_leg [YKINE_TROC].cv   =  x_leg [YKINE_COXA].cv;
      x_leg [YKINE_TROC].ch   =  x_leg [YKINE_COXA].ch;
      DEBUG_YKINE_CALC   yLOG_note    ("save basics");
      /*---(calc cums)-------------------*/
      x_leg [YKINE_TROC].cx   =  x_leg [YKINE_COXA].cx;
      x_leg [YKINE_TROC].cz   =  x_leg [YKINE_COXA].cz;
      x_leg [YKINE_TROC].cy   =  x_leg [YKINE_COXA].cy;
      DEBUG_YKINE_CALC   yLOG_note    ("save endpoints");
      /*---(calc extras)-----------------*/
      x_leg [YKINE_TROC].cxz  =  x_leg [YKINE_COXA].cxz;
      x_leg [YKINE_TROC].fl   =  x_leg [YKINE_COXA].fl; 
      DEBUG_YKINE_CALC   yLOG_note    ("save lengths");
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     calculation drivers                      ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVERS_________________o (void) {;}


char
yKINE__femu        (int  a_num, double a_deg, int a_meth)
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
   double      cx, cy, cz;             /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   double      x_forgive   = 0.0;
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   DEBUG_YKINE_CALC   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_YKINE_CALC   yLOG_info    ("a_meth"    , (a_meth == YKINE_FK) ? "FK" : "IK");
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_meth == YKINE_IK)   x_forgive = 0.5;
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
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [YKINE_FEMU].l;
   x_leg [YKINE_FEMU].d    =  a_deg;
   x_leg [YKINE_FEMU].v    =  0.0f;
   x_leg [YKINE_FEMU].h    =  a_deg * DEG2RAD;
   /*---(calc basics)--------------------*/
   d    =  x_leg [YKINE_FEMU].cd   =  x_leg [YKINE_TROC].cd + a_deg;
   v    =  x_leg [YKINE_FEMU].cv   =  0.0f;
   h    =  x_leg [YKINE_FEMU].ch   =  d * DEG2RAD;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)----------------*/
   x    =  x_leg [YKINE_FEMU].x    =  l * cos (h);
   z    =  x_leg [YKINE_FEMU].z    = -l * sin (h);
   y    =  x_leg [YKINE_FEMU].y    =  0.0f;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [YKINE_FEMU].cx   =  x_leg [YKINE_TROC].cx + x;
   cz   =  x_leg [YKINE_FEMU].cz   =  x_leg [YKINE_TROC].cz + z;
   cy   =  x_leg [YKINE_FEMU].cy   =  x_leg [YKINE_TROC].cy + y;
   DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   xz   =  x_leg [YKINE_FEMU].xz   =  sqrt (( x *  x) + ( z *  z));
   sl   =  x_leg [YKINE_FEMU].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [YKINE_FEMU].cxz          =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [YKINE_FEMU].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [YKINE_CALC].x   += x;
   x_leg [YKINE_CALC].z   += z;
   x_leg [YKINE_CALC].y   += y;
   DEBUG_YKINE_CALC   yLOG_note    ("add values to YKINE_CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__lowr        (int  a_num, int a_meth)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      l;                      /* length                              */
   double      d;                      /* degrees                             */
   double      h,  v;                  /* horz and vert angles in radians     */
   double      cx, cy, cz;             /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   DEBUG_YKINE_CALC   yLOG_value   ("a_meth"    , a_meth);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_GK)  x_leg = ((tSEG *) gk) + (a_num * YKINE_MAX_SEGS);
   /*---(cumulatives)--------------------*/
   x    = x_leg [YKINE_LOWR].cx   =  x_leg [YKINE_TARG].cx;
   z    = x_leg [YKINE_LOWR].cz   =  x_leg [YKINE_TARG].cz;
   y    = x_leg [YKINE_LOWR].cy   =  x_leg [YKINE_TARG].cy;
   fl   = x_leg [YKINE_LOWR].fl   =  sqrt  (( x *  x) + ( z *  z) + ( y *  y));
   x_leg [YKINE_LOWR].cxz  =  x_leg [YKINE_TARG].cxz;
   x_leg [YKINE_LOWR].cv   =  x_leg [YKINE_TARG].cv;
   x_leg [YKINE_LOWR].ch   =  x_leg [YKINE_TARG].ch;
   x_leg [YKINE_LOWR].cd   =  x_leg [YKINE_TARG].cd;
   /*---(patella/tibia calcs)------------*/
   x    =  x_leg [YKINE_LOWR].x    =  x_leg [YKINE_TARG].cx - x_leg [YKINE_FEMU].cx;
   z    =  x_leg [YKINE_LOWR].z    =  x_leg [YKINE_TARG].cz - x_leg [YKINE_FEMU].cz;
   y    =  x_leg [YKINE_LOWR].y    =  x_leg [YKINE_TARG].cy - x_leg [YKINE_FEMU].cy;
   xz   =  x_leg [YKINE_LOWR].xz   =  sqrt  (( x *  x) + ( z *  z));
   DEBUG_YKINE_CALC   yLOG_complex ("lower"    , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   x_leg [YKINE_LOWR].sl   =  sqrt  (( x *  x) + ( z *  z) + ( y *  y));
   x_leg [YKINE_LOWR].l    =  x_leg [YKINE_LOWR].sl;
   x_leg [YKINE_LOWR].v    =  atan2  (-y , xz);
   x_leg [YKINE_LOWR].h    =  atan2  ( z ,  x);
   x_leg [YKINE_LOWR].d    =  x_leg [YKINE_LOWR].v * RAD2DEG;
   DEBUG_YKINE_CALC   yLOG_complex ("radians"  , "%6.3fv , %6.3fh , %6.1fd ", x_leg [YKINE_LOWR].v, x_leg [YKINE_LOWR].h, x_leg [YKINE_LOWR].d);
   /*---(check tibia orientation)--------*/
   d    = (atan2 (-z, x) * RAD2DEG) - x_leg [YKINE_COXA].d;
   /*----(adjust direction)--------------*/
   while (d  <=  -360.0) d += 360.0;
   while (d  >=   360.0) d -= 360.0;
   DEBUG_YKINE_CALC   yLOG_double  ("fixed d"   , d);
   /*----(adjust direction)--------------*/
   if      (d >  270.0) { d = d - 360.0;  x_leg [YKINE_PATE].u     = '-'; }
   if      (d >   90.0) { d = d - 180.0;  x_leg [YKINE_PATE].u     = 'y'; }
   else if (d >    0.0) { d = d        ;  x_leg [YKINE_PATE].u     = '-'; }
   else if (d >  -90.0) { d = d        ;  x_leg [YKINE_PATE].u     = '-'; }
   else if (d > -270.0) { d = 180.0 + d;  x_leg [YKINE_PATE].u     = 'y'; }
   else                 { d = 360.0 + d;  x_leg [YKINE_PATE].u     = '-'; }     
   DEBUG_YKINE_CALC   yLOG_double  ("another d" , d);
   DEBUG_YKINE_CALC   yLOG_char    ("under pate", x_leg [YKINE_PATE].u);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__pate        (int  a_num, double a_deg, int a_meth)
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
   double      cx, cy, cz;             /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   double      x_forgive   = 0.0;
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   DEBUG_YKINE_CALC   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_YKINE_CALC   yLOG_value   ("a_meth"    , a_meth);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_meth == YKINE_IK)   x_forgive = 0.5;
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
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [YKINE_PATE].l;
   x_leg [YKINE_PATE].d    =  a_deg;
   x_leg [YKINE_PATE].v    =  a_deg * DEG2RAD; 
   x_leg [YKINE_PATE].h    =  0.0f;
   /*---(calc basics)--------------------*/
   d    =  x_leg [YKINE_PATE].cd   =  a_deg;
   v    =  x_leg [YKINE_PATE].cv   = -x_leg [YKINE_PATE].v;
   h    =  x_leg [YKINE_PATE].ch   =  x_leg [YKINE_FEMU].ch;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  x_leg [YKINE_PATE].y    = -l * sin (v);
   xz   =  x_leg [YKINE_PATE].xz   =  sqrt (( l *  l) - ( y *  y));
   x    =  x_leg [YKINE_PATE].x    =  xz * cos (h);
   z    =  x_leg [YKINE_PATE].z    = -xz * sin (h);
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [YKINE_PATE].cx   =  x_leg [YKINE_FEMU].cx + x;
   cz   =  x_leg [YKINE_PATE].cz   =  x_leg [YKINE_FEMU].cz + z;
   cy   =  x_leg [YKINE_PATE].cy   =  x_leg [YKINE_FEMU].cy + y;
   DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   sl   =  x_leg [YKINE_PATE].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [YKINE_PATE].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [YKINE_PATE].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fsl, %6.1ffl", sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [YKINE_CALC].x   += x;
   x_leg [YKINE_CALC].z   += z;
   x_leg [YKINE_CALC].y   += y;
   DEBUG_YKINE_CALC   yLOG_note    ("add values to YKINE_CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__tibi        (int  a_num, double a_deg, int a_meth)
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
   double      cx, cy, cz;             /* coordintates                        */
   double      xz, sl, fl;             /* lengths in xz, seg, and full        */
   double      x_forgive   =  0.0;
   tSEG       *x_leg       = NULL;
   double      x_cum       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   DEBUG_YKINE_CALC   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_YKINE_CALC   yLOG_value   ("a_meth"    , a_meth);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_meth == YKINE_IK)   x_forgive = 0.5;
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
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [YKINE_TIBI].l;
   x_leg [YKINE_TIBI].d    =  a_deg;
   x_leg [YKINE_TIBI].v    = (90.0 - a_deg) * DEG2RAD; 
   x_leg [YKINE_TIBI].h    =  0.0f;
   /*---(calc basics)--------------------*/
   d    =  x_leg [YKINE_TIBI].cd   =  x_leg [YKINE_TIBI].d;
   v    =  x_leg [YKINE_TIBI].cv   =  x_leg [YKINE_PATE].cv + x_leg [YKINE_TIBI].v;
   h    =  x_leg [YKINE_TIBI].ch   =  x_leg [YKINE_PATE].ch;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  x_leg [YKINE_TIBI].y    = -l * sin (v);
   xz   =  x_leg [YKINE_TIBI].xz   =  sqrt (( l *  l) - ( y *  y));
   if (x_leg [YKINE_TIBI].cv > 90.0 * DEG2RAD)  xz *= -1.0;
   x_cum = x_leg [YKINE_COXA].cv - x_leg [YKINE_PATE].cv;
   DEBUG_YKINE_CALC   yLOG_complex ("verts"    , "femu %6.1fd/%6.3fv/%6.3fcv, pate %6.1fd/%6.3fv/%6.3fcv, tibi %6.1fd/%6.3fv/%6.3fcv", x_leg [YKINE_FEMU].d, x_leg [YKINE_FEMU].v, x_leg [YKINE_FEMU].cv, x_leg [YKINE_PATE].d, x_leg [YKINE_PATE].v, x_leg [YKINE_PATE].cv, x_leg [YKINE_TIBI].d, x_leg [YKINE_TIBI].v, x_leg [YKINE_TIBI].cv);
   x    =  x_leg [YKINE_TIBI].x    =  xz * cos (h);
   z    =  x_leg [YKINE_TIBI].z    = -xz * sin (h);
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [YKINE_TIBI].cx   =  x_leg [YKINE_PATE].cx + x;
   cz   =  x_leg [YKINE_TIBI].cz   =  x_leg [YKINE_PATE].cz + z;
   cy   =  x_leg [YKINE_TIBI].cy   =  x_leg [YKINE_PATE].cy + y;
   DEBUG_YKINE_CALC   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   sl   =  x_leg [YKINE_TIBI].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [YKINE_TIBI].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [YKINE_TIBI].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fsl, %6.1ffl", sl, fl);
   /*---(add to leg values)--------------*/
   DEBUG_YKINE_CALC   yLOG_note    ("add values to YKINE_CALC segment");
   x_leg [YKINE_CALC].x   += x;
   x_leg [YKINE_CALC].z   += z;
   x_leg [YKINE_CALC].y   += y;
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
yKINE__meta        (int  a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   tSEG       *x_leg       = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0)            return rce;
   --rce;  if (a_num > YKINE_MAX_LEGS)     return rce;
   /*---(kinematics)---------------------*/
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
      /*---(calc basics)-----------------*/
      x_leg [YKINE_META].d    =  0.0;
      x_leg [YKINE_META].cd   =  x_leg [YKINE_TIBI].cd;
      x_leg [YKINE_META].cv   =  x_leg [YKINE_TIBI].cv;
      x_leg [YKINE_META].ch   =  x_leg [YKINE_TIBI].ch;
      /*---(calc cums)-------------------*/
      x_leg [YKINE_META].cx   =  x_leg [YKINE_TIBI].cx;
      x_leg [YKINE_META].cz   =  x_leg [YKINE_TIBI].cz;
      x_leg [YKINE_META].cy   =  x_leg [YKINE_TIBI].cy;
      /*---(calc extras)-----------------*/
      x_leg [YKINE_META].cxz  =  x_leg [YKINE_TIBI].cxz;
      x_leg [YKINE_META].fl   =  x_leg [YKINE_TIBI].fl; 
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE__tars        (int  a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   tSEG       *x_leg       = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0)            return rce;
   --rce;  if (a_num > YKINE_MAX_LEGS)     return rce;
   /*---(kinematics)---------------------*/
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
      /*---(calc basics)-----------------*/
      x_leg [YKINE_TARS].d    =  0.0;
      x_leg [YKINE_TARS].cd   =  x_leg [YKINE_META].cd;
      x_leg [YKINE_TARS].cv   =  x_leg [YKINE_META].cv;
      x_leg [YKINE_TARS].ch   =  x_leg [YKINE_META].ch;
      /*---(calc cums)-------------------*/
      x_leg [YKINE_TARS].cx   =  x_leg [YKINE_META].cx;
      x_leg [YKINE_TARS].cz   =  x_leg [YKINE_META].cz;
      x_leg [YKINE_TARS].cy   =  x_leg [YKINE_META].cy;
      /*---(calc extras)-----------------*/
      x_leg [YKINE_TARS].cxz  =  x_leg [YKINE_META].cxz;
      x_leg [YKINE_TARS].fl   =  x_leg [YKINE_META].fl; 
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE__foot        (int  a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   tSEG       *x_leg       = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0)            return rce;
   --rce;  if (a_num > YKINE_MAX_LEGS)     return rce;
   /*---(kinematics)---------------------*/
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
      /*---(calc basics)-----------------*/
      x_leg [YKINE_FOOT].d    =  0.0;
      x_leg [YKINE_FOOT].cd   =  x_leg [YKINE_TARS].cd;
      x_leg [YKINE_FOOT].cv   =  x_leg [YKINE_TARS].cv;
      x_leg [YKINE_FOOT].ch   =  x_leg [YKINE_TARS].ch;
      /*---(calc cums)-------------------*/
      x_leg [YKINE_FOOT].cx   =  x_leg [YKINE_TARS].cx;
      x_leg [YKINE_FOOT].cz   =  x_leg [YKINE_TARS].cz;
      x_leg [YKINE_FOOT].cy   =  x_leg [YKINE_TARS].cy;
      /*---(calc extras)-----------------*/
      x_leg [YKINE_FOOT].cxz  =  x_leg [YKINE_TARS].cxz;
      x_leg [YKINE_FOOT].fl   =  x_leg [YKINE_TARS].fl; 
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      foreward kinematics                     ----===*/
/*====================------------------------------------====================*/
static void      o___FORWARD_________________o (void) {;}

char       /*----: set the leg target ----------------------------------------*/
yKINE__FK_targ     (int a_num, int a_meth)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   double      ch, cv, fl;             /*                                     */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_GK)  x_leg = ((tSEG *) gk) + (a_num * YKINE_MAX_SEGS);
   /*---(save cums)----------------------*/
   x  = x_leg [YKINE_TARG].x   =  x_leg [YKINE_TARG].cx  =  x_leg [YKINE_TIBI].cx;
   z  = x_leg [YKINE_TARG].z   =  x_leg [YKINE_TARG].cz  =  x_leg [YKINE_TIBI].cz;
   y  = x_leg [YKINE_TARG].y   =  x_leg [YKINE_TARG].cy  =  x_leg [YKINE_TIBI].cy;
   xz = x_leg [YKINE_TARG].xz  =  x_leg [YKINE_TARG].cxz =  x_leg [YKINE_TIBI].cxz;
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*> ch = x_leg [YKINE_TARG].h   =  x_leg [YKINE_TARG].ch  =  x_leg [YKINE_TIBI].ch;   <*/
   /*> cv = x_leg [YKINE_TARG].v   =  x_leg [YKINE_TARG].cv  =  x_leg [YKINE_TIBI].cv;   <*/
   fl = x_leg [YKINE_TARG].sl  =  x_leg [YKINE_TARG].fl  =  x_leg [YKINE_TIBI].fl;
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.3fcv, %6.3fch, %6.1ffl",  cv, ch, fl);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     inverse kinematics                       ----===*/
/*====================------------------------------------====================*/
static void      o___INVERSE_________________o (void) {;};

char       /*----: set the leg target ----------------------------------------*/
yKINE__IK_targ     (int a_num, double a_x, double a_z, double a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   double      ch, cv, fl;             /*                                     */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   DEBUG_YKINE_CALC   yLOG_double  ("x"         , a_x);
   DEBUG_YKINE_CALC   yLOG_double  ("z"         , a_z);
   DEBUG_YKINE_CALC   yLOG_double  ("y"         , a_y);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(save cums)----------------------*/
   x    =  x_leg [YKINE_TARG].x   =  x_leg [YKINE_TARG].cx  =  a_x;
   z    =  x_leg [YKINE_TARG].z   =  x_leg [YKINE_TARG].cz  =  a_z;
   y    =  x_leg [YKINE_TARG].y   =  x_leg [YKINE_TARG].cy  =  a_y;
   xz   =  x_leg [YKINE_TARG].xz  =  x_leg [YKINE_TARG].cxz = sqrt  ((x * x) + (z * z));
   DEBUG_YKINE_CALC   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc basics)--------------------*/
   fl   =  x_leg [YKINE_TARG].sl   =  x_leg [YKINE_TARG].fl   = sqrt  ((x * x) + (z * z) + (y * y));
   DEBUG_YKINE_CALC   yLOG_complex ("basics"   , "%6.3fcv, %6.3fch, %6.1ffl",  cv, ch, fl);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: isolate the leg values ------------------------------------*/
yKINE__IK_femu     (int a_leg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        rc          =   0;      /* generic return code                 */
   double      d;                      /* degrees                             */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_leg);
   /*---(defense)------------------------*/
   --rce;  if (a_leg < 0 || a_leg > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);
   /*----(check femur orientation)-------*/
   DEBUG_YKINE_CALC   yLOG_complex ("troc"     , "%6.1fcx, %6.1fcz, %6.1fcd", x_leg [YKINE_TROC].cx, x_leg [YKINE_TROC].cz, x_leg [YKINE_TROC].cd);
   x    =  x_leg [YKINE_TARG].cx          - x_leg [YKINE_TROC].cx;
   z    =  x_leg [YKINE_TARG].cz          - x_leg [YKINE_TROC].cz;
   xz   =  sqrt  ((x * x) + (z * z));
   d    = (atan2 (-z, x) * RAD2DEG) - x_leg [YKINE_TROC].cd;
   DEBUG_YKINE_CALC   yLOG_complex ("femu"     , "%6.1fx , %6.1fz , %6.1fxz, %8.3fd ", x, z, xz, d);
   /*----(adjust direction)--------------*/
   while (d  <=  -360.0) d += 360.0;
   while (d  >=   360.0) d -= 360.0;
   DEBUG_YKINE_CALC   yLOG_double  ("fixed d"   , d);
   /*----(adjust direction)--------------*/
   if      (d >  270.0) { d = d - 360.0;  x_leg [YKINE_FEMU].u     = '-'; }
   if      (d >   90.0) { d = d - 180.0;  x_leg [YKINE_FEMU].u     = 'y'; }
   else if (d >    0.0) { d = d        ;  x_leg [YKINE_FEMU].u     = '-'; }
   else if (d >  -90.0) { d = d        ;  x_leg [YKINE_FEMU].u     = '-'; }
   else if (d > -270.0) { d = 180.0 + d;  x_leg [YKINE_FEMU].u     = 'y'; }
   else                 { d = 360.0 + d;  x_leg [YKINE_FEMU].u     = '-'; }     
   DEBUG_YKINE_CALC   yLOG_double  ("new d"     , d);
   DEBUG_YKINE_CALC   yLOG_char    ("under femu", x_leg [YKINE_FEMU].u);
   /*----(save)--------------------------*/
   rc   = yKINE__femu     (a_leg, d, YKINE_IK);
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
yKINE__IK_pate     (int a_num)
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
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(patella angle)------------------*/
   dv   =  x_leg [YKINE_LOWR].d;
   sl   =  x_leg [YKINE_LOWR].sl;
   pl   =  x_leg [YKINE_PATE].l;
   tl   =  x_leg [YKINE_TIBI].l;
   DEBUG_YKINE_CALC   yLOG_double  ("dv"        , dv);
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((sl * sl) + (pl * pl) - (tl * tl)) / (2.0 * sl * pl));
   if (isnan (a))    d = 0.0;
   else              d = a * RAD2DEG;
   DEBUG_YKINE_CALC   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*---(reorient as needed)-------------*/
   DEBUG_YKINE_CALC   yLOG_char    ("under femu", x_leg [YKINE_FEMU].u);
   DEBUG_YKINE_CALC   yLOG_char    ("under pate", x_leg [YKINE_PATE].u);
   DEBUG_YKINE_CALC   yLOG_double  ("lowr y"    , x_leg [YKINE_LOWR].y);
   if         (x_leg [YKINE_FEMU].u == 'y') {
      if      (x_leg [YKINE_PATE].u == 'y')   d = 180.0 - (d  + dv); /* 8.6000 */
      else                                    d =  d  - dv;
   } else {
      if      (x_leg [YKINE_LOWR].y >  0.0)   d =  dv - d;  /* fix 0.3750 */
      else if (x_leg [YKINE_PATE].u == 'y')   d =  180.0 - (dv + d); /* 7.2750 */
      else                                    d =  dv - d;  /* fix 0.0250 */
   }
   DEBUG_YKINE_CALC   yLOG_double  ("new d"     , d);
   /*----(save)--------------------------*/
   rc   = yKINE__pate     (a_num, -d, YKINE_IK);
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
yKINE__IK_tibi     (int a_num)
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
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_CALC   yLOG_value   ("a_leg"     , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_YKINE_CALC   yLOG_note    ("leg number is out of range");
      DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(tibia angle)--------------------*/
   dv   =  x_leg [YKINE_LOWR].d;
   sl   =  x_leg [YKINE_LOWR].sl;
   pl   =  x_leg [YKINE_PATE].l;
   tl   =  x_leg [YKINE_TIBI].l;
   DEBUG_YKINE_CALC   yLOG_double  ("dv"        , dv);
   DEBUG_YKINE_CALC   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((tl * tl) + (pl * pl) - (sl * sl)  ) / (2.0 * pl * tl));
   a   -=  M_PI;
   d    =  -(round  (a  * RAD2DEG));
   if (isnan (a))    d = 0.0;
   DEBUG_YKINE_CALC   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*----(save)--------------------------*/
   rc   = yKINE__tibi     (a_num, 90.0 - d, YKINE_IK);
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
/*===----                        full drivers                          ----===*/
/*====================------------------------------------====================*/
static void      o___MAIN____________________o (void) {;};

char         /*--> drive the leg position from angles ----[ ------ [ ------ ]-*/
yKINE_forward      (int a_num, double a_femu, double a_pate, double a_tibi)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;      /* generic return code                 */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   yKINE__wipe     (a_num, YKINE_FK);
   /*---(fixed body)---------------------*/
   if (rc >= 0)  rc = yKINE__thor     (a_num);
   if (rc >= 0)  rc = yKINE__coxa     (a_num);
   if (rc >= 0)  rc = yKINE__troc     (a_num);
   /*---(movable)------------------------*/
   if (rc >= 0)  rc = yKINE__femu     (a_num, a_femu, YKINE_FK);
   if (rc >= 0)  rc = yKINE__pate     (a_num, a_pate, YKINE_FK);
   if (rc >= 0)  rc = yKINE__tibi     (a_num, a_tibi, YKINE_FK);
   /*---(target setting)-----------------*/
   if (rc >= 0)  rc = yKINE__FK_targ  (a_num, YKINE_FK);
   if (rc >= 0)  rc = yKINE__lowr     (a_num, YKINE_FK);
   /*---(future)-------------------------*/
   if (rc >= 0)  rc = yKINE__meta     (a_num);
   if (rc >= 0)  rc = yKINE__tars     (a_num);
   if (rc >= 0)  rc = yKINE__foot     (a_num);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
yKINE_inverse      (int a_num, double a_x, double a_z, double a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;      /* generic return code                 */
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   yKINE__wipe     (a_num, YKINE_IK);
   /*---(target setting)-----------------*/
   if (rc >= 0)  rc = yKINE__IK_targ  (a_num, a_x, a_z, a_y);
   /*---(fixed body)---------------------*/
   if (rc >= 0)  rc = yKINE__thor     (a_num);
   if (rc >= 0)  rc = yKINE__coxa     (a_num);
   if (rc >= 0)  rc = yKINE__troc     (a_num);
   /*---(movable)------------------------*/
   if (rc >= 0)  rc = yKINE__IK_femu  (a_num);
   if (rc >= 0)  rc = yKINE__lowr     (a_num, YKINE_IK);
   if (rc >= 0)  rc = yKINE__IK_pate  (a_num);
   if (rc >= 0)  rc = yKINE__IK_tibi  (a_num);
   /*---(future)-------------------------*/
   if (rc >= 0)  rc = yKINE__meta     (a_num);
   if (rc >= 0)  rc = yKINE__tars     (a_num);
   if (rc >= 0)  rc = yKINE__foot     (a_num);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*============================----end-of-source---============================*/
