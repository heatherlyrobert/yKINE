/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



typedef struct cLOCAL tLOCAL;
struct cLOCAL {
   /*---(overall)-----------*/
   char        debug;
   int         logger;
};
static tLOCAL its;
#define     DEBUG_KINE     if (its.debug == 'y')

char        yKINE_ver     [500];




const    double   DEG2RAD   = M_PI / 180.0;
const    double   RAD2DEG   = 180.0 / M_PI;


tSEG      fk [MAX_LEGS] [MAX_SEGS];    /* forward kinematics        */
tSEG      ik [MAX_LEGS] [MAX_SEGS];    /* inverse kinematics        */
tSEG      gk [MAX_LEGS] [MAX_SEGS];    /* opengl kinematics check   */

/*---(global characteristics)----------------------*/



struct cLEGDATA
{
   char        full        [25];
   char        two         [ 5];
   char        caps        [ 5];
   double      deg;
} leg_data [MAX_LEGS] = {
   /* front is facing down the negative z-axis        */
   /* ---full------------ --two---- --caps--- ----deg */
   { "right front"       , "rf"    , "RF"    ,   60.0 },
   { "right middle"      , "rm"    , "RM"    ,    0.0 },
   { "right rear"        , "rr"    , "RR"    ,  300.0 },
   { "left rear"         , "lr"    , "LR"    ,  240.0 },
   { "left middle"       , "lm"    , "LM"    ,  180.0 },
   { "left front"        , "lf"    , "LF"    ,  120.0 },
   { "-----"             , "--"    , "--"    ,    0.0 },
};



