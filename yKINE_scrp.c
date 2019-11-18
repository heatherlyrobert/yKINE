/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


#define    PARSE_ANGLE        'a'
#define    PARSE_POINT        'e'
#define    PARSE_MUSIC        'c'
#define    PARSE_NONE         '-'

#define        FAILED    -666.0


#define    MAX_VERBS       100
typedef    struct  cVERBS  tVERBS;
struct cVERBS {
   char        code;                        /* ref id of verb                 */
   char        terse       [LEN_LABEL];     /* short, common name             */
   char        name        [LEN_LABEL];     /* full verb name                 */
   char        active;                      /* allowed for use                */
   char        servo;                       /* has a servo specifier field    */
   char        target;                      /* leg/seg targeting override     */
   char        from;                        /* pure vs from coding            */
   char        style;                       /* normal (xzy) vs polar (dly)    */
   char        mask;                        /* masked field for yPARSE reload */
   char        (*call)     (void);          /* function pointer               */
   char        desc        [LEN_HUND ];     /* english description            */
};
tVERBS   s_verb_info    [MAX_VERBS] = {
   /* ===[[ forward kinematics ]]================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_FK  , "fk"   , "forward"      , 'y' , 'y' , YKINE_FORWARD, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_fk         , "set absolute joint angles on all three joints"     },
   /* ===[[ inverse kinematics ]]================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_IK  , "ik"   , "inverse"      , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_ik         , "set an exact endpoint in 3d space"                 },
   { YKINE_TK  , "tk"   , "tangent"      , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_tk         , "set an exact endpoint tangential to leg"           },
   { YKINE_CK  , "ck"   , "center"       , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_POLAR ,  1, ykine_legs_ck         , "set absolute joint angles on all three joints"     },
   { YKINE_RK  , "rk"   , "radial"       , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_POLAR ,  1, ykine_legs_rk         , "set absolute joint angles on all three joints"     },
   { YKINE_SK  , "sk"   , "step"         , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_POLAR ,  1, ykine_legs_sk         , "set absolute joint angles on all three joints"     },
   { YKINE_NK  , "nk"   , "neighbor"     , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_nk         , "set exact endpoint relative to military"           },
   /* ===[[ body zero-point ]]===================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_ZE  , "ze"   , "zero"         , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, YKINE_LINEAR, -1, ykine_body_zero       , "set absolute body position in 3d space"            },
   { YKINE_PO  , "po"   , "polar"        , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, YKINE_POLAR , -1, ykine_body_polar      , "set relative body position based on last position" },
   /* ===[[ body orientation ]]==================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_OR  , "or"   , "orient"       , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, YKINE_LINEAR, -1, ykine_body_orient     , "set absolute body orientation angles"              },
   { YKINE_TI  , "ti"   , "tilt"         , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, YKINE_POLAR , -1, ykine_body_tilt       , "set relative body position based on last position" },
   { YKINE_ST  , "st"   , "stance"       , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, YKINE_POLAR , -1, ykine_stance          , "set relative body position based on last position" },
   /* ===[[ music notation ]]====================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { -1        , "metr" , "meter"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_repeat     , "time signature for rhythm/beat"                    },
   { -1        , "tmpo" , "tempo"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_repeat     , "pace of music in beats-per-minute"                 },
   /* ===[[ flow control ]]======================================================*/
   { YKINE_SEGN, "segn" , "segno"        , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_segno      , "flow control beginning of a repeat group"          },
   { YKINE_REPT, "rept" , "ripetere"     , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_repeat     , "flow control ending of a repeat group"             },
   /* ===[[ minor/partial sync points ]]=========================================*/
   { YKINE_PHRA, "phra" , "frase"        , '-' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "mark level 1 (smallest) partial/minor sync point"  },
   { YKINE_STZA, "stnz" , "stanza"       , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "mark level 2 (largest) partial/minor sync point"   },
   /* ===[[ major/full sync points ]]============================================*/
   { YKINE_PASS, "pass" , "passaggio"    , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_section    , "mark level 3 (smallest) full/major sync point"     },
   { YKINE_SECT, "sect" , "sezione"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_section    , "mark level 4 (largest) full/major sync point"      },
   { YKINE_PASS, "ini"  , "inizio"       , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_section    , "mark beginning of script"                          },
   { YKINE_PASS, "fin"  , "finire"       , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_scrp_section    , "mark ending of script"                             },
   /* ===[[ external ]]==========================================================*/
   { YKINE_SONG, "song" , "song"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "insert external script to allow reuse"             },
   /* ===[[ turtle graphics ]]===================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_TSP , "tsp"  , "tu_speed"     , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_speed    , "set the speed of the turtle movement"              },
   { YKINE_TWA , "twa"  , "tu_wait"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_wait     , "stop action for a specific period"                 },
   { YKINE_THM , "thm"  , "tu_home"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_home     , "move the turtle to 0x, 0z at the same height"      },
   { YKINE_TMV , "tmv"  , "tu_move"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_move     , "move the turtle on existing direction"             },
   { YKINE_TGO , "tgo"  , "tu_goto"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_goto     , "time signature for rhythm/beat"                    },
   { YKINE_THE , "the"  , "tu_head"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_head     , "change the absolute direction of the turtle"       },
   { YKINE_TTU , "ttu"  , "tu_turn"      , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_turn     , "adjust the direction of the turtle from current"   },
   { YKINE_TLO , "tlo"  , "tu_lower"     , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_lower    , "time signature for rhythm/beat"                    },
   { YKINE_TRA , "tra"  , "tu_raise"     , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_raise    , "time signature for rhythm/beat"                    },
   { YKINE_TDE , "tde"  , "tu_depth"     , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_depth    , "time signature for rhythm/beat"                    },
   { YKINE_TPU , "tpu"  , "tu_push"      , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "time signature for rhythm/beat"                    },
   { YKINE_TPO , "tpo"  , "tu_pop"       , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "time signature for rhythm/beat"                    },
   /* ===[[ gait framework ]]====================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { -1        , "incp" , "gait_beg"     , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_beg        , "beginning of a gait description"                   },
   { -1        , "incp" , "incipio"      , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_beg        , "beginning of a gait description"                   },
   { -1        , "comp" , "gait_end"     , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_end        , "end of a gait description"                         },
   { -1        , "comp" , "compleo"      , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_end        , "end of a gait description"                         },
   /* ===[[ complex gaits ]]=====================================================*/
   /* constant , terse-  verb----------- actv- servo targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { -1        , "walk" , "walk"         , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "repeat a specific number of steps"                 },
   { -1        , "circ" , "circle"       , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "repeat a specific number of steps"                 },
   /* ===[[ special ]]===========================================================*/
   { YKINE_BODE, "e/bod", "error_body"   , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, YKINE_LINEAR, -1, "-"                   , "verb not understood"                               },
   { YKINE_LEGE, "e/leg", "error_legs"   , 'y' , '-' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, "-"                   , "verb not understood"                               },
   { YKINE_NOOP, "noop" , "empty"        , 'y' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, "-"                   , "nothing to do"                                     },
   /* done-------------------*/
   { -1        , NULL   , NULL           ,  0  ,  0  , -1           , -1        , -1          , -1, NULL                  , NULL                                                },
};


