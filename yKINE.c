/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"





char        yKINE_ver     [500];




const    double   DEG2RAD   = M_PI / 180.0;
const    double   RAD2DEG   = 180.0 / M_PI;


tSEG      fk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* forward kinematics        */
tSEG      ik [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* inverse kinematics        */
tSEG      gk [YKINE_MAX_LEGS] [YKINE_MAX_SEGS];    /* opengl kinematics         */

double    s_femu_prev [YKINE_MAX_LEGS];     /* to help smooth/predict femur IK  */



/*---(global characteristics)----------------------*/

struct cLEGDATA
{
   char        full        [25];
   char        two         [ 5];
   char        caps        [ 5];
   double      deg;
} leg_data [YKINE_MAX_LEGS] = {
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
} seg_data [YKINE_MAX_SEGS] = {
   /* ---full------------ --four--- --caps--- ----len ----min ----max --test1 --test2 --test3 */
   { "focus"             , "focu"  , "FOCU"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "core"              , "core"  , "CORE"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "thorax"            , "thor"  , "THOR"  ,  125.0, -360.0,  360.0,   75.0,   75.0,    0.0 },
   { "coxa"              , "coxa"  , "COXA"  ,   30.0,    0.0,    0.0,   25.0,   30.0,    0.0 },
   { "trochanter"        , "troc"  , "TROC"  ,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0 },
   { "femur"             , "femu"  , "FEMU"  ,   30.0,  -85.0,   85.0,   25.0,   30.0,    0.0 },
   { "patella"           , "pate"  , "PATE"  ,   57.0,  -90.0,   45.0,   50.0,   57.0,    0.0 },
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
   if        (a_flag == 'A') {
      yKINE_its.debug   = 'y';
      yKINE_its.unit    = 'y';
   } else if (a_flag == 'y')  {
      yKINE_its.debug   = 'y';
      yKINE_its.unit    = '-';
   } else if (a_flag == 'u')  {
      yKINE_its.debug   = '-';
      yKINE_its.unit    = 'y';
   } else {
      yKINE_its.debug   = '-';
      yKINE_its.unit    = '-';
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
      s_femu_prev [x_leg] = 0.0;
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

char       /*----: clear most recent kinematics working data -----------------*/
yKINE__wipe        (int  a_leg, int a_meth)
{
   /*---(locals)-----------+-----------+-*/
   tSEG       *x_leg       = NULL;
   char        rce         = -10;      /* return code for errors              */
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_leg);
   /*---(defense)------------------------*/
   --rce;  if (a_leg < 0 || a_leg > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_double  ("a_x"       , a_x);
   DEBUG_KINE   yLOG_double  ("a_z"       , a_z);
   DEBUG_KINE   yLOG_double  ("a_y"       , a_y);
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
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
   return 0;
}



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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("leg num"   , a_num);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
      DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
      /*---(calc end coords)-------------*/
      x     =  x_leg [YKINE_THOR].x    =  l * cos (h);
      z     =  x_leg [YKINE_THOR].z    = -l * sin (h);
      y     =  x_leg [YKINE_THOR].y    =  0.0f;
      DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
      /*---(calc cums)-------------------*/
      cx    =  x_leg [YKINE_THOR].cx   =  x_leg [YKINE_CORE].cx + x;
      cz    =  x_leg [YKINE_THOR].cz   =  x_leg [YKINE_CORE].cz + z;
      cy    =  x_leg [YKINE_THOR].cy   =  x_leg [YKINE_CORE].cy + y;
      DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
      /*---(calc extras)-----------------*/
      xz    =  x_leg [YKINE_THOR].xz   =  sqrt (( x *  x) + ( z *  z));
      sl    =  x_leg [YKINE_THOR].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
      x_leg [YKINE_THOR].cxz  =  sqrt ((cx * cx) + (cz * cz));
      fl    =  x_leg [YKINE_THOR].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
      DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
      /*---(add to leg values)-----------*/
      x_leg [YKINE_CALC].x   += x;
      x_leg [YKINE_CALC].z   += z;
      x_leg [YKINE_CALC].y   += y;
      DEBUG_KINE   yLOG_note    ("add values to YKINE_CALC segment");
      /*---(done)------------------------*/
   }
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
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
      DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
      /*---(calc end coords)-------------*/
      x     =  x_leg [YKINE_COXA].x    =  l * cos (h);
      z     =  x_leg [YKINE_COXA].z    = -l * sin (h);
      y     =  x_leg [YKINE_COXA].y    =  0.0f;
      DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
      /*---(calc cums)-------------------*/
      cx    =  x_leg [YKINE_COXA].cx   =  x_leg [YKINE_THOR].cx + x;
      cz    =  x_leg [YKINE_COXA].cz   =  x_leg [YKINE_THOR].cz + z;
      cy    =  x_leg [YKINE_COXA].cy   =  x_leg [YKINE_THOR].cy + y;
      DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
      /*---(calc extras)-----------------*/
      xz    =  x_leg [YKINE_COXA].xz   =  sqrt (( x *  x) + ( z *  z));
      sl    =  x_leg [YKINE_COXA].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
      x_leg [YKINE_COXA].cxz           =  sqrt ((cx * cx) + (cz * cz));
      fl    =  x_leg [YKINE_COXA].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
      DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
      /*---(add to leg values)-----------*/
      x_leg [YKINE_CALC].x   += x;
      x_leg [YKINE_CALC].z   += z;
      x_leg [YKINE_CALC].y   += y;
      DEBUG_KINE   yLOG_note    ("add values to YKINE_CALC segment");
      /*---(done)------------------------*/
   }
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
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
      DEBUG_KINE   yLOG_note    ("save basics");
      /*---(calc cums)-------------------*/
      x_leg [YKINE_TROC].cx   =  x_leg [YKINE_COXA].cx;
      x_leg [YKINE_TROC].cz   =  x_leg [YKINE_COXA].cz;
      x_leg [YKINE_TROC].cy   =  x_leg [YKINE_COXA].cy;
      DEBUG_KINE   yLOG_note    ("save endpoints");
      /*---(calc extras)-----------------*/
      x_leg [YKINE_TROC].cxz  =  x_leg [YKINE_COXA].cxz;
      x_leg [YKINE_TROC].fl   =  x_leg [YKINE_COXA].fl; 
      DEBUG_KINE   yLOG_note    ("save lengths");
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   double      x_forgive   = 0.1;
   tSEG       *x_leg       = NULL;
   /*---(header)-------------------------*/
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_KINE   yLOG_info    ("a_meth"    , (a_meth == YKINE_FK) ? "FK" : "IK");
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_meth == YKINE_IK)   x_forgive = 0.5;
   DEBUG_KINE   yLOG_double  ("IK forgive", x_forgive);
   DEBUG_KINE   yLOG_double  ("min deg"   , seg_data [YKINE_FEMU].min);
   --rce;  if (a_deg <= seg_data [YKINE_FEMU].min - x_forgive) {
      DEBUG_KINE   yLOG_note    ("degree is less than minimum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("max deg"   , seg_data [YKINE_FEMU].max);
   --rce;  if (a_deg >= seg_data [YKINE_FEMU].max + x_forgive) {
      DEBUG_KINE   yLOG_note    ("degree is greater than maximum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fv , %6.3fh ", l, d, v, h);
   /*---(calc end coords)----------------*/
   x    =  x_leg [YKINE_FEMU].x    =  l * cos (h);
   z    =  x_leg [YKINE_FEMU].z    = -l * sin (h);
   y    =  x_leg [YKINE_FEMU].y    =  0.0f;
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy ",  x,  z,  y);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [YKINE_FEMU].cx   =  x_leg [YKINE_TROC].cx + x;
   cz   =  x_leg [YKINE_FEMU].cz   =  x_leg [YKINE_TROC].cz + z;
   cy   =  x_leg [YKINE_FEMU].cy   =  x_leg [YKINE_TROC].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   xz   =  x_leg [YKINE_FEMU].xz   =  sqrt (( x *  x) + ( z *  z));
   sl   =  x_leg [YKINE_FEMU].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [YKINE_FEMU].cxz          =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [YKINE_FEMU].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fxz, %6.1fsl, %6.1ffl", xz, sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [YKINE_CALC].x   += x;
   x_leg [YKINE_CALC].z   += z;
   x_leg [YKINE_CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to YKINE_CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_value   ("a_meth"    , a_meth);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_complex ("lower"    , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   x_leg [YKINE_LOWR].sl   =  sqrt  (( x *  x) + ( z *  z) + ( y *  y));
   x_leg [YKINE_LOWR].l    =  x_leg [YKINE_LOWR].sl;
   x_leg [YKINE_LOWR].v    =  atan2  (-y , xz);
   x_leg [YKINE_LOWR].h    =  atan2  ( z ,  x);
   x_leg [YKINE_LOWR].d    =  x_leg [YKINE_LOWR].v * RAD2DEG;
   DEBUG_KINE   yLOG_complex ("radians"  , "%6.3fv , %6.3fh , %6.1fd ", x_leg [YKINE_LOWR].v, x_leg [YKINE_LOWR].h, x_leg [YKINE_LOWR].d);
   /*---(check tibia orientation)--------*/
   d    = (atan2 (-z, x) * RAD2DEG) - x_leg [YKINE_COXA].d;
   /*----(adjust direction)--------------*/
   while (round (d) <    0.0) d += 360.0;
   while (round (d) >= 360.0) d -= 360.0;
   DEBUG_KINE   yLOG_double  ("another d" , d);
   if      (d >  180.0) x_leg [YKINE_TIBI].u     = '-';
   else if (d >   90.0) x_leg [YKINE_TIBI].u     = 'y';
   else if (d < -180.0) x_leg [YKINE_TIBI].u     = '-';
   else if (d <  -90.0) x_leg [YKINE_TIBI].u     = 'y';
   else                 x_leg [YKINE_TIBI].u     = '-';
   DEBUG_KINE   yLOG_char    ("tibia u"   , x_leg [YKINE_TIBI].u);
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_KINE   yLOG_value   ("a_meth"    , a_meth);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_meth == YKINE_IK)   x_forgive = 0.5;
   DEBUG_KINE   yLOG_double  ("IK forgive", x_forgive);
   DEBUG_KINE   yLOG_double  ("min deg"   , seg_data [YKINE_PATE].min);
   --rce;  if (a_deg <= seg_data [YKINE_PATE].min - x_forgive) {
      DEBUG_KINE   yLOG_note    ("degree is less than minimum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("max deg"   , seg_data [YKINE_PATE].max);
   --rce;  if (a_deg >= seg_data [YKINE_PATE].max + x_forgive) {
      DEBUG_KINE   yLOG_note    ("degree is greater than maximum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   v    =  x_leg [YKINE_PATE].cv   =  a_deg * DEG2RAD;
   h    =  x_leg [YKINE_PATE].ch   =  x_leg [YKINE_FEMU].ch;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  x_leg [YKINE_PATE].y    = -l * sin (v);
   xz   =  x_leg [YKINE_PATE].xz   =  sqrt (( l *  l) - ( y *  y));
   x    =  x_leg [YKINE_PATE].x    =  xz * cos (h);
   z    =  x_leg [YKINE_PATE].z    = -xz * sin (h);
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [YKINE_PATE].cx   =  x_leg [YKINE_FEMU].cx + x;
   cz   =  x_leg [YKINE_PATE].cz   =  x_leg [YKINE_FEMU].cz + z;
   cy   =  x_leg [YKINE_PATE].cy   =  x_leg [YKINE_FEMU].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   sl   =  x_leg [YKINE_PATE].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [YKINE_PATE].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [YKINE_PATE].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1ffl", sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [YKINE_CALC].x   += x;
   x_leg [YKINE_CALC].z   += z;
   x_leg [YKINE_CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to YKINE_CALC segment");
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_KINE   yLOG_value   ("a_meth"    , a_meth);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_meth == YKINE_IK)   x_forgive = 0.5;
   DEBUG_KINE   yLOG_double  ("IK forgive", x_forgive);
   DEBUG_KINE   yLOG_double  ("min deg"   , seg_data [YKINE_TIBI].min);
   --rce;  if (a_deg <= seg_data [YKINE_TIBI].min - x_forgive) {
      DEBUG_KINE   yLOG_note    ("degree is less than minimum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_KINE   yLOG_double  ("max deg"   , seg_data [YKINE_TIBI].max);
   --rce;  if (a_deg >= seg_data [YKINE_TIBI].max + x_forgive) {
      DEBUG_KINE   yLOG_note    ("degree is greater than maximum");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (a_num * YKINE_MAX_SEGS);
   if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(save basics)--------------------*/
   l    =  x_leg [YKINE_TIBI].l;
   x_leg [YKINE_TIBI].d    =  a_deg;
   x_leg [YKINE_TIBI].v    =  a_deg * DEG2RAD; 
   x_leg [YKINE_TIBI].h    =  0.0f;
   /*---(calc basics)--------------------*/
   d    =  x_leg [YKINE_TIBI].cd   =  a_deg;
   v    =  x_leg [YKINE_TIBI].cv   =  x_leg [YKINE_PATE].cv + x_leg [YKINE_TIBI].v;
   h    =  x_leg [YKINE_TIBI].ch   =  x_leg [YKINE_PATE].ch;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.1fm , %6.1fd , %6.3fcv, %6.3fch", l, d, v, h);
   /*---(calc end coords)----------------*/
   y    =  x_leg [YKINE_TIBI].y    = -l * sin (v);
   xz   =  x_leg [YKINE_TIBI].xz   =  sqrt (( l *  l) - ( y *  y));
   if (x_leg [YKINE_TIBI].cv > 90.0 * DEG2RAD)  xz *= -1.0;
   x    =  x_leg [YKINE_TIBI].x    =  xz * cos (h);
   z    =  x_leg [YKINE_TIBI].z    = -xz * sin (h);
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc cums)----------------------*/
   cx   =  x_leg [YKINE_TIBI].cx   =  x_leg [YKINE_PATE].cx + x;
   cz   =  x_leg [YKINE_TIBI].cz   =  x_leg [YKINE_PATE].cz + z;
   cy   =  x_leg [YKINE_TIBI].cy   =  x_leg [YKINE_PATE].cy + y;
   DEBUG_KINE   yLOG_complex ("endpoint" , "%6.1fcx, %6.1fcz, %6.1fcy", cx, cz, cy);
   /*---(calc extras)--------------------*/
   sl   =  x_leg [YKINE_TIBI].sl   =  sqrt (( x *  x) + ( z *  z) + (y * y));
   x_leg [YKINE_TIBI].cxz  =  sqrt ((cx * cx) + (cz * cz));
   fl   =  x_leg [YKINE_TIBI].fl   =  sqrt ((cx * cx) + (cz * cz) + (cy * cy));
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1ffl", sl, fl);
   /*---(add to leg values)--------------*/
   x_leg [YKINE_CALC].x   += x;
   x_leg [YKINE_CALC].z   += z;
   x_leg [YKINE_CALC].y   += y;
   DEBUG_KINE   yLOG_note    ("add values to YKINE_CALC segment");
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
yKINE__targ        (int a_num, int a_meth)
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
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*> ch = x_leg [YKINE_TARG].h   =  x_leg [YKINE_TARG].ch  =  x_leg [YKINE_TIBI].ch;   <*/
   /*> cv = x_leg [YKINE_TARG].v   =  x_leg [YKINE_TARG].cv  =  x_leg [YKINE_TIBI].cv;   <*/
   fl = x_leg [YKINE_TARG].sl  =  x_leg [YKINE_TARG].fl  =  x_leg [YKINE_TIBI].fl;
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.3fcv, %6.3fch, %6.1ffl",  cv, ch, fl);
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_num);
   DEBUG_KINE   yLOG_double  ("x"         , a_x);
   DEBUG_KINE   yLOG_double  ("z"         , a_z);
   DEBUG_KINE   yLOG_double  ("y"         , a_y);
   /*---(defense)------------------------*/
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(save cums)----------------------*/
   x    =  x_leg [YKINE_TARG].x   =  x_leg [YKINE_TARG].cx  =  a_x;
   z    =  x_leg [YKINE_TARG].z   =  x_leg [YKINE_TARG].cz  =  a_z;
   y    =  x_leg [YKINE_TARG].y   =  x_leg [YKINE_TARG].cy  =  a_y;
   xz   =  x_leg [YKINE_TARG].xz  =  x_leg [YKINE_TARG].cxz = sqrt  ((x * x) + (z * z));
   DEBUG_KINE   yLOG_complex ("segment"  , "%6.1fx , %6.1fz , %6.1fy , %6.1fxz",  x,  z,  y, xz);
   /*---(calc basics)--------------------*/
   /*> ch   =  x_leg [YKINE_TARG].h    =  x_leg [YKINE_TARG].ch   = atan2 (-z, x);           <* 
    *> if (ch < 0.0) {                                                                       <* 
    *>    ch   =  x_leg [YKINE_TARG].h    =  x_leg [YKINE_TARG].ch   =  (M_PI * 2.0) + ch;   <* 
    *> }                                                                                     <* 
    *> cv   =  x_leg [YKINE_TARG].v    =  x_leg [YKINE_TARG].cv   = atan2 (-y, xz);          <*/
   fl   =  x_leg [YKINE_TARG].sl   =  x_leg [YKINE_TARG].fl   = sqrt  ((x * x) + (z * z) + (y * y));
   DEBUG_KINE   yLOG_complex ("basics"   , "%6.3fcv, %6.3fch, %6.1ffl",  cv, ch, fl);
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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
   char        x_saved     = '-';
   /*---(header)-------------------------*/
   DEBUG_KINE   yLOG_enter   (__FUNCTION__);
   DEBUG_KINE   yLOG_value   ("a_leg"     , a_leg);
   /*---(defense)------------------------*/
   --rce;  if (a_leg < 0 || a_leg > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_leg * YKINE_MAX_SEGS);
   /*----(check femur orientation)-------*/
   DEBUG_KINE   yLOG_complex ("troc"     , "%6.1fcx, %6.1fcz, %6.1fcd", x_leg [YKINE_TROC].cx, x_leg [YKINE_TROC].cz, x_leg [YKINE_TROC].cd);
   x    =  x_leg [YKINE_TARG].cx          - x_leg [YKINE_TROC].cx;
   z    =  x_leg [YKINE_TARG].cz          - x_leg [YKINE_TROC].cz;
   xz   =  sqrt  ((x * x) + (z * z));
   /*> if (fabs (xz) <= 10.0) {                                                       <* 
    *>    x_saved = 'Y';                                                              <* 
    *>    d    =  s_femu_prev [a_leg];                                                <* 
    *> } else {                                                                       <*/
      x_saved = '-';
      d    = (atan2 (-z, x) * RAD2DEG) - x_leg [YKINE_TROC].cd;
      s_femu_prev [a_leg] = d;
   /*> }                                                                              <*/
   DEBUG_KINE   yLOG_complex ("femu"     , "%6.1fx , %6.1fz , %6.1fxz, saved=%c, %8.3fd ", x, z, xz, x_saved, d);
   /*----(adjust direction)--------------*/
   while (d  <=  -360.0) d += 360.0;
   while (d  >=   360.0) d -= 360.0;
   DEBUG_KINE   yLOG_double  ("fixed d"   , d);
   /*----(adjust direction)--------------*/
   /*> if      (d >  180.0) { d =  d - 360.0;  x_leg [YKINE_FEMU].u     = '-'; }      <* 
    *> else if (d >   90.0) { d =  d - 180.0;  x_leg [YKINE_FEMU].u     = 'y'; }      <* 
    *> else if (d < -180.0) { d =  360.0 + d;  x_leg [YKINE_FEMU].u     = '-'; }      <* 
    *> else if (d <  -90.0) { d =  180.0 + d;  x_leg [YKINE_FEMU].u     = 'y'; }      <* 
    *> else                                    x_leg [YKINE_FEMU].u     = '-';        <*/
   /*----(adjust direction)--------------*/
   if      (d >  270.0) { d = d - 360.0;  x_leg [YKINE_FEMU].u     = '-'; }
   if      (d >   90.0) { d = d - 180.0;  x_leg [YKINE_FEMU].u     = 'y'; }
   else if (d >    0.0) { d = d        ;  x_leg [YKINE_FEMU].u     = '-'; }
   else if (d >  -90.0) { d = d        ;  x_leg [YKINE_FEMU].u     = '-'; }
   else if (d > -270.0) { d = 180.0 + d;  x_leg [YKINE_FEMU].u     = 'y'; }
   else                 { d = 360.0 + d;  x_leg [YKINE_FEMU].u     = '-'; }     
   DEBUG_KINE   yLOG_double  ("new d"     , d);
   DEBUG_KINE   yLOG_char    ("femu u"    , x_leg [YKINE_FEMU].u);
   /*----(save)--------------------------*/
   rc   = yKINE__femu     (a_leg, d, YKINE_IK);
   DEBUG_KINE   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_KINE   yLOG_note    ("calc function failed");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rc;
   }
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
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(patella angle)------------------*/
   dv   =  x_leg [YKINE_LOWR].d;
   sl   =  x_leg [YKINE_LOWR].sl;
   pl   =  x_leg [YKINE_PATE].l;
   tl   =  x_leg [YKINE_TIBI].l;
   DEBUG_KINE   yLOG_double  ("dv"        , dv);
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((sl * sl) + (pl * pl) - (tl * tl)) / (2.0 * sl * pl));
   d    =  (double) round (a * RAD2DEG);
   if (isnan (a))    d = 0.0;
   DEBUG_KINE   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*---(reorient as needed)-------------*/
   DEBUG_KINE   yLOG_char    ("femu u"    , x_leg [YKINE_FEMU].u);
   DEBUG_KINE   yLOG_char    ("tibi u"    , x_leg [YKINE_TIBI].u);
   DEBUG_KINE   yLOG_double  ("lowr y"    , x_leg [YKINE_LOWR].y);
   if (x_leg [YKINE_FEMU].u == 'y') {
      if (x_leg [YKINE_TIBI].u == 'y')         d = (d  - dv) - 180.0;
      else                                     d =  d  - dv;
   } else {
      if (x_leg [YKINE_LOWR].y > 0.0)          d =  dv + d;
      else if (x_leg [YKINE_TIBI].u == 'y')    d =  fabs (dv) - fabs (d);
      else                                     d =  fabs (d)  - fabs (dv);
   }
   DEBUG_KINE   yLOG_double  ("if yy"     , (d - dv) - 180.0);
   DEBUG_KINE   yLOG_double  ("if y-"     , d  - dv);
   DEBUG_KINE   yLOG_double  ("if pos y"  , dv + d);
   DEBUG_KINE   yLOG_double  ("if -y"     , dv - d);
   DEBUG_KINE   yLOG_double  ("if -y fabs", (double) fabs (dv) - fabs (d) );
   DEBUG_KINE   yLOG_double  ("if --"     , (double) fabs (d)  - fabs (dv));
   DEBUG_KINE   yLOG_double  ("new d"     , d);
   /*----(save)--------------------------*/
   rc   = yKINE__pate     (a_num, d, YKINE_IK);
   DEBUG_KINE   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_KINE   yLOG_note    ("calc function failed");
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
   --rce;  if (a_num < 0 || a_num > YKINE_MAX_LEGS) {
      DEBUG_KINE   yLOG_note    ("leg number is out of range");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(test and set)-------------------*/
   x_leg = ((tSEG *) ik) + (a_num * YKINE_MAX_SEGS);
   /*---(tibia angle)--------------------*/
   dv   =  x_leg [YKINE_LOWR].d;
   sl   =  x_leg [YKINE_LOWR].sl;
   pl   =  x_leg [YKINE_PATE].l;
   tl   =  x_leg [YKINE_TIBI].l;
   DEBUG_KINE   yLOG_double  ("dv"        , dv);
   DEBUG_KINE   yLOG_complex ("lengths"  , "%6.1fsl, %6.1fpl, %6.1ftl",  sl, pl, tl);
   a    =  acos (((tl * tl) + (pl * pl) - (sl * sl)  ) / (2.0 * pl * tl));
   a   -=  M_PI;
   d    =  -(round  (a  * RAD2DEG));
   if (isnan (a))    d = 0.0;
   DEBUG_KINE   yLOG_complex ("arccos"   , "%6.3fa , %6.3fd ", a, d);
   /*----(save)--------------------------*/
   rc   = yKINE__tibi     (a_num, d, YKINE_IK);
   DEBUG_KINE   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_KINE   yLOG_note    ("calc function failed");
      DEBUG_KINE   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_KINE   yLOG_exit    (__FUNCTION__);
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



/*====================------------------------------------====================*/
/*===----                        full drivers                          ----===*/
/*====================------------------------------------====================*/
static void      o___MAIN____________________o (void) {;};

char         /*--> drive the leg position from angles ----[ ------ [ ------ ]-*/
yKINE_forward      (int a_num, double a_femu, double a_pate, double a_tibi)
{
   /*---(clear)--------------------------*/
   yKINE__wipe     (a_num, YKINE_FK);
   /*---(fixed body)---------------------*/
   yKINE__thor     (a_num);
   yKINE__coxa     (a_num);
   yKINE__troc     (a_num);
   /*---(movable)------------------------*/
   yKINE__femu     (a_num, a_femu, YKINE_FK);
   yKINE__pate     (a_num, a_pate, YKINE_FK);
   yKINE__tibi     (a_num, a_tibi, YKINE_FK);
   /*---(target setting)-----------------*/
   yKINE__targ     (a_num, YKINE_FK);
   yKINE__lowr     (a_num, YKINE_FK);
   /*---(future)-------------------------*/
   yKINE__meta     (a_num);
   yKINE__tars     (a_num);
   yKINE__foot     (a_num);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> drive the leg position to a target ----[ ------ [ ------ ]-*/
yKINE_inverse      (int a_num, double a_x, double a_z, double a_y)
{
   /*---(clear)--------------------------*/
   yKINE__wipe     (a_num, YKINE_IK);
   /*---(target setting)-----------------*/
   yKINE__IK_targ  (a_num, a_x, a_z, a_y);
   /*---(fixed body)---------------------*/
   yKINE__thor     (a_num);
   yKINE__coxa     (a_num);
   yKINE__troc     (a_num);
   /*---(movable)------------------------*/
   yKINE__IK_femu  (a_num);
   yKINE__lowr     (a_num, YKINE_IK);
   yKINE__IK_pate  (a_num);
   yKINE__IK_tibi  (a_num);
   /*---(future)-------------------------*/
   yKINE__meta     (a_num);
   yKINE__tars     (a_num);
   yKINE__foot     (a_num);
   /*---(complete)-----------------------*/
   return 0;
}



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
yKINE_angles       (double a_leg, double *a_coxa, double *a_femu, double *a_pate, double *a_tibi)
{
   if (a_coxa != NULL)  *a_coxa = fk [(int) a_leg][YKINE_COXA].d;
   if (a_femu != NULL)  *a_femu = fk [(int) a_leg][YKINE_FEMU].d;
   if (a_pate != NULL)  *a_pate = fk [(int) a_leg][YKINE_PATE].d;
   if (a_tibi != NULL)  *a_tibi = fk [(int) a_leg][YKINE_TIBI].d;
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

