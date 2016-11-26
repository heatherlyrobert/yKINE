/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*---(global characteristics)----------------------*/

tLEGDATA    leg_data [YKINE_MAX_LEGS] = {
   /* front is facing down the negative z-axis        */
   /* ---full------------ --two---- --caps--- ----deg */
   { "right rear"        , "rr"    , "RR"    ,  300.0 },
   { "right middle"      , "rm"    , "RM"    ,    0.0 },
   { "right front"       , "rf"    , "RF"    ,   60.0 },
   { "left front"        , "lf"    , "LF"    ,  120.0 },
   { "left middle"       , "lm"    , "LM"    ,  180.0 },
   { "left rear"         , "lr"    , "LR"    ,  240.0 },
   { "-----"             , "--"    , "--"    ,    0.0 },
};

tSEGDATA    seg_data [YKINE_MAX_SEGS] = {
   /* ---full------------ --four--- --caps--- move ----len ----min ----max --test1 --test2 --test3 */
   { "focus"             , "focu"  , "FOCU"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "core"              , "core"  , "CORE"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "thorax"            , "thor"  , "THOR"  , '-',  125.0, -360.0,  360.0,   75.0,   75.0,    0.0 },
   { "coxa"              , "coxa"  , "COXA"  , '-',   30.0,    0.0,    0.0,   25.0,   30.0,    0.0 },
   { "trochanter"        , "troc"  , "TROC"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "femur"             , "femu"  , "FEMU"  , 'y',   30.0,  -85.0,   85.0,   25.0,   30.0,    0.0 },
   { "patella"           , "pate"  , "PATE"  , 'y',   57.0,  -90.0,   45.0,   50.0,   57.0,    0.0 },
   { "tibia"             , "tibi"  , "TIBI"  , 'y',  130.0,   10.0,  130.0,  100.0,  130.0,    0.0 },
   { "metatarsus"        , "meta"  , "META"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "tarsus"            , "tars"  , "TARS"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "foot"              , "foot"  , "FOOT"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "claw"              , "claw"  , "CLAW"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "magnet"            , "magn"  , "MAGN"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "hook"              , "hook"  , "HOOK"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "original"          , "orig"  , "ORIG"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "target"            , "targ"  , "TARG"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "lower_leg"         , "lowr"  , "LOWR"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "calculation"       , "calc"  , "CALC"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "-----"             , "----"  , "----"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
};



/*====================------------------------------------====================*/
/*===----                      static data accessors                   ----===*/
/*====================------------------------------------====================*/
static void      o___STATIC__________________o (void) {;};

char*    yKINE_legtwo       (int a_leg)    { return leg_data [a_leg].two;  }
char*    yKINE_legcaps      (int a_leg)    { return leg_data [a_leg].caps; }
char*    yKINE_legfull      (int a_leg)    { return leg_data [a_leg].full; }
double   yKINE_legdeg       (int a_leg)    { return leg_data [a_leg].deg; }

char*    yKINE_segfour      (int a_seg)    { return seg_data [a_seg].four; }
char*    yKINE_segcaps      (int a_seg)    { return seg_data [a_seg].caps; }
char*    yKINE_segfull      (int a_seg)    { return seg_data [a_seg].full; }
double   yKINE_seglen       (int a_seg)    { return seg_data [a_seg].len; }
double   yKINE_segmin       (int a_seg)    { return seg_data [a_seg].min; }
double   yKINE_segmax       (int a_seg)    { return seg_data [a_seg].max; }



/*====================------------------------------------====================*/
/*===----                     dynamic data accessors                   ----===*/
/*====================------------------------------------====================*/
static void      o___DYNAMIC_________________o (void) {;};

char
yKINE_endpoint     (int a_leg, int a_seg, int a_meth, double *a_deg, double *a_len, double *a_x, double *a_z, double *a_y)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   switch (a_meth) {
   case  YKINE_GK : x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_FK : x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_IK : x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);  break;
   }
   /*---(return actuals)-----------------*/
   if (a_deg  != NULL)  *a_deg  = x_leg [a_seg].d;
   if (a_len  != NULL)  *a_len  = x_leg [a_seg].fl;
   if (a_x    != NULL)  *a_x    = x_leg [a_seg].cx;
   if (a_z    != NULL)  *a_z    = x_leg [a_seg].cz;
   if (a_y    != NULL)  *a_y    = x_leg [a_seg].cy;
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE_segment      (int a_leg, int a_seg, int a_meth, double *a_deg, double *a_len, double *a_x, double *a_z, double *a_y)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   switch (a_meth) {
   case  YKINE_GK : x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_FK : x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_IK : x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);  break;
   }
   /*---(return actuals)-----------------*/
   if (a_deg  != NULL)  *a_deg  = x_leg [a_seg].d;
   if (a_len  != NULL)  *a_len  = x_leg [a_seg].l;
   if (a_x    != NULL)  *a_x    = x_leg [a_seg].x;
   if (a_z    != NULL)  *a_z    = x_leg [a_seg].z;
   if (a_y    != NULL)  *a_y    = x_leg [a_seg].y;
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE_angle        (int a_leg, int a_seg, int a_meth, double *a_deg, double *a_len, double *a_v, double *a_h)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   switch (a_meth) {
   case  YKINE_GK : x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_FK : x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_IK : x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);  break;
   }
   /*---(return actuals)-----------------*/
   if (a_deg  != NULL)  *a_deg  = x_leg [a_seg].d;
   if (a_len  != NULL)  *a_len  = x_leg [a_seg].l;
   if (a_v    != NULL)  *a_v    = x_leg [a_seg].cv;
   if (a_h    != NULL)  *a_h    = x_leg [a_seg].ch;
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE_enddiff      (int a_leg, int a_seg, int a_meth, double *a_deg, double *a_len, double *a_x, double *a_z, double *a_y)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   switch (a_meth) {
   case  YKINE_GK : x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_FK : x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_IK : x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);  break;
   default        : return -1;
   }
   /*---(return actuals)-----------------*/
   if (a_deg  != NULL)  *a_deg  = x_leg [a_seg].d  - gk [(int) a_leg][a_seg].d;
   if (a_len  != NULL)  *a_len  = x_leg [a_seg].fl - gk [(int) a_leg][a_seg].fl;
   if (a_x    != NULL)  *a_x    = x_leg [a_seg].cx - gk [(int) a_leg][a_seg].cx;
   if (a_z    != NULL)  *a_z    = x_leg [a_seg].cz - gk [(int) a_leg][a_seg].cz;
   if (a_y    != NULL)  *a_y    = x_leg [a_seg].cy - gk [(int) a_leg][a_seg].cy;
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE_segdiff      (int a_leg, int a_seg, int a_meth, double *a_deg, double *a_len, double *a_x, double *a_z, double *a_y)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   switch (a_meth) {
   case  YKINE_GK : x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_FK : x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_IK : x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);  break;
   default        : return -1;
   }
   /*---(return actuals)-----------------*/
   if (a_deg  != NULL)  *a_deg  = x_leg [a_seg].d  - gk [(int) a_leg][a_seg].d;
   if (a_len  != NULL)  *a_len  = x_leg [a_seg].l  - gk [(int) a_leg][a_seg].l;
   if (a_x    != NULL)  *a_x    = x_leg [a_seg].x  - gk [(int) a_leg][a_seg].x;
   if (a_z    != NULL)  *a_z    = x_leg [a_seg].z  - gk [(int) a_leg][a_seg].z;
   if (a_y    != NULL)  *a_y    = x_leg [a_seg].y  - gk [(int) a_leg][a_seg].y;
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE_angles       (int a_leg, int a_meth, double *a_coxa, double *a_femu, double *a_pate, double *a_tibi)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   switch (a_meth) {
   case  YKINE_GK : x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_FK : x_leg = ((tSEG *) fk) + (a_leg * YKINE_MAX_SEGS);  break;
   case  YKINE_IK : x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);  break;
   default        : return -1;
   }
   if (a_coxa != NULL)  *a_coxa = x_leg [YKINE_COXA].d;
   if (a_femu != NULL)  *a_femu = x_leg [YKINE_FEMU].d;
   if (a_pate != NULL)  *a_pate = x_leg [YKINE_PATE].d;
   if (a_tibi != NULL)  *a_tibi = x_leg [YKINE_TIBI].d;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     data changing/setting                    ----===*/