#define    MAX_SCRPARG      100
typedef    struct  cSCRPARG   tSCRPARG;
struct cSCRPARG {
   char        name        [LEN_LABEL];
   char        flag;
   char        desc        [LEN_STR  ];
};
tSCRPARG   s_scrparg [MAX_SCRPARG] = {
   /*---(fixed positions)----------------*/
   /* label--------   cnt   description ------------------------------------- */
   { "attn"         , '-',  "IK position calculations relative to attention"  },
   { "crab"         , '-',  "IK position calculations relative to crab pose"  },
   /*---(reflection)---------------------*/
   /* label--------   cnt   description ------------------------------------- */
   { "R2L"          , '-',  "reflect movements of right legs to left"         },
   { "xR2L"         , '-',  "reflect x increments from right to left"         },
   { "zR2L"         , '-',  "reflect z increments from right to left"         },
   { "F2R"          , '-',  "reflect movements of front legs to rear"         },
   { "xF2R"         , '-',  "reflect x increments from front to rear"         },
   { "zF2R"         , '-',  "reflect z increments from front to rear"         },
   /* label--------   cnt   description ------------------------------------- */
   { "end-of-list"  , '-',  ""                                                },
};


#define     MAX_GAITS       100
typedef     struct cGAIT    tGAIT;
struct cGAIT {
   char        name        [LEN_LABEL];
   float       xpos        [12];
   float       zpos        [12];
   float       ypos        [12];
};
tGAIT       s_gait_info [MAX_GAITS] = {
   /* label--------------- , */
   {  "crab_left"          , /* x */   0.0, -45.0, -40.0, -30.0, -20.0, -10.0,   0.0,  10.0,  20.0,  30.0,  40.0,  45.0,
      /*------------------*/ /* z */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      /*------------------*/ /* y */  10.0,   5.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   5.0},
   {  "end-of-list"        , /* x */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      /*------------------*/ /* z */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      /*------------------*/ /* y */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0},
};

#define     MAX_HINTS       676
static struct cHINTS {
   char        major;
   char        minor;
   int         line;
   char       *label;
   float       secs;
} s_hints [MAX_HINTS];
static int  s_nhint   =   0;