struct cSEGDATA {
   char        full        [25];            /* descriptive name               */
   char        four        [ 5];            /* abbreviated name               */
   char        caps        [ 5];            /* capitalized name for titles    */
   double      len;                         /* length                         */
   double      min;                         /* min degrees for joint          */
   double      max;                         /* max degrees for joint          */
   double      test1;                       /* test length (1) original       */
   double      test2;                       /* test length (2) newer          */
   double      test3;                       /* test length (3)                */
} seg_data [MAX_SEGS] = {
   /* ---full------------ --four--- --caps--- ----len ----min ----max --test1 --test2 --test3 */
   { "focus"             , "focu"  , "FOCU"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "core"              , "core"  , "CORE"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "thorax"            , "thor"  , "THOR"  ,  125.0, -360.0,  360.0,   75.0,   75.0,    0.0 },
   { "coxa"              , "coxa"  , "COXA"  ,   30.0,    0.0,    0.0,   25.0,   30.0,    0.0 },
   { "trochanter"        , "troc"  , "TROC"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "femur"             , "femu"  , "FEMU"  ,   30.0,  -85.0,   85.0,   25.0,   30.0,    0.0 },
   { "patella"           , "pate"  , "PATE"  ,   57.0,  -45.0,   90.0,   50.0,   57.0,    0.0 },
   { "tibia"             , "tibi"  , "TIBI"  ,  130.0,   10.0,  130.0,  100.0,  130.0,    0.0 },
   { "metatarsus"        , "meta"  , "META"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "tarsus"            , "tars"  , "TARS"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "foot"              , "foot"  , "FOOT"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "claw"              , "claw"  , "CLAW"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "magnet"            , "magn"  , "MAGN"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "hook"              , "hook"  , "HOOK"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "original"          , "orig"  , "ORIG"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "target"            , "targ"  , "TARG"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "lower_leg"         , "lowr"  , "LOWR"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "calculation"       , "calc"  , "CALC"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "-----"             , "----"  , "----"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
};









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
   if   (a_flag == 'y')  its.debug   = 'y';
   else                  its.debug   = '-';
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
   /*---(set body)-----------------------*/
   /*> kine_center       (0.0f, 0.0f);                                                <* 
    *> kine_height       (segs_len [TIBI]);                                           <* 
    *> kine_pivot        (0.0f, 0.0f);                                                <* 
    *> kine_attitude     (0.0f, 0.0f, 0.0f);                                          <*/
   /*---(clean legs)---------------------*/
   for (x_leg = 0; x_leg < MAX_LEGS; ++x_leg) {
      for (x_seg = 0; x_seg < MAX_SEGS; ++x_seg) {
         yKINE__clear ( &(fk [x_leg][x_seg]), "fk", x_leg, x_seg, a_type);
         yKINE__clear ( &(ik [x_leg][x_seg]), "ik", x_leg, x_seg, a_type);
         yKINE__clear ( &(gk [x_leg][x_seg]), "gk", x_leg, x_seg, a_type);
      }
   }
   /*---(complete)-----------------------*/
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
   if (a_seg == THOR)  a_curr->d    =  leg_data [a_leg].deg;
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
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       fixed body parts                       ----===*/
/*====================------------------------------------====================*/
static void      o___FIXED___________________o (void) {;}
/*
 *  fx, ik, etc makes no difference to the thorax and coxa (at this time)
 *
 */

char         /*--: establish thorax endpoint -------------[ leaf   [ ------ ]-*/
yKINE__thor        (int   a_num)
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)----------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(save basics)-----------------*/
   l     =  x_leg [THOR].l;                        /* set during yKINE__clear */
   d     =  x_leg [THOR].cd   =  x_leg [THOR].d;   /* set during yKINE__clear */
   v     =  x_leg [THOR].v    =  x_leg [THOR].cv  =  0.0f;
   h     =  x_leg [THOR].h    =  x_leg [THOR].ch  =  d * DEG2RAD;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)-------------*/
   x     =  x_leg [THOR].x    =  l * cos (h);
   z     =  x_leg [THOR].z    = -l * sin (h);
   y     =  x_leg [THOR].y    =  0.0f;
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
   /*---(calc cums)-------------------*/
   cx    =  x_leg [THOR].cx   =  x;
   cz    =  x_leg [THOR].cz   =  z;
   cy    =  x_leg [THOR].cy   =  y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)-----------------*/
   xz    =  x_leg [THOR].xz   =  sqrt (( x *  x) + ( z *  z));
   sl    =  x_leg [THOR].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [THOR].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl    =  x_leg [THOR].fl   =  x_leg [THOR].cxz;
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
   /*---(add to leg values)-----------*/
   x_leg [CALC].x   += x;
   x_leg [CALC].z   += z;
   x_leg [CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(save basics)--------------------*/
   l     =  x_leg [COXA].l;
   d     =  x_leg [COXA].cd   =  x_leg [THOR].cd;
   v     =  x_leg [COXA].cv   =  0.0f;
   h     =  x_leg [COXA].ch   =  d * DEG2RAD;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)----------------*/
   x     =  x_leg [COXA].x    =  l * cos (h);
   z     =  x_leg [COXA].z    = -l * sin (h);
   y     =  x_leg [COXA].y    =  0.0f;
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
   /*---(calc cums)----------------------*/
   cx    =  x_leg [COXA].cx   =  x_leg [THOR].cx + x;
   cz    =  x_leg [COXA].cz   =  x_leg [THOR].cz + z;
   cy    =  x_leg [COXA].cy   =  x_leg [THOR].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)-----------------*/
   xz    =  x_leg [COXA].xz   =  sqrt (( x *  x) + ( z *  z));
   sl    =  x_leg [COXA].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [COXA].cxz           =  sqrt ((cx * cx) + (cz * cz));
   fl    =  x_leg [COXA].fl   =  x_leg [THOR].cxz;
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
   /*---(add to leg values)-----------*/
   x_leg [CALC].x   += x;
   x_leg [CALC].z   += z;
   x_leg [CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(calc basics)-----------------*/
   x_leg [TROC].cd   =  x_leg [COXA].cd;
   x_leg [TROC].cv   =  x_leg [COXA].cv;
   x_leg [TROC].ch   =  x_leg [COXA].ch;
   DEBUG_KINE   yLOG_note    ("save basics");
   /*---(calc cums)-------------------*/
   x_leg [TROC].cx   =  x_leg [COXA].cx;
   x_leg [TROC].cz   =  x_leg [COXA].cz;
   x_leg [TROC].cy   =  x_leg [COXA].cy;
   DEBUG_KINE   yLOG_note    ("save endpoints");
   /*---(calc extras)-----------------*/
   x_leg [TROC].cxz  =  x_leg [COXA].cxz;
   x_leg [TROC].fl   =  x_leg [COXA].fl; 
   DEBUG_KINE   yLOG_note    ("save lengths");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      foreward kinematics                     ----===*/
/*====================------------------------------------====================*/
static void      o___FORWARD_________________o (void) {;}

char
yKINE__FK_femu     (int  a_num, float a_deg)
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("a_deg"     , a_deg);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("min deg"   , seg_data [FEMU].min);
   --rce;  if (a_deg < seg_data [FEMU].min) {
      DEBUG_KINE   yLOG_note    ("degree is less than minimum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("max deg"   , seg_data [FEMU].max);
   --rce;  if (a_deg > seg_data [FEMU].max) {
      DEBUG_KINE   yLOG_note    ("degree is greater than maximum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [FEMU].l;
   x_leg [FEMU].d    =  a_deg;
   x_leg [FEMU].v    =  0.0f;
   x_leg [FEMU].h    =  a_deg * DEG2RAD;
   /*---(calc basics)--------------------*/
   d    =  x_leg [FEMU].cd   =  x_leg [TROC].cd + a_deg;
   v    =  x_leg [FEMU].cv   =  0.0f;
   h    =  x_leg [FEMU].ch   =  d * DEG2RAD;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)----------------*/
   x    =  x_leg [FEMU].x    =  l * cos (h);
   z    =  x_leg [FEMU].z    = -l * sin (h);
   y    =  x_leg [FEMU].y    =  0.0f;
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [FEMU].cx   =  x_leg [TROC].cx + x;
   cz   =  x_leg [FEMU].cz   =  x_leg [TROC].cz + z;
   cy   =  x_leg [FEMU].cy   =  x_leg [TROC].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   xz   =  x_leg [FEMU].xz   =  sqrt (( x *  x) + ( z *  z));
   sl   =  x_leg [FEMU].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [FEMU].cxz          =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [FEMU].fl   =  x_leg [FEMU].cxz;
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [CALC].x   += x;
   x_leg [CALC].z   += z;
   x_leg [CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__FK_pate     (int  a_num, float a_deg)
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("a_deg"     , a_deg);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("min deg"   , seg_data [PATE].min);
   --rce;  if (a_deg < seg_data [PATE].min) {
      DEBUG_KINE   yLOG_note    ("degree is less than minimum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("max deg"   , seg_data [PATE].max);
   --rce;  if (a_deg > seg_data [PATE].max) {
      DEBUG_KINE   yLOG_note    ("degree is greater than maximum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [PATE].l;
   x_leg [PATE].d    =  a_deg;
   x_leg [PATE].v    =  a_deg * DEG2RAD; 
   x_leg [PATE].h    =  0.0f;
   /*---(calc basics)--------------------*/
   d    =  x_leg [PATE].cd   =  a_deg;
   v    =  x_leg [PATE].cv   =  a_deg * DEG2RAD;
   h    =  x_leg [PATE].ch   =  x_leg [FEMU].ch;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  x_leg [PATE].y    = -l * sin (v);
   xz   =  x_leg [PATE].xz   =  sqrt (( l *  l) - ( y *  y));
   x    =  x_leg [PATE].x    =  xz * cos (h);
   z    =  x_leg [PATE].z    = -xz * sin (h);
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [PATE].cx   =  x_leg [FEMU].cx + x;
   cz   =  x_leg [PATE].cz   =  x_leg [FEMU].cz + z;
   cy   =  x_leg [PATE].cy   =  x_leg [FEMU].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   sl   =  x_leg [PATE].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [PATE].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [PATE].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1ffl", sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [CALC].x   += x;
   x_leg [CALC].z   += z;
   x_leg [CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE__FK_tibi     (int  a_num, float a_deg)
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("a_deg"     , a_deg);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("min deg"   , seg_data [TIBI].min);
   --rce;  if (a_deg < seg_data [TIBI].min) {
      DEBUG_KINE   yLOG_note    ("degree is less than minimum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("max deg"   , seg_data [TIBI].max);
   --rce;  if (a_deg > seg_data [TIBI].max) {
      DEBUG_KINE   yLOG_note    ("degree is greater than maximum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [TIBI].l;
   x_leg [TIBI].d    =  a_deg;
   x_leg [TIBI].v    =  a_deg * DEG2RAD; 
   x_leg [TIBI].h    =  0.0f;
   /*---(calc basics)--------------------*/
   d    =  x_leg [TIBI].cd   =  a_deg;
   v    =  x_leg [TIBI].cv   =  x_leg [PATE].cv + x_leg [TIBI].v;
   h    =  x_leg [TIBI].ch   =  x_leg [PATE].ch;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  x_leg [TIBI].y    = -l * sin (v);
   xz   =  x_leg [TIBI].xz   =  sqrt (( l *  l) - ( y *  y));
   if (d > 90.0)  xz *= -1.0;
   x    =  x_leg [TIBI].x    =  xz * cos (h);
   z    =  x_leg [TIBI].z    = -xz * sin (h);
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [TIBI].cx   =  x_leg [PATE].cx + x;
   cz   =  x_leg [TIBI].cz   =  x_leg [PATE].cz + z;
   cy   =  x_leg [TIBI].cy   =  x_leg [PATE].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   sl   =  x_leg [TIBI].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [TIBI].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [TIBI].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1ffl", sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [CALC].x   += x;
   x_leg [CALC].z   += z;
   x_leg [CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   --rce;  if (a_num > MAX_LEGS)     return rce;
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * MAX_SEGS);
      /*---(calc basics)-----------------*/
      x_leg [META].cd   =  x_leg [TIBI].cd;
      x_leg [META].cv   =  x_leg [TIBI].cv;
      x_leg [META].ch   =  x_leg [TIBI].ch;
      /*---(calc cums)-------------------*/
      x_leg [META].cx   =  x_leg [TIBI].cx;
      x_leg [META].cz   =  x_leg [TIBI].cz;
      x_leg [META].cy   =  x_leg [TIBI].cy;
      /*---(calc extras)-----------------*/
      x_leg [META].cxz  =  x_leg [TIBI].cxz;
      x_leg [META].fl   =  x_leg [TIBI].fl; 
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
   --rce;  if (a_num > MAX_LEGS)     return rce;
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * MAX_SEGS);
      /*---(calc basics)-----------------*/
      x_leg [TARS].cd   =  x_leg [META].cd;
      x_leg [TARS].cv   =  x_leg [META].cv;
      x_leg [TARS].ch   =  x_leg [META].ch;
      /*---(calc cums)-------------------*/
      x_leg [TARS].cx   =  x_leg [META].cx;
      x_leg [TARS].cz   =  x_leg [META].cz;
      x_leg [TARS].cy   =  x_leg [META].cy;
      /*---(calc extras)-----------------*/
      x_leg [TARS].cxz  =  x_leg [META].cxz;
      x_leg [TARS].fl   =  x_leg [META].fl; 
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
   --rce;  if (a_num > MAX_LEGS)     return rce;
   for (i = 0; i < 2; ++i) {
      /*---(test and set)----------------*/
      if (i == 0)  x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
      if (i == 1)  x_leg = ((tSEG *) ik) + (a_num * MAX_SEGS);
      /*---(calc basics)-----------------*/
      x_leg [FOOT].cd   =  x_leg [TARS].cd;
      x_leg [FOOT].cv   =  x_leg [TARS].cv;
      x_leg [FOOT].ch   =  x_leg [TARS].ch;
      /*---(calc cums)-------------------*/
      x_leg [FOOT].cx   =  x_leg [TARS].cx;
      x_leg [FOOT].cz   =  x_leg [TARS].cz;
      x_leg [FOOT].cy   =  x_leg [TARS].cy;
      /*---(calc extras)-----------------*/
      x_leg [FOOT].cxz  =  x_leg [TARS].cxz;
      x_leg [FOOT].fl   =  x_leg [TARS].fl; 
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     inverse kinematics                       ----===*/
/*====================------------------------------------====================*/
static void      o___INVERSE_________________o (void) {;};

char       /*----: set the leg target ----------------------------------------*/
yKINE__target      (int a_num, float a_x, float a_z, float a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   double      ch, cv, fl;             /*                                     */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("x"         , a_x);
   DEBUG_KINE   yLOG_double  ("z"         , a_z);
   DEBUG_KINE   yLOG_double  ("y"         , a_y);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);    /* trying to use only fk */
   /*---(save cums)----------------------*/
   x    =  x_leg [TARG].cx  =  a_x;
   z    =  x_leg [TARG].cz  =  a_z;
   y    =  x_leg [TARG].cy  =  a_y;
   xz   =  x_leg [TARG].cxz  = sqrt  ((x * x) + (z * z));
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc basics)--------------------*/
   ch   =  x_leg [TARG].ch   = atan2 (z, x);
   cv   =  x_leg [TARG].cv   = atan2 (y, xz);
   fl   =  x_leg [TARG].fl   = sqrt  ((x * x) + (z * z) + (y * y));
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.3fcv, %6.3fch, %6.1ffl",  cv, ch, fl);
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: isolate the leg values ------------------------------------*/
yKINE__IK_femu     (int a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        rc          =   0;      /* generic return code                 */
   double      d;                      /* degrees                             */
   double      x,  y,  z;              /* coordintates                        */
   double      xz;                     /* xz plane length                     */
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);    /* trying to use only fk */
   /*----(check femur orientation)-------*/
   DEBUG_KINE   yLOG_complex ("troc"     , "%6.1fcx, %6.1fcz, %6.1fcd", x_leg [TROC].cx, x_leg [TROC].cz, x_leg [TROC].cd);
   x    =  x_leg [TARG].cx          - x_leg [TROC].cx;
   z    =  x_leg [TARG].cz          - x_leg [TROC].cz;
   d    = (atan2 (-z, x) * RAD2DEG) - x_leg [TROC].cd;
   DEBUG_KINE   yLOG_complex ("femu"     , "%6.1fx , %6.1fc , %6.1fd ", x, z, d);
   /*----(adjust direction)--------------*/
   if (d >  180.0) { d =  d - 360.0;  x_leg [FEMU].u     = '-'; }
   if (d >   90.0) { d =  d - 180.0;  x_leg [FEMU].u     = 'y'; }
   if (d < -180.0) { d =  360.0 + d;  x_leg [FEMU].u     = '-'; }
   if (d <  -90.0) { d =  180.0 + d;  x_leg [FEMU].u     = 'y'; }
   DEBUG_KINE   yLOG_double  ("new d"     , d);
   DEBUG_KINE   yLOG_char    ("femu u"    , x_leg [FEMU].u);
   /*----(save)--------------------------*/
   rc   = yKINE__FK_femu  (a_num, d);
   DEBUG_KINE   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_KINE   yLOG_note    ("fk function failed");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(patella/tibia calcs)------------*/
   x    =  x_leg [LOWR].x    =  x_leg [TARG].cx - x_leg [FEMU].cx;
   z    =  x_leg [LOWR].z    =  x_leg [TARG].cz - x_leg [FEMU].cz;
   y    =  x_leg [LOWR].y    =  x_leg [TARG].cy - x_leg [FEMU].cy;
   xz   =  x_leg [LOWR].xz   =  sqrt  (( x *  x) + ( z *  z));
   DEBUG_KINE   yLOG_complex ("lower"    , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   x_leg [LOWR].sl   =  sqrt  (( x *  x) + ( z *  z) + ( y *  y));
   x_leg [LOWR].v    =  atan2  ( y , xz);
   x_leg [LOWR].h    =  atan2  ( z ,  x);
   x_leg [LOWR].d    =  x_leg [LOWR].v * RAD2DEG;
   DEBUG_KINE   yLOG_complex ("radians"  , "%6.3fv , %6.3fh , %6.1fd ", x_leg [LOWR].v, x_leg [LOWR].h, x_leg [LOWR].d);
   /*---(check tibia orientation)--------*/
   d    = (atan2 (-z, x) * RAD2DEG) - x_leg [COXA].d;
   /*----(adjust direction)--------------*/
   while (round (d) <    0.0) d += 360.0;
   while (round (d) >= 360.0) d -= 360.0;
   DEBUG_KINE   yLOG_double  ("another d" , d);
   if      (d >  180.0) x_leg [TIBI].u     = '-';
   else if (d >   90.0) x_leg [TIBI].u     = 'y';
   else if (d < -180.0) x_leg [TIBI].u     = '-';
   else if (d <  -90.0) x_leg [TIBI].u     = 'y';
   DEBUG_KINE   yLOG_char    ("tibia u"   , x_leg [TIBI].u);
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(patella angle)------------------*/
   dv   =  x_leg [LOWR].d;
   sl   =  x_leg [LOWR].sl;
   pl   =  x_leg [PATE].l;
   tl   =  x_leg [TIBI].l;
   DEBUG_KINE   yLOG_double  ("dv"        , dv);
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((sl * sl) + (pl * pl) - (tl * tl)) / (2.0 * sl * pl));
   d    =  round (a * RAD2DEG);
   if (isnan (a))    d = 0.0;
   DEBUG_KINE   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*---(reorient as needed)-------------*/
   DEBUG_KINE   yLOG_char    ("femu u"    , x_leg [FEMU].u);
   if (x_leg [FEMU].u == 'y') {
      if (x_leg [TIBI].u == 'y')         d = (d  - dv) - 180.0;
      else                               d =  d  - x_leg [LOWR].d;
   } else {
      if (x_leg [LOWR].y > 0.0)          d =  dv + d;
      else if (x_leg [TIBI].u == 'y')    d =  dv - x_leg [LOWR].d;
      else                               d =  fabs (d) - fabs (dv);
   }
   DEBUG_KINE   yLOG_double  ("new d"     , d);
   /*----(save)--------------------------*/
   rc   = yKINE__FK_pate  (a_num, d);
   DEBUG_KINE   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_KINE   yLOG_note    ("fk function failed");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) fk) + (a_num * MAX_SEGS);
   /*---(tibia angle)--------------------*/
   dv   =  x_leg [LOWR].d;
   sl   =  x_leg [LOWR].sl;
   pl   =  x_leg [PATE].l;
   tl   =  x_leg [TIBI].l;
   DEBUG_KINE   yLOG_double  ("dv"        , dv);
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((tl * tl) + (pl * pl) - (sl * sl)  ) / (2.0 * pl * tl));
   a   -=  M_PI;
   d    =  -(round  (a  * RAD2DEG));
   if (isnan (a))    d = 0.0;
   DEBUG_KINE   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*----(save)--------------------------*/
   rc   = yKINE__FK_tibi  (a_num, d);
   DEBUG_KINE   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_KINE   yLOG_note    ("fk function failed");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        full drivers                          ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVERS_________________o (void) {;};

char         /*--> drive the leg position from angles ----[ ------ [ ------ ]-*/
yKINE_forward      (int a_num, float a_femu, float a_pate, float a_tibi)
{
   /*---(fixed body)---------------------*/
   yKINE__thor    (a_num);
   yKINE__coxa    (a_num);
   yKINE__troc    (a_num);
   /*---(movable)------------------------*/
   yKINE__FK_femu (a_num, a_femu);
   yKINE__FK_pate (a_num, a_pate);
   yKINE__FK_tibi (a_num, a_tibi);
   /*---(future)-------------------------*/
   yKINE__meta    (a_num);
   yKINE__tars    (a_num);
   yKINE__foot    (a_num);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
yKINE_inverse      (int a_num, float a_x, float a_z, float a_y)
{
   /*---(target setting)-----------------*/
   yKINE__target  (a_num, a_x, a_z, a_y);
   /*---(fixed body)---------------------*/
   yKINE__thor    (a_num);
   yKINE__coxa    (a_num);
   yKINE__troc    (a_num);
   /*---(movable)------------------------*/
   yKINE__IK_femu (a_num);
   yKINE__IK_pate (a_num);
   yKINE__IK_tibi (a_num);
   /*---(future)-------------------------*/
   yKINE__meta    (a_num);
   yKINE__tars    (a_num);
   yKINE__foot    (a_num);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

#define       LEN_TEXT  2000
char          unit_answer [ LEN_TEXT ];

char       /*----: unit testing setter for unit testing ----------------------*/
yKINE__setlen      (
      double a_thor, double a_coxa, double a_troc,
      double a_femu, double a_pate, double a_tibi,
      double a_meta, double a_tars, double a_foot)
{
   int i = 0;
   for (i = 0; i < MAX_LEGS; ++i) {
      fk [i][THOR].l   = a_thor;
      fk [i][COXA].l   = a_coxa;
      fk [i][TROC].l   = a_troc;
      fk [i][FEMU].l   = a_femu;
      fk [i][PATE].l   = a_pate;
      fk [i][TIBI].l   = a_tibi;
      fk [i][META].l   = a_meta;
      fk [i][TARS].l   = a_tars;
      fk [i][FOOT].l   = a_foot;
   }
   return 0;
}

char       /*----: unit testing setter for unit testing ----------------------*/
yKINE__setter      (char *a_request, int a_leg, int a_seg, double a_value)
{
   char        rce         = -11;
   /*---(defense)-----------------------------------------*/
   --rce;  if (a_leg < 0       )   return rce;
   --rce;  if (a_leg > MAX_LEGS)   return rce;
   --rce;  if (a_seg < 0       )   return rce;
   --rce;  if (a_seg > MAX_SEGS)   return rce;
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

char*      /*----: unit testing accessor for clean validation interface ------*/
yKINE__getter      (char *a_question, int a_leg,  int a_seg)
{
   /*---(defense)-----------------------------------------*/
   if (a_leg < 0 || a_leg > MAX_LEGS) {
      sprintf(unit_answer, "leg value out of range");
      return unit_answer;
   }
   if (a_seg < 0 || a_seg > MAX_SEGS) {
      sprintf(unit_answer, "segment value out of range");
      return unit_answer;
   }
   strncpy (unit_answer, "unknown request", 100);
   /*---(answer)------------------------------------------*/
   if (strcmp(a_question, "IK_final") == 0) {
      sprintf(unit_answer, "IK %1d/final     : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, ik[a_leg][CALC].x, ik[a_leg][CALC].z, ik[a_leg][CALC].y);
   } else if (strcmp(a_question, "FK_final") == 0) {
      sprintf(unit_answer, "FK %1d/final     : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, fk[a_leg][CALC].x, fk[a_leg][CALC].z, fk[a_leg][CALC].y);
   } else if (strcmp(a_question, "IK_target") == 0) {
      sprintf(unit_answer, "IK %1d/target    : %8.2fx,%8.2fz,%8.2fy", 
            a_leg, ik[a_leg][TARG].x, ik[a_leg][TARG].z, ik[a_leg][TARG].y);
   } else if (strcmp(a_question, "IK_match") == 0) {
      if (  ik[a_leg][CALC].x == ik[a_leg][TARG].x &&
            ik[a_leg][CALC].z == ik[a_leg][TARG].z &&
            ik[a_leg][CALC].y == ik[a_leg][TARG].y )
         sprintf(unit_answer, "IK %1d/success", a_leg);
      else
         sprintf(unit_answer, "IK %1d/FAILURE", a_leg);
   } else if (strcmp(a_question, "IK_angles_OLD") == 0) {
      sprintf(unit_answer, "IK %1d/angles    : %8.2fc,%8.2ff,%8.2fp,%8.2ft", 
            a_leg, ik[a_leg][COXA].d, ik[a_leg][FEMU].d,
            ik[a_leg][PATE].d, ik[a_leg][TIBI].d);
   } else if (strcmp(a_question, "IK_lower")  == 0) {
      sprintf(unit_answer, "IK %1d/lower     : %8.3fp,%8.3ft", 
            a_leg, ik[a_leg][PATE].v, ik[a_leg][TIBI].v);
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
   its.logger = yLOG_begin    ("yKINE" , yLOG_SYSTEM, yLOG_QUIET);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yKINE__testloud    (void)
{
   yKINE_debug ('y');
   its.logger = yLOG_begin    ("yKINE" , yLOG_SYSTEM, yLOG_NOISE);
   DEBUG_KINE   yLOG_info     ("yKINE" , yKINE_version   ());
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yKINE__testend     (void)
{
   yKINE_debug ('-');
   yLOG_end      ();
   return 0;
}

