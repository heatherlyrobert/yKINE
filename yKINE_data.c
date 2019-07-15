/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



tLEGDATA    g_leg_data [YKINE_MAX_LEGS] = {
   /* front is facing down the negative z-axis        */
   /* leg-------- abbr full----------------- two------ caps----- deg---- */
   {  YKINE_BODY, '-', "body"              , "--"    , "--"    ,    0.0  },
   {  YKINE_RR  , '0', "right rear"        , "rr"    , "RR"    ,  300.0  },
   {  YKINE_RM  , '1', "right middle"      , "rm"    , "RM"    ,    0.0  },
   {  YKINE_RF  , '2', "right front"       , "rf"    , "RF"    ,   60.0  },
   {  YKINE_LF  , '3', "left front"        , "lf"    , "LF"    ,  120.0  },
   {  YKINE_LM  , '4', "left middle"       , "lm"    , "LM"    ,  180.0  },
   {  YKINE_LR  , '5', "left rear"         , "lr"    , "LR"    ,  240.0  },
   {  YKINE_RP  , '-', "right posterior"   , "rp"    , "RP"    ,    0.0  },
   {  YKINE_RA  , '-', "right anterior"    , "ra"    , "RA"    ,    0.0  },
   {  YKINE_LA  , '-', "left anterior"     , "la"    , "LA"    ,    0.0  },
   {  YKINE_LP  , '-', "left posterior"    , "lp"    , "LP"    ,    0.0  },
   {  YKINE_BEAK, '-', "beak"              , "bk"    , "BK"    ,    0.0  },
   {  -1        , '-', "-----"             , "--"    , "--"    ,    0.0  },
};



/*> tSEGDATA    g_seg_data [YKINE_MAX_SEGS] = {                                                                                                      <* 
 *>    /+ seg-------- abbr ---full-------------- four----- caps----- move len---- min---- attn--- max---- test1-- test2-- test3-- negative-dir--+/   <* 
 *>    {  YKINE_FOCU, '-', "focus"             , "focu"  , "FOCU"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_CORE, '-', "core"              , "core"  , "CORE"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_THOR, '-', "thorax"            , "thor"  , "THOR"  , '-',  125.0, -360.0,    0.0,  360.0,   75.0,   75.0,    0.0, ""            },   <* 
 *>    {  YKINE_COXA, '-', "coxa"              , "coxa"  , "COXA"  , '-',   30.0,    0.0,    0.0,    0.0,   25.0,   30.0,    0.0, ""            },   <* 
 *>    {  YKINE_TROC, '-', "trochanter"        , "troc"  , "TROC"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_FEMU, 'f', "femur"             , "femu"  , "FEMU"  , 'y',   30.0,  -85.0,    0.0,   85.0,   25.0,   30.0,    0.0, "clockwise"   },   <* 
 *>    {  YKINE_PATE, 'p', "patella"           , "pate"  , "PATE"  , 'y',   57.0,  -45.0,    0.0,   90.0,   50.0,   57.0,    0.0, "down/inward" },   <* 
 *>    {  YKINE_TIBI, 't', "tibia"             , "tibi"  , "TIBI"  , 'y',  130.0,  -40.0,    0.0,   80.0,  100.0,  130.0,    0.0, "down/inward" },   <* 
 *>    {  YKINE_META, '-', "metatarsus"        , "meta"  , "META"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_TARS, '-', "tarsus"            , "tars"  , "TARS"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_FOOT, '-', "foot"              , "foot"  , "FOOT"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_CLAW, '-', "claw"              , "claw"  , "CLAW"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_MAGN, '-', "magnet"            , "magn"  , "MAGN"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_HOOK, '-', "hook"              , "hook"  , "HOOK"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_ORIG, '-', "original"          , "orig"  , "ORIG"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_TARG, '-', "target"            , "targ"  , "TARG"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_LOWR, '-', "lower_leg"         , "lowr"  , "LOWR"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  YKINE_CALC, '-', "calculation"       , "calc"  , "CALC"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *>    {  -1        , '-', "-----"             , "----"  , "----"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },   <* 
 *> };                                                                                                                                               <*/