/*====================------------------------------------====================*/
/*===----                         file access                          ----===*/
/*====================------------------------------------====================*/
static void      o___ACCESS__________________o (void) {;}

char
ykine_verb_init         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(verbs)--------------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      if (s_verb_info [i].name  [0] == NULL) break;
      if (s_verb_info [i].active   != 'y' )  continue;
      DEBUG_YKINE_SCRP   yLOG_info    ("verb"      , s_verb_info [i].name);
      yPARSE_handler ('·', s_verb_info [i].name, 0.0, "", s_verb_info [i].mask, NULL, NULL, "", "", s_verb_info [i].desc);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*> char         /+--> locate a servo entry ------------------[ ------ [ ------ ]-+/               <* 
 *> yKINE_servo        (char *a_source)                                                            <* 
 *> {                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                    <* 
 *>    int         i           = 0;                                                                <* 
 *>    /+---(cycle)--------------------------+/                                                    <* 
 *>    /+> printf ("start check\n");                                                      <+/      <* 
 *>    for (i = 0; i < YKINE_MAX_SERVO; ++i) {                                                     <* 
 *>       if (g_servo_info [i].label [0] == 'e')   break;                                          <* 
 *>       /+> printf ("checking %s¦", g_servo_info[i].label);                                <+/   <* 
 *>       if (a_source [0] != g_servo_info [i].label [0])       continue;                          <* 
 *>       if (strcmp (a_source, g_servo_info [i].label) != 0)   continue;                          <* 
 *>       return i;                                                                                <* 
 *>    }                                                                                           <* 
 *>    return -1;                                                                                  <* 
 *> }                                                                                              <*/

char  /*--> prepare for use ---------s-------------[ leaf   [ ------ ]-*/
ykine_scrp_begin   (void)
{
   myKINE.scrp_len    = 0.0;
   myKINE.s_nline     =   0;
   myKINE.s_cline     =   0;
   return 0;
}

char  /* file reading driver ----------------------[--------[--------]-*/
ykine__scrp_prep   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(call clears)--------------------*/
   ykine_servo_prep  ();
   for (i = 0; i < MAX_SCRPARG; ++i) {
      if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
      s_scrparg [i].flag = '-';
   }
   /*---(reset globals)------------------*/
   strlcpy (myKINE.s_verb, "", LEN_LABEL);
   myKINE.s_hidden    = '-';
   myKINE.s_iverb     = -1;
   myKINE.s_targ      = '-';
   myKINE.s_from      = '-';
   myKINE.s_count     = -1;
   myKINE.s_beats     = 0.0;
   myKINE.s_secs      = 0.0;
   myKINE.s_femu      = FAILED;
   myKINE.s_pate      = FAILED;
   myKINE.s_tibi      = FAILED;
   myKINE.s_xpos      = FAILED;
   myKINE.s_zpos      = FAILED;
   myKINE.s_ypos      = FAILED;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> return verb info based on code --------[ ------ [ ------ ]-*/
ykine_scrp_by_code      (char a_code, char *a_terse, char *a_name, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_schar   (a_code);
   /*---(default)------------------------*/
   if (a_terse != NULL)  strlcpy (a_terse, "", LEN_LABEL);
   if (a_name  != NULL)  strlcpy (a_name , "", LEN_LABEL);
   if (a_desc  != NULL)  strlcpy (a_desc , "", LEN_HUND );
   /*---(defense)------------------------*/
   --rce;  if (a_code < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(by-terse)-----------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      if (s_verb_info [i].name  [0] == NULL)    break;
      if (s_verb_info [i].active != 'y')        continue;
      if (a_code  != s_verb_info [i].code)      continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
      if (a_terse != NULL)  strlcpy (a_terse, s_verb_info [i].terse, LEN_LABEL);
      if (a_name  != NULL)  strlcpy (a_name , s_verb_info [i].name , LEN_LABEL);
      if (a_desc  != NULL)  strlcpy (a_desc , s_verb_info [i].desc , LEN_HUND );
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   --rce;
   DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
   return rce;
}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_scrp_verb         (char *a_char)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         i           =    0;
   int         x_len       =    0;
   int         x_index     =   -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_char);
   /*---(default)------------------------*/
   myKINE.s_iverb = -1;
   myKINE.s_style = -1;
   myKINE.s_targ  = -1;
   myKINE.s_from  = -1;
   myKINE.s_servo = -1;
   /*---(defense)------------------------*/
   --rce;  if (a_char == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no value");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_snote   (a_char);
   /*---(by-terse)-----------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      if (s_verb_info [i].name  [0] == NULL)             break;
      if (s_verb_info [i].active != 'y')                 continue;
      if (a_char [0] != s_verb_info [i].terse  [0])      continue;
      if (strcmp (a_char, s_verb_info [i].terse) != 0)   continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
      x_index = i;
      break;
   }
   /*---(by-name)------------------------*/
   if (x_index < 0) {
      for (i = 0; i < MAX_VERBS; ++i) {
         if (s_verb_info [i].name  [0] == NULL)             break;
         if (s_verb_info [i].active != 'y')                 continue;
         if (a_char [0] != s_verb_info [i].name  [0])       continue;
         if (strcmp (a_char, s_verb_info [i].name) != 0)    continue;
         DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
         x_index = i;
         break;
      }
   }
   /*---(error)--------------------------*/
   --rce;  if (x_index <  0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("not found");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return 0;
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_sint    (x_index);
   /*---(save)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("saving globally");
   strncpy   (myKINE.s_verb, s_verb_info [x_index].name, LEN_LABEL);
   myKINE.s_iverb = x_index;
   myKINE.s_style = s_verb_info [myKINE.s_iverb].style;
   myKINE.s_targ  = s_verb_info [myKINE.s_iverb].target;
   myKINE.s_from  = s_verb_info [myKINE.s_iverb].from;
   myKINE.s_servo = s_verb_info [myKINE.s_iverb].servo;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

static char  /*--> reture an argument value --------------[ ------ [ ------ ]-*/
yKINE__scrp_argval (char *a_name)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   for (i = 0; i < MAX_SCRPARG; ++i) {
      if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
      if (a_name[0] != s_scrparg [i].name [0])                   continue;
      if (strcmp (s_scrparg [i].name, a_name) != 0)              continue;
      return s_scrparg [i].flag;
   }
   return -1;
}

static char  /*--> parse an argument list ----------------[ ------ [ ------ ]-*/
yKINE__scrp_args   (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char       *p           = NULL;
   char       *q           = ", ";               /* strtok delimeters         */
   char       *s           = NULL;               /* strtok context variable   */
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(start the parse)-------------*/
   /*> printf ("a_source %s\n", a_source);                                            <*/
   p = strtok_r (a_source, q, &s);
   while (p != NULL) {
      /*> printf ("  search for %s\n", p);                                            <*/
      for (i = 0; i < MAX_SCRPARG; ++i) {
         /*> printf ("    checking %s\n", s_scrparg [i].name);                        <*/
         if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
         if (p[0] != s_scrparg [i].name [0])                   continue;
         if (strcmp (s_scrparg [i].name, p) != 0)              continue;
         s_scrparg [i].flag = 'y';
         /*> printf ("    found    %s\n", s_scrparg [i].name);                        <*/
         break;
      }
      p = strtok_r (NULL    , q, &s);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     specialty popping                        ----===*/
/*====================------------------------------------====================*/
static void      o___HELPERS_________________o (void) {;};

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_scrp_popverb      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   int         i           =    0;
   char        x_verb      [LEN_RECD];
   int         x_len       =    0;
   int         x_index     =   -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(pop)----------------------------*/
   rc = yPARSE_popstr (x_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(lower)--------------------------*/
   x_len = strllen (x_verb, LEN_RECD);
   DEBUG_YKINE_SCRP   yLOG_value   ("x_len"     , x_len);
   for (i = 0; i < x_len; ++i)   x_verb [i] = tolower (x_verb [i]);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_verb"    , x_verb);
   /*---(find)---------------------------*/
   rc = ykine_scrp_verb   (x_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_scrp_popservo     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_servo     [LEN_RECD];
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(pop)----------------------------*/
   rc = yPARSE_popstr (x_servo);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(prepare)------------------------*/
   x_len = strlen (x_servo);
   /*---(check empty)--------------------*/
   --rce;  if (x_len == 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_len <  2) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_len >  2) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(identify servos)----------------*/
   myKINE.s_count = ykine_servos (x_servo);
   DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);
   --rce;  if (myKINE.s_count < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          repeating                           ----===*/
/*====================------------------------------------====================*/
static void      o___REPEATS_________________o (void) {;}

char
ykine_scrp_prev         (tMOVE *a_move, float *d, float *x, float *z, float *y)
{
   if (d != NULL)  *d = 0.0;
   if (x != NULL)  *x = 0.0;
   if (z != NULL)  *z = 0.0;
   if (y != NULL)  *y = 0.0;
   if (a_move == NULL)          return 0;
   if (d != NULL)  *d = a_move->degs;
   if (x != NULL)  *x = a_move->x_pos;
   if (z != NULL)  *z = a_move->z_pos;
   if (y != NULL)  *y = a_move->y_pos;
   return 0;
}

char         /*--> parse a segno marker ------------------[ ------ [ ------ ]-*/
ykine_scrp_segno        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   char        x_list      [LEN_HUND];
   tSERVO     *x_servo     = NULL;
   float       d, x, z, y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   /*---(process)------------------------*/
   for (j = 0; j < g_nservo; ++j) {
      /*---(filter)----------------------*/
      if (x_list [j] == '_')  continue;
      /*---(handle)----------------------*/
      x_servo = &(g_servo_info [j]);
      if (x_servo == NULL)      continue;
      if (x_servo->seg == YKINE_FOCU) {
         DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
         ykine_scrp_prev   (x_servo->tail, &d, &x, &z, &y);
         ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, myKINE.s_cline, "segno", '-', YKINE_NONE, d, 0.0);
         ykine_move_addloc (x_servo, x, z, y);
         x_servo->segni [x_servo->nsegno] = x_servo->tail;
         ++(x_servo->nsegno);
      } else if (x_servo->seg == YKINE_TIBI) {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            ykine_scrp_prev   (x_servo->tail, &d, &x, &z, &y);
            ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, myKINE.s_cline, "segno", '-', YKINE_NONE, d, 0.0);
            ykine_move_addloc (x_servo, x, z, y);
            x_servo->segni [x_servo->nsegno] = x_servo->tail;
            ++(x_servo->nsegno);
            --x_servo;
         }
      } else {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            ykine_scrp_prev   (x_servo->tail, &d, &x, &z, &y);
            ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, myKINE.s_cline, "segno", '-', YKINE_NONE, d, 0.0);
            ykine_move_addloc (x_servo, x, z, y);
            x_servo->segni [x_servo->nsegno] = x_servo->tail;
            ++(x_servo->nsegno);
            ++x_servo;
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> parse a low level repeat --------------[ ------ [ ------ ]-*/
ykine_scrp_repeat       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   int         j           = 0;
   tSERVO     *x_servo     = NULL;
   double      c           = 0;
   char        x_list      [LEN_HUND];
   int         x_line      =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   rc  = ykine_scrp_popservo ();
   DEBUG_YKINE_SCRP   yLOG_value   ("servo"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ykine_servo_list (x_list);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
   x_line = myKINE.s_cline;
   /*---(get count)-------------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &c);
   DEBUG_YKINE_SCRP  yLOG_value   ("c"         , c);
   for (i = 0; i < g_nservo; ++i) {
      if (x_list [i] == '_')  continue;
      /*---(handle)----------------------*/
      x_servo = &(g_servo_info [i]);
      if (x_servo == NULL)      continue;
      if (x_servo->seg == YKINE_FOCU) {
         DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
         ykine_move_repeat (x_servo, c);
      } else if (x_servo->seg == YKINE_TIBI) {
         for (j = 0; j < 3; ++j) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            ykine_move_repeat (x_servo, c);
            --x_servo;
         }
      } else {
         for (j = 0; j < 3; ++j) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            ykine_move_repeat (x_servo, c);
            ++x_servo;
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       synchronizations                       ----===*/
/*====================------------------------------------====================*/
static void      o___SECTION_________________o (void) {;}

char
ykine_hint__purge        (char a_1st)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(hints)--------------------------*/
   for (i = 0; i < MAX_HINTS; ++i) {
      s_hints [i].major = '-';
      s_hints [i].minor = '-';
      s_hints [i].line  = -1;;
      if (a_1st != 'y' && s_hints [i].label != NULL)  free (s_hints [i].label);
      s_hints [i].label = NULL;
      s_hints [i].secs  = -1.0;
   }
   s_nhint = 0;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_hint_init         (void)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(hints)--------------------------*/
   ykine_hint__purge      ('y');
   ykine_hint__new        ("liberare" , 0, "liberare", 0.0);
   ykine_hint__new        ("inizio"   , 0, "inizio", 0.0);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_hint_reset        (void)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(hints)--------------------------*/
   ykine_hint__purge      ('-');
   ykine_hint__new        ("liberare" , 0, "liberare", 0.0);
   ykine_hint__new        ("inizio"   , 0, "inizio", 0.0);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_hint_final  (float a_sec)
{
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(hints)--------------------------*/
   ykine_hint__new        ("finire"   , 0, "finire", a_sec);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_hint_wrap   (void)        {
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(hints)--------------------------*/
   ykine_hint__purge      ('-');
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_hint__new         (char *a_verb, int a_line, char *a_label, float a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   static char x_major     =  'a';
   static char x_minor     =  'a';
   static char x_finish    =  '-';
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(reset)--------------------------*/
   if (strcmp (a_verb, "liberare") == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("resetting");
      x_major  = 'a';
      x_minor  = 'a';
      x_finish = '-';
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   if (a_label != NULL)  strlcpy (x_label, a_label  , LEN_LABEL);
   else                  strlcpy (x_label, "-"      , LEN_LABEL);
   /*---(check hint)---------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   (a_verb);
   if (strcmp (a_verb, "inizio" ) == 0 && (x_major != 'a' || x_minor != 'a')) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("start is automatic, nothing to do");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   if (strcmp (a_verb, "sezione") == 0) {
      ++x_major;
      x_minor  = 'a';
   }
   if (strcmp (a_verb, "finire" ) == 0 && x_finish != 'y') {
      x_major  = 'z';
      x_minor  = 'z';
      x_finish = 'y';
      strlcpy (x_label, "finire", LEN_LABEL);
   }
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_schar   (x_major);
   --rce;  if (x_major > 'z' || x_major < 0) {
      x_major = '¢';
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_schar   (x_minor);
   --rce;  if (x_minor > 'z' || x_minor < 0) {
      x_minor = '¢';
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_major == 'z' && x_minor == 'z' && strcmp (a_verb, "finire") != 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(add section)--------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("add");
   s_hints [s_nhint].major = x_major;
   s_hints [s_nhint].minor = x_minor;
   s_hints [s_nhint].line  = a_line;
   s_hints [s_nhint].label = strdup (x_label);
   s_hints [s_nhint].secs  = a_sec;
   /*---(update minor)-------------------*/
   ++x_minor;
   if (x_minor > 'z' || x_minor < 0)  x_minor = '¢';
   DEBUG_YKINE_SCRP   yLOG_schar   (x_minor);
   /*---(update count)-------------------*/
   ++s_nhint;
   DEBUG_YKINE_SCRP   yLOG_sint    (s_nhint);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yKINE_sect_cursor       (char a_dir, char *a_major, char *a_minor, int *a_line, char *a_label, float *a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   char       rce          =  -10;
   static int x_curr       =    0;
   /*---(defaults)-----------------------*/
   if (a_major != NULL)  *a_major     = '-';
   if (a_minor != NULL)  *a_minor     = '-';
   if (a_line  != NULL)  *a_line      = -1;
   if (a_label != NULL)  a_label [0]  = '\0';
   if (a_sec   != NULL)  *a_sec       = -1.0;
   /*---(set cursor)---------------------*/
   switch (a_dir) {
   case YKINE_HEAD : x_curr = 0;                   break;
   case YKINE_NEXT : ++x_curr;                     break;
   case YKINE_SAME : break;
   case YKINE_PREV : --x_curr;                     break;
   case YKINE_TAIL : x_curr = s_nhint - 1;         break;
   default   : return -1;
   }
   /*---(defense)------------------------*/
   --rce;  if (x_curr <  0      ) {
      x_curr = 0;
      return rce;
   }
   --rce;  if (x_curr >= s_nhint) {
      x_curr = s_nhint - 1;
      return rce;
   }
   /*---(populate)-----------------------*/
   if (a_major != NULL)  *a_major     = s_hints [x_curr].major;
   if (a_minor != NULL)  *a_minor     = s_hints [x_curr].minor;
   if (a_line  != NULL)  *a_line      = s_hints [x_curr].line;
   if (a_label != NULL)  strlcpy (a_label, s_hints [x_curr].label, LEN_LABEL);
   if (a_sec   != NULL)  *a_sec       = s_hints [x_curr].secs;
   /*---(complete)-----------------------*/
   return 0;

}

char
yKINE_sect_rpt          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   printf ("HINT LIST\n");
   printf ("- - -#- ---label------- -secs-\n");
   for (i = 0; i < s_nhint; ++i) {
      printf ("%c %c %3d %-15.15s %6.1f\n",
            s_hints [i].major, s_hints [i].minor,
            s_hints [i].line , s_hints [i].label,
            s_hints [i].secs);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_scrp_section      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   char        x_label     [LEN_LABEL];
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_tail      = NULL;
   float       x_max       =  0.0;
   float       x_dur       =  0.0;
   char        x_leg       =    0;
   char        x_seg       =    0;
   float       d, x, z, y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get label)----------------------*/
   rc = yPARSE_popstr    (x_label);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_label"   , "%3d, %s", rc, x_label);
   if (strcmp (x_label, "-") == 0)   strlcpy (x_label, "(empty)", LEN_LABEL);
   /*---(find max)-----------------------*/
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      /*---(filter)--------------*/
      if (g_servo_info [i].label [0] == 'e')   break;
      x_tail = g_servo_info [i].tail;
      if (x_tail == NULL)         continue;
      if (x_tail->secs <= x_max)  continue;
      /*---(new max)-------------*/
      DEBUG_YKINE_SCRP   yLOG_complex ("found"     , "%-10.10s, %6.1f", g_servo_info [i].label, x_tail->secs);
      x_max = x_tail->secs;
      /*---(done)----------------*/
   }
   /*---(check nothing)------------------*/
   DEBUG_YKINE_SCRP   yLOG_double  ("x_max"     , x_max);
   if (x_max < 0.1) {
      DEBUG_YKINE_SCRP   yLOG_note    ("nothing to do");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(fill-out)-----------------------*/
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      /*---(filter)--------------*/
      if (g_servo_info [i].label [0] == 'e')   break;
      x_servo = &(g_servo_info [i]);
      x_leg   = x_servo->leg;
      x_seg   = x_servo->seg;
      x_tail  = x_servo->tail;
      /*---(check for first)-----*/
      if (x_tail == NULL) {
         x_dur = x_max;
         d = x = z = y = 0.0;
      }
      /*---(handle ongoing)------*/
      else {
         if (x_tail->secs >= x_max)  continue;
         x_dur   = x_max - x_tail->secs;
         d       = x_tail->degs;
         x       = x_tail->x_pos;
         z       = x_tail->z_pos;
         y       = x_tail->y_pos;
      }
      /*---(add filler)----------*/
      DEBUG_YKINE_SCRP   yLOG_complex ("adding"    , "%-10.10s, %6.1fs", x_servo->label, x_dur);
      ykine_move_create (x_servo, YKINE_SERVO, YKINE_NOOP, -1, "", '-', YKINE_NONE, d, x_dur);
      ykine_move_addloc (x_servo, x, z, y);
      /*---(done)----------------*/
   }
   /*---(add section)--------------------*/
   ykine_hint__new        (myKINE.s_verb, myKINE.s_nline, x_label, x_max);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        script driver                         ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVER__________________o (void) {;}

char
ykine_scrp_crap         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   char        x_active    =  '-';
   char        x_servos    [LEN_LABEL];
   char        x_list      [LEN_HUND ];
   int         x_len       =    0;
   char        x_accel     [LEN_LABEL];
   double      b           =  0.0;
   float       s           =  0.0;
   int         x_leg       = YKINE_RR;
   int         i           =    0;
   char        x_verb      = YKINE_LEGE;
   tSERVO     *x_servo     = NULL;
   float       f, p, t, x, z, y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(check for servos)------------*/
   yPARSE_top      (x_servos);
   DEBUG_YKINE_SCRP  yLOG_info    ("x_servos"   , x_servos);
   x_len = strlen (x_servos);
   DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);
   if (x_len != 2)  --rc;
   else {
      if (strchr ("LRAlra<>*" , x_servos [0]) == 0)  --rc;
      if (strchr ("RMFArfav^*", x_servos [1]) == 0)  --rc;
   }
   /*---(set verb)--------------------*/
   if (rc == 0) {
      x_verb = YKINE_LEGE;
      ykine_scrp_verb   ("error_legs");
   } else {
      x_verb = YKINE_BODE;
      ykine_scrp_verb   ("error_body");
      strcpy (x_servos, "zz");
   }
   /*---(pop servos)------------------*/
   if (rc == 0) {
      myKINE.s_count = ykine_servos (x_servos);
      DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);
      if (myKINE.s_count > 0) {
         ykine_servo_list (x_list);
         DEBUG_YKINE_SCRP   yLOG_info    ("x_list"    , x_list);
      }
      yPARSE_popstr (x_servos);
   }
   /*---(get timing)------------------*/
   ykine_accel_timing ();
   if (myKINE.b < 0.0)  {
      DEBUG_YKINE_SCRP  yLOG_note    ("accelerated must convert to normal");
      myKINE.b = 5.0;
      strlcpy (myKINE.accel, "", LEN_LABEL);
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("pace"      , myKINE.s_pace);
   s = myKINE.b * myKINE.s_pace;
   DEBUG_YKINE_SCRP  yLOG_double  ("s"         , s);
   /*---(get servo)-------------------*/
   for (i = 0; i < g_nservo; ++i) {
      if (g_servo_info [i].scrp != 'y')  continue;
      x_servo = &(g_servo_info [i]);
      DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
      if (x_servo->tail != NULL) {
         x = x_servo->tail->x_pos;
         z = x_servo->tail->z_pos;
         y = x_servo->tail->y_pos;
         DEBUG_YKINE_SCRP  yLOG_complex ("endpoint"  , "%6.1fx, %6.1fz, %6.1fy", x, z, y);
      }
      if (x_verb == YKINE_LEGE) {
         x_servo = &(g_servo_info [i-2]);
         if (x_servo->tail != NULL)  f = myKINE.fe = x_servo->tail->degs;
         x_servo = &(g_servo_info [i-1]);
         if (x_servo->tail != NULL)  p = myKINE.pe = x_servo->tail->degs;
         x_servo = &(g_servo_info [i-0]);
         if (x_servo->tail != NULL)  t = myKINE.te = x_servo->tail->degs;
         x_leg = g_servo_info [i].leg;
         ykine_accel_immediate   (x_verb, x_leg, myKINE.b, "error entry");
         ykine_move_addloc (x_servo, x, z, y); /* update */
      } else {
         x_servo = &(g_servo_info [i]);
         ykine_move_create (x_servo, YKINE_SERVO, x_verb, myKINE.s_cline, "error entry", '-', YKINE_NONE, 0.0, s);
         ykine_move_addloc (x_servo, x, z, y);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_scrp_exec         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   char        x_active    =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP  yLOG_value   ("iverb"     , myKINE.s_iverb);
   --rce;  if (myKINE.s_iverb < 0) {
      DEBUG_YKINE_SCRP  yLOG_note    ("verb not found");
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(filter)-------------------------*/
   x_active = s_verb_info [myKINE.s_iverb].active;
   DEBUG_YKINE_SCRP  yLOG_char    ("active"    , x_active);
   --rce;  if (x_active != 'y') {
      DEBUG_YKINE_SCRP  yLOG_note    ("verb not active");
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)----------------------------*/
   DEBUG_YKINE_SCRP  yLOG_point   ("function"  , s_verb_info [myKINE.s_iverb].call);
   if (s_verb_info [myKINE.s_iverb].call == NULL) {
      DEBUG_YKINE_SCRP  yLOG_note    ("verb handler null");
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = s_verb_info [myKINE.s_iverb].call ();
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /* file reading driver ----------------------[--------[--------]-*/
yKINE_script       (float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   float       x_len       =  0.0;
   float       x_sec       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   rc = ykine_scrp_begin ();
   /*---(open stdin)---------------------*/
   rc = yPARSE_stdin     ();
   DEBUG_YKINE_SCRP  yLOG_value   ("open"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(read lines)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("read lines");
   while (1) {
      ykine__scrp_prep  ();
      /*---(parse)-----------------------*/
      rc = yPARSE_read  (&(myKINE.s_nline), &(myKINE.s_cline));
      DEBUG_YKINE_SCRP  yLOG_complex ("read"      , "%3dn, %3dc, %d", myKINE.s_nline, myKINE.s_cline, rc);
      if (feof (stdin))          break;
      if (rc <= 0)               continue;
      /*---(handle verb)-----------------*/
      DEBUG_YKINE_SCRP  yLOG_value   ("i_verb"    , myKINE.s_iverb);
      if (myKINE.s_iverb < 0) {
         rc = ykine_scrp_crap ();
         DEBUG_YKINE_SCRP  yLOG_note    ("returned");
      }
      else                       rc = ykine_scrp_exec ();
      DEBUG_YKINE_SCRP  yLOG_value   ("exec"      , rc);
      /*---(done)------------------------*/
   }
   /*---(close stdin)--------------------*/
   rc = yPARSE_close_in  ();
   DEBUG_YKINE_SCRP  yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(fix length)---------------------*/
   x_len = 0.0;
   for (i = 0; i < g_nservo; ++i) {
      yKINE_move_tail (i, &x_sec, NULL);
      /*> printf ("#%-2d, %-10.10s, %fs\n", i, g_servo_info [i].label, x_sec);        <*/
      if (x_sec > x_len)  x_len = x_sec;
   }
   myKINE.scrp_len = x_len;
   if (a_len != NULL)  *a_len = x_len;
   /*> printf ("so, %fs and %fs\n", x_len, myKINE.scrp_len);                          <*/
   /*> exit (1);                                                                      <*/
   ykine_hint_final (myKINE.scrp_len);
   yKINE_sect_rpt   ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_scrp        (char *a_question, int a_num)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "SCRP unit        : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if (strcmp (a_question, "verb"    ) == 0) {
      if (myKINE.s_iverb < 0) {
         sprintf (ykine__unit_answer, "SCRP verb      : none");
      } else {
         sprintf (ykine__unit_answer, "SCRP verb      : %-10.10s, targ %c, from %c", myKINE.s_verb, myKINE.s_targ, myKINE.s_from);
      }
   }
   else if (strcmp (a_question, "angles"   ) == 0) {
      sprintf (ykine__unit_answer, "SCRP angles    : %8.3fb, %8.2ff, %8.2fp, %8.2ft", myKINE.s_beats, myKINE.s_femu, myKINE.s_pate, myKINE.s_tibi);
   }
   else if (strcmp (a_question, "positions") == 0) {
      sprintf (ykine__unit_answer, "SCRP positions : %8.3fb, %8.2fx, %8.2fz, %8.2fy", myKINE.s_beats, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   }
   else if (strcmp (a_question, "passage"  ) == 0) {
      if (a_num >= s_nhint)  sprintf (ykine__unit_answer, "SCRP passage   : --/-- - - -- --------------- ----.-");
      else                   sprintf (ykine__unit_answer, "SCRP passage   : %2d/%2d %c %c %2d %-15.15s %6.1f", a_num + 1, s_nhint, s_hints [a_num].major, s_hints [a_num].minor, s_hints [a_num].line, s_hints [a_num].label, s_hints [a_num].secs);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