/*====================------------------------------------====================*/
static void      o___SETTERS_________________o (void) {;};

char       /*----: unit testing setter for unit testing ----------------------*/
yKINE__setlen      (
      double a_thor, double a_coxa, double a_troc,
      double a_femu, double a_pate, double a_tibi,
      double a_meta, double a_tars, double a_foot)
{
   int i = 0;
   for (i = 0; i < YKINE_MAX_LEGS; ++i) {
      ik [i][YKINE_THOR].l   = fk [i][YKINE_THOR].l   = a_thor;
      ik [i][YKINE_COXA].l   = fk [i][YKINE_COXA].l   = a_coxa;
      ik [i][YKINE_TROC].l   = fk [i][YKINE_TROC].l   = a_troc;
      ik [i][YKINE_FEMU].l   = fk [i][YKINE_FEMU].l   = a_femu;
      ik [i][YKINE_PATE].l   = fk [i][YKINE_PATE].l   = a_pate;
      ik [i][YKINE_TIBI].l   = fk [i][YKINE_TIBI].l   = a_tibi;
      ik [i][YKINE_META].l   = fk [i][YKINE_META].l   = a_meta;
      ik [i][YKINE_TARS].l   = fk [i][YKINE_TARS].l   = a_tars;
      ik [i][YKINE_FOOT].l   = fk [i][YKINE_FOOT].l   = a_foot;
   }
   return 0;
}