/* changes to address foot
 *    -- it always faces target due to round shape
 *    -- roughly one-half inch in diameter
 *    -- 0.25in = 6.35mm, i will round down to 6.3mm
 *
 * remeasure tibia to be more accurrate
 *    -- ground to center of screw 5.5in
 *    -- 139.7mm minus foot of 6.3
 *    -- 133.4mm
 *
 * remeasure patella to be more accurrate
 *    -- between center of screws 2.5in
 *    -- 63.5mm
 *
 * remeasure femu to be more accurrate
 *    -- between center of screws 1.125in (1 and 1/8)
 *    -- 28.6mm
 *
 * remeasure body to be more accurrate
 *    -- between center of coxa screws 10.75in
 *    -- 273mm in diameter
 *    -- 136mm
 *    -- coxa is fixed but 1.125in (1 and 1/8)
 *    -- will round cox back up to tranditional 30mm
 *
 */
tSEGDATA    g_seg_data [YKINE_MAX_SEGS] = {
   /* seg-------- abbr ---full-------------- four----- caps----- move len---- min---- attn--- max---- test1-- test2-- test3-- negative-dir--*/
   {  YKINE_FOCU, '-', "focus"             , "focu"  , "FOCU"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_CORE, '-', "core"              , "core"  , "CORE"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_THOR, '-', "thorax"            , "thor"  , "THOR"  , '-',  106.0, -360.0,    0.0,  360.0,   75.0,   75.0,    0.0, ""            },
   {  YKINE_COXA, '-', "coxa"              , "coxa"  , "COXA"  , '-',   30.0,    0.0,    0.0,    0.0,   25.0,   30.0,    0.0, ""            },
   {  YKINE_TROC, '-', "trochanter"        , "troc"  , "TROC"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_FEMU, 'f', "femur"             , "femu"  , "FEMU"  , 'y',   28.6,  -85.0,    0.0,   85.0,   25.0,   30.0,    0.0, "clockwise"   },
   {  YKINE_PATE, 'p', "patella"           , "pate"  , "PATE"  , 'y',   63.5,  -45.0,    0.0,   90.0,   50.0,   57.0,    0.0, "down/inward" },
   {  YKINE_TIBI, 't', "tibia"             , "tibi"  , "TIBI"  , 'y',  133.4,  -40.0,    0.0,   80.0,  100.0,  130.0,    0.0, "down/inward" },
   {  YKINE_META, '-', "metatarsus"        , "meta"  , "META"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_TARS, '-', "tarsus"            , "tars"  , "TARS"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_FOOT, '-', "foot"              , "foot"  , "FOOT"  , '-',    6.3,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_CLAW, '-', "claw"              , "claw"  , "CLAW"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_MAGN, '-', "magnet"            , "magn"  , "MAGN"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_HOOK, '-', "hook"              , "hook"  , "HOOK"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_ORIG, '-', "original"          , "orig"  , "ORIG"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_TARG, '-', "target"            , "targ"  , "TARG"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_LOWR, '-', "lower_leg"         , "lowr"  , "LOWR"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  YKINE_CALC, '-', "calculation"       , "calc"  , "CALC"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
   {  -1        , '-', "-----"             , "----"  , "----"  , '-',    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0, ""            },
};



/*====================------------------------------------====================*/
/*===----                      static data accessors                   ----===*/
/*====================------------------------------------====================*/
static void      o___STATIC__________________o (void) {;};

char*    yKINE_legtwo       (char a_leg)    { return g_leg_data [a_leg].two;  }
char*    yKINE_legcaps      (char a_leg)    { return g_leg_data [a_leg].caps; }
char*    yKINE_legfull      (char a_leg)    { return g_leg_data [a_leg].full; }
float    yKINE_legdeg       (char a_leg)    { return g_leg_data [a_leg].deg;  }

char*    yKINE_segfour      (char a_seg)    { return g_seg_data [a_seg].four; }
char*    yKINE_segcaps      (char a_seg)    { return g_seg_data [a_seg].caps; }
char*    yKINE_segfull      (char a_seg)    { return g_seg_data [a_seg].full; }
float    yKINE_seglen       (char a_seg)    { return g_seg_data [a_seg].len;  }
float    yKINE_segmin       (char a_seg)    { return g_seg_data [a_seg].min;  }
float    yKINE_segattn      (char a_seg)    { return g_seg_data [a_seg].attn; }
float    yKINE_segmax       (char a_seg)    { return g_seg_data [a_seg].max;  }



/*====================------------------------------------====================*/
/*===----                     dynamic data accessors                   ----===*/
/*====================------------------------------------====================*/
static void      o___DYNAMIC_________________o (void) {;};

char
yKINE_endpoint     (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y)
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
yKINE_segment      (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y)
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
yKINE_angle        (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_v, float *a_h)
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
yKINE_enddiff      (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y)
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
yKINE_segdiff      (char a_leg, char a_seg, int a_meth, float *a_deg, float *a_len, float *a_x, float *a_z, float *a_y)
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
yKINE_angles       (char a_leg, char a_meth, float *a_coxa, float *a_femu, float *a_pate, float *a_tibi)
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
      float a_thor, float a_coxa, float a_troc,
      float a_femu, float a_pate, float a_tibi,
      float a_meta, float a_tars, float a_foot)
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
yKINE__setter      (char *a_request, int a_leg, int a_seg, float a_value)
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
yKINE_opengl       (int a_leg, int a_seg, float a_deg, float a_x, float a_z, float a_y, float a_len)
{
   /*---(locals)-----------+-----------+-*/
   double      x_len       = 0.0;
   double      x_y         = 0.0;
   tSEG       *x_leg       = NULL;
   double      x, z, y, sl, fl;
   /*---(header)-------------------------*/
   DEBUG_YKINE_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   yKINE__wipe     (a_leg, YKINE_IK);
   /*---(target setting)-----------------*/
   DEBUG_YKINE_CALC   yLOG_complex ("object"    , "%1dl (%2.2s), %2ds (%4.4s), %8.3lfd", a_leg, g_leg_data [a_leg].caps, a_seg, g_seg_data [a_seg].four, a_deg);
   /*---(set the leg)--------------------*/
   x_leg = ((tSEG *) gk) + (a_leg * YKINE_MAX_SEGS);
   /*---(set cumulatives)----------------*/
   x_leg [a_seg].cx  = a_x;
   x_leg [a_seg].cz  = a_z;
   x_leg [a_seg].cy  = a_y;
   fl = x_leg [a_seg].fl  = sqrt ((a_x * a_x) + (a_z * a_z) + (a_y * a_y));
   x_len             = x_leg [a_seg].fl;
   x_leg [a_seg].cxz = sqrt ((x_len * x_len) - (a_y * a_y));
   DEBUG_YKINE_CALC   yLOG_complex ("cums"      , "%8.3lfcx, %8.3lfcz, %8.3lfcy, %8.3lffl", a_z, a_z, a_y, fl);
   /*---(set individuals)----------------*/
   x_leg [a_seg].d   = a_deg;
   x_leg [a_seg].l   = a_len;
   x_leg [a_seg].sl  = a_len;
   x  = x_leg [a_seg].x   = a_x - x_leg [a_seg - 1].cx;
   z  = x_leg [a_seg].z   = a_z - x_leg [a_seg - 1].cz;
   y  = x_leg [a_seg].y   = a_y - x_leg [a_seg - 1].cy;
   x_y               = x_leg [a_seg].y;
   x_leg [a_seg].xz  = sqrt ((a_len * a_len) - (x_y * x_y));
   DEBUG_YKINE_CALC   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy , %8.3lfsl", x, z, y, a_len);
   /*---(radians)------------------------*/
   if        (a_seg <= YKINE_TROC) {
      x_leg [a_seg].cv  = x_leg [a_seg].v   = 0.0;
      x_leg [a_seg].ch  = x_leg [a_seg].h   = a_deg * DEG2RAD;
   } else if (a_seg == YKINE_FEMU) {
      x_leg [a_seg].cv  = x_leg [a_seg].v   = 0.0;
      x_leg [a_seg].h   = a_deg * DEG2RAD;
      x_leg [a_seg].ch  = x_leg [YKINE_FEMU].h   + x_leg [YKINE_TROC].ch;
   } else if (a_seg == YKINE_PATE) {
      x_leg [a_seg].cv  = x_leg [a_seg].v   = -a_deg * DEG2RAD;
      x_leg [a_seg].h   = 0.0;
      x_leg [a_seg].ch  = x_leg [YKINE_FEMU].ch;
   } else if (a_seg == YKINE_TIBI) {
      x_leg [a_seg].v   = (90.0 - a_deg) * DEG2RAD;
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
   if (a_seg == YKINE_FOOT) {
      yKINE__FK_targ  (a_leg, YKINE_GK);
      yKINE__lowr     (a_leg, YKINE_GK);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*============================----end-of-source---============================*/