char       /*----: unit testing setter for unit testing ----------------------*/
yKINE__setter      (char *a_request, int a_leg, int a_seg, double a_value)
{
   char        rce         = -11;
   /*---(defense)-----------------------------------------*/
   --rce;  if (a_leg < 0       )   return rce;
   --rce;  if (a_leg > YKINE_MAX_LEGS)   return rce;
   --rce;  if (a_seg < 0       )   return rce;
   --rce;  if (a_seg > YKINE_MAX_SEGS)   return rce;
   if        (strcmp (a_request , "seg_length"   ) == 0) {
      fk [a_leg][a_seg].l   = fk [a_leg][a_seg].sl  = a_value;
      ik [a_leg][a_seg].l   = ik [a_leg][a_seg].sl  = a_value;
      gk [a_leg][a_seg].l   = gk [a_leg][a_seg].sl  = a_value;
   } else if (strcmp (a_request , "seg_degree"   ) == 0) {
      ik [a_leg][a_seg].d = a_value;
      fk [a_leg][a_seg].d = a_value;
      gk [a_leg][a_seg].d = a_value;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       opengl actuals                         ----===*/
/*====================------------------------------------====================*/
static void      o___OPENGL__________________o (void) {;};

char         /*--> set the opengl actual values ----------[ ------ [ ------ ]-*/
yKINE_opengl       (int a_leg, int a_seg, double a_deg, double a_x, double a_z, double a_y, double a_len)
{
   /*---(locals)-----------+-----------+-*/
   double      x_len       = 0.0;
   double      x_y         = 0.0;
   tSEG       *x_leg       = NULL;
   /*---(set the leg)--------------------*/
   x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);
   /*---(set cumulatives)----------------*/
   x_leg [a_seg].cx  = a_x;
   x_leg [a_seg].cz  = a_z;
   x_leg [a_seg].cy  = a_y;
   x_leg [a_seg].fl  = sqrt ((a_x * a_x) + (a_z * a_z) + (a_y * a_y));
   x_len             = x_leg [a_seg].fl;
   x_leg [a_seg].cxz = sqrt ((x_len * x_len) - (a_y * a_y));
   /*---(set individuals)----------------*/
   x_leg [a_seg].d   = a_deg;
   x_leg [a_seg].l   = a_len;
   x_leg [a_seg].sl  = a_len;
   x_leg [a_seg].x   = a_x - x_leg [a_seg - 1].cx;
   x_leg [a_seg].z   = a_z - x_leg [a_seg - 1].cz;
   x_leg [a_seg].y   = a_y - x_leg [a_seg - 1].cy;
   x_y               = x_leg [a_seg].y;
   x_leg [a_seg].xz  = sqrt ((a_len * a_len) - (x_y * x_y));
   /*---(radians)------------------------*/
   if        (a_seg <= YKINE_TROC) {
      x_leg [a_seg].cv  = x_leg [a_seg].v   = 0.0;
      x_leg [a_seg].ch  = x_leg [a_seg].h   = a_deg * DEG2RAD;
   } else if (a_seg == YKINE_FEMU) {
      x_leg [a_seg].cv  = x_leg [a_seg].v   = 0.0;
      x_leg [a_seg].h   = a_deg * DEG2RAD;
      x_leg [a_seg].ch  = x_leg [YKINE_FEMU].h   + x_leg [YKINE_TROC].ch;
   } else if (a_seg == YKINE_PATE) {
      x_leg [a_seg].cv  = x_leg [a_seg].v   = a_deg * DEG2RAD;
      x_leg [a_seg].h   = 0.0;
      x_leg [a_seg].ch  = x_leg [YKINE_FEMU].ch;
   } else if (a_seg == YKINE_TIBI) {
      x_leg [a_seg].v   = a_deg * DEG2RAD;
      x_leg [a_seg].cv  = x_leg [YKINE_TIBI].v   + x_leg [YKINE_PATE].cv;
      x_leg [a_seg].h   = 0.0;
      x_leg [a_seg].ch  = x_leg [YKINE_FEMU].ch;
   } else if (a_seg <= YKINE_FOOT) {
      x_leg [a_seg].v   = 0.0;
      x_leg [a_seg].cv  = x_leg [YKINE_TIBI].cv;
      x_leg [a_seg].h   = 0.0;
      x_leg [a_seg].ch  = x_leg [YKINE_TIBI].ch;
   }
   /*---(target)-------------------------*/
   if (a_seg == YKINE_TIBI) {
      yKINE__targ     (a_leg, YKINE_GK);
      yKINE__lowr     (a_leg, YKINE_GK);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
