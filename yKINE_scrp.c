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
   int         (*call)     (int n, char *a_verb);
   char        desc        [LEN_HUND ];     /* english description            */
};
tVERBS   s_verb_info    [MAX_VERBS] = {
   /* ===[[ forward kinematics ]]================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_FK  , "fk"   , "forward"      , 'y' , 'y' , YKINE_FORWARD, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_fk         , "set absolute joint angles on all three joints"     },
   /* ===[[ inverse kinematics ]]================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_IK  , "ik"   , "inverse"      , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_ik         , "set an exact endpoint in 3d space"                 },
   { YKINE_TK  , "tk"   , "tangent"      , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_tk         , "set an exact endpoint tangential to leg"           },
   { YKINE_CK  , "ck"   , "center"       , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_POLAR ,  1, ykine_legs_ck         , "set absolute joint angles on all three joints"     },
   { YKINE_RK  , "rk"   , "radial"       , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_POLAR ,  1, ykine_legs_rk         , "set absolute joint angles on all three joints"     },
   { YKINE_SK  , "sk"   , "step"         , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_POLAR ,  1, ykine_legs_sk         , "set absolute joint angles on all three joints"     },
   { YKINE_NK  , "nk"   , "neighbor"     , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, ykine_legs_nk         , "set exact endpoint relative to military"           },
   /* ===[[ body zero-point ]]===================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_ZE  , "ze"   , "zero"         , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, YKINE_LINEAR, -1, ykine_body_zero       , "set absolute body position in 3d space"            },
   { YKINE_PO  , "po"   , "polar"        , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, YKINE_POLAR , -1, ykine_body_polar      , "set relative body position based on last position" },
   /* ===[[ body orientation ]]==================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_OR  , "or"   , "orient"       , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, YKINE_LINEAR, -1, ykine_body_orient     , "set absolute body orientation angles"              },
   { YKINE_TI  , "ti"   , "tilt"         , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, YKINE_POLAR , -1, ykine_body_tilt       , "set relative body position based on last position" },
   { YKINE_ST  , "st"   , "stance"       , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, YKINE_POLAR , -1, ykine_stance          , "set relative body position based on last position" },
   /* ===[[ synchronization ]]===================================================*/
   { YKINE_SYNC, "sync" , "sync-up"      , 'y' , '-' , YKINE_FORWARD, YKINE_PURE, YKINE_LINEAR, -1, NULL                  , "non-moving periods to sync up stances"             },
   /* ===[[ music notation ]]====================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
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
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { YKINE_TSP , "tsp"  , "speed"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_speed    , "set the speed of the turtle movement"              },
   { YKINE_TWA , "twa"  , "wait"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_wait     , "stop action for a specific period"                 },
   { YKINE_THM , "thm"  , "home"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_home     , "move the turtle to 0x, 0z at the same height"      },
   { YKINE_TMV , "tmv"  , "move"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_move     , "move the turtle on existing direction"             },
   { YKINE_TGO , "tgo"  , "goto"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_goto     , "time signature for rhythm/beat"                    },
   { YKINE_THE , "the"  , "head"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_head     , "change the absolute direction of the turtle"       },
   { YKINE_TTU , "ttu"  , "turn"         , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_turn     , "adjust the direction of the turtle from current"   },
   { YKINE_TLO , "tlo"  , "lower"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_lower    , "time signature for rhythm/beat"                    },
   { YKINE_TRA , "tra"  , "raise"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_raise    , "time signature for rhythm/beat"                    },
   { YKINE_TDE , "tde"  , "depth"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, ykine_turtle_depth    , "time signature for rhythm/beat"                    },
   { YKINE_TPU , "tpu"  , "push"         , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "time signature for rhythm/beat"                    },
   { YKINE_TPO , "tpo"  , "pop"          , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "time signature for rhythm/beat"                    },
   /* ===[[ gait framework ]]====================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { -1        , "incp" , "gait_beg"     , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_beg        , "beginning of a gait description"                   },
   { -1        , "incp" , "incipio"      , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_beg        , "beginning of a gait description"                   },
   { -1        , "comp" , "gait_end"     , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_end        , "end of a gait description"                         },
   { -1        , "comp" , "compleo"      , 'y' , 'y' , YKINE_CONTROL, YKINE_NONE, YKINE_NONE  , -1, ykine_gait_end        , "end of a gait description"                         },
   /* ===[[ complex gaits ]]=====================================================*/
   /* constant , terse--  verb----------- actv- servo  targ---------- rel-------- style------- mask call------------------- description---------------------------------------- */
   { -1        , "walk" , "walk"         , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "repeat a specific number of steps"                 },
   { -1        , "circ" , "circle"       , '-' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, NULL                  , "repeat a specific number of steps"                 },
   /* ===[[ special ]]===========================================================*/
   { YKINE_BODE, "e/bod", "error_body"   , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, YKINE_LINEAR, -1, "-"                   , "verb not understood"                               },
   { YKINE_LEGE, "e/leg", "error_legs"   , 'y' , '-' , YKINE_INVERSE, YKINE_PURE, YKINE_LINEAR,  1, "-"                   , "verb not understood"                               },
   { YKINE_MISS, "miss" , "DEFAULT"      , 'y' , '-' , NULL         , NULL      , NULL        , -1, ykine_scrp_miss       , "default verb handler, verb not found"              },
   { YKINE_NOOP, "noop" , "empty"        , 'y' , '-' , YKINE_NONE   , YKINE_NONE, YKINE_NONE  , -1, "-"                   , "nothing to do"                                     },
   /* ===[[ done ]]==============================================================*/
   { -1        , NULL   , NULL           ,  0  ,  0  , -1           , -1        , -1          , -1, NULL                  , NULL                                                },
};
/*
 *
 *   angle       absolute direction
 *   turn        relative direction
 *
 *   move        relative movement
 *   goto        absolute movement
 *   home        return to zero point
 *
 *   depth       set surface height
 *   raise       normal raising of turtle
 *   lower       put turtle on surface
 *
 *
 *   tu_wait     tu wait       twait      wait    rest
 *   tu_turn     tu turn       tturn      turn    pivot
 *   tu_move     tu move       tmove      move
 *
 *
 */

#define    MAX_SCRPARG      100
typedef    struct  cSCRPARG   tSCRPARG;
struct cSCRPARG {
   char        name        [LEN_LABEL];
   char        flag;
   char        desc        [LEN_DESC ];
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
   return yKINE_handlers ();
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
 *>       /+> printf ("checking %s�", g_servo_info[i].label);                                <+/   <* 
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
ykine_scrp_by_terse     (char *a_terse, char *a_code, char *a_name, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_snote   (a_terse);
   /*---(default)------------------------*/
   if (a_code  != NULL)  *a_code = -1;
   if (a_name  != NULL)  strlcpy (a_name , "", LEN_LABEL);
   if (a_desc  != NULL)  strlcpy (a_desc , "", LEN_HUND );
   /*---(defense)------------------------*/
   --rce;  if (a_terse == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(by-terse)-----------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      if (s_verb_info [i].name  [0] == NULL)    break;
      if (s_verb_info [i].active != 'y')        continue;
      if (strcmp (a_terse, s_verb_info [i].terse) != 0) continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
      if (a_code  != NULL)  *a_code = i;
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
ykine_scrp_verb         (char *a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         i           =    0;
   int         x_len       =    0;
   int         x_index     =   -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_verb);
   /*---(default)------------------------*/
   myKINE.s_iverb = -1;
   myKINE.s_style = -1;
   myKINE.s_targ  = -1;
   myKINE.s_from  = -1;
   myKINE.s_servo = -1;
   /*---(defense)------------------------*/
   --rce;  if (a_verb == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no value");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_snote   (a_verb);
   /*---(by-terse)-----------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      if (s_verb_info [i].name  [0] == NULL)             break;
      if (s_verb_info [i].active != 'y')                 continue;
      if (a_verb [0] != s_verb_info [i].terse  [0])      continue;
      if (strcmp (a_verb, s_verb_info [i].terse) != 0)   continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
      x_index = i;
      break;
   }
   /*---(by-name)------------------------*/
   if (x_index < 0) {
      for (i = 0; i < MAX_VERBS; ++i) {
         if (s_verb_info [i].name  [0] == NULL)             break;
         if (s_verb_info [i].active != 'y')                 continue;
         if (a_verb [0] != s_verb_info [i].name  [0])       continue;
         if (strcmp (a_verb, s_verb_info [i].name) != 0)    continue;
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
ykine_scrp_popverb      (int n, uchar *a_verb, char a_exist, void *a_handler)
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
   DEBUG_YKINE_SCRP  yLOG_complex ("args"      , "%2d, %s, %c, %p", n, a_verb, a_exist, a_handler);
   /*---(pop)----------------------------*/
   strlcpy (x_verb, a_verb, LEN_LABEL);
   --rce;  if (n < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> rc = yPARSE_popstr (x_verb);                                                   <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);                         <* 
    *>    return rc;                                                                  <* 
    *> }                                                                              <*/
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
   if (d != NULL)  *d = a_move->pure_d;
   if (x != NULL)  *x = a_move->pure_x;
   if (z != NULL)  *z = a_move->pure_z;
   if (y != NULL)  *y = a_move->pure_y;
   return 0;
}

char         /*--> parse a segno marker ------------------[ ------ [ ------ ]-*/
ykine_scrp_segno_OLD    (int n, uchar *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   char        x_list      [LEN_HUND];
   tSERVO     *x_servo     = NULL;
   float       d, x, z, y;
   char        PURE_RC     =    0;
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
         ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, myKINE.s_tline, "segno", '-', YKINE_NONE, 0.0);
         ykine_move_add_pure (x_servo, PURE_RC, d, x, z, y);
         x_servo->segni [x_servo->nsegno] = x_servo->tail;
         ++(x_servo->nsegno);
      } else if (x_servo->seg == YKINE_TIBI) {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            ykine_scrp_prev   (x_servo->tail, &d, &x, &z, &y);
            ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, myKINE.s_tline, "segno", '-', YKINE_NONE, 0.0);
            ykine_move_add_pure (x_servo, PURE_RC, d, x, z, y);
            x_servo->segni [x_servo->nsegno] = x_servo->tail;
            ++(x_servo->nsegno);
            --x_servo;
         }
      } else {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            ykine_scrp_prev   (x_servo->tail, &d, &x, &z, &y);
            ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, myKINE.s_tline, "segno", '-', YKINE_NONE, 0.0);
            ykine_move_add_pure (x_servo, PURE_RC, d, x, z, y);
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

char         /*--> parse a segno marker ------------------[ ------ [ ------ ]-*/
ykine_scrp_segno        (int n, uchar *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   char        x_list      [LEN_HUND];
   tSERVO     *x_servo     = NULL;
   float       d, x, z, y;
   char        PURE_RC     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (j = 0; j < g_nservo; ++j) {
      /*---(handle)----------------------*/
      x_servo = &(g_servo_info [j]);
      if (x_servo == NULL)      continue;
      ykine_scrp_prev   (x_servo->tail, &d, &x, &z, &y);
      ykine_move_create (x_servo, YKINE_NOTE, YKINE_SEGN, n, "segno", '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (x_servo, PURE_RC, d, x, z, y);
      x_servo->segni [x_servo->nsegno] = x_servo->tail;
      ++(x_servo->nsegno);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> parse a low level repeat --------------[ ------ [ ------ ]-*/
ykine_scrp_repeat_OLD   (int n, uchar *a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   tSERVO     *x_servo     = NULL;
   double      c           =    0;
   char        x_list      [LEN_HUND];
   int         x_line      =    0;
   char        x_leg       =    0;
   char        x_seg       =    0;
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
      /*---(filter)----------------------*/
      if (x_list [i] == '_')  continue;
      x_servo = &(g_servo_info [i]);
      if (x_servo == NULL)      continue;
      x_leg   = x_servo->leg;
      x_seg   = x_servo->seg;
      /*---(body)------------------------*/
      if (x_leg == YKINE_BODY) {
         DEBUG_YKINE_SCRP   yLOG_info    ("body label", x_servo->label);
         switch (x_seg) {
         case YKINE_FOCU : /* zero-point */
            DEBUG_YKINE_SCRP   yLOG_note    ("zero-point");
            ykine_move_repeat (x_servo, c);
            continue;
            break;
         case YKINE_YAW  : /* orientation */
            DEBUG_YKINE_SCRP   yLOG_note    ("orientation");
            for (j = 0; j < 3; ++j) {
               ykine_move_repeat (x_servo, c);
               ++x_servo;
            }
            continue;
            break;
         default         :
            DEBUG_YKINE_SCRP   yLOG_note    ("can not be processed");
            continue;
            break;
         }
      }
      /*---(legs)------------------------*/
      else {
         DEBUG_YKINE_SCRP   yLOG_info    ("leg label" , x_servo->label);
         switch (x_seg) {
         case YKINE_FEMU : /* forward */
            for (j = 0; j < 4; ++j) {
               DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
               ykine_move_repeat (x_servo, c);
               ++x_servo;
            }
            continue;
            break;
         case YKINE_TIBI : /* inverse */
            ++x_servo;
            for (j = 0; j < 4; ++j) {
               DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
               ykine_move_repeat (x_servo, c);
               --x_servo;
            }
            continue;
            break;
         default         :
            DEBUG_YKINE_SCRP   yLOG_note    ("can not be processed");
            continue;
            break;
         }
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> parse a low level repeat --------------[ ------ [ ------ ]-*/
ykine_scrp_repeat       (int n, uchar *a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   tSERVO     *x_servo     = NULL;
   double      c           =    0;
   char        x_list      [LEN_HUND];
   int         x_line      =    0;
   char        x_leg       =    0;
   char        x_seg       =    0;
   tMOVE      *x_move      = NULL;
   int         x_beg       =    0;
   int         x_end       =    0;
   char        x_verb      [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get count)-------------------*/
   if (rc >= 0)  rc  = yPARSE_popval   (0.0, &c);
   --c;
   if (c  <  0)  c = 0;
   DEBUG_YKINE_SCRP  yLOG_value   ("c"         , c);
   /*---(get seg line number)---------*/
   x_servo = &(g_servo_info [0]);
   DEBUG_YKINE_MOVE   yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo <= 0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("servo not found");
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("nsegno"    , x_servo->nsegno);
   --rce;  if (x_servo->nsegno <= 0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("no segni in servo stack");
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --(x_servo->nsegno);
   x_move  = x_servo->segni [x_servo->nsegno];
   --rce;  if (x_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_note    ("null segni at current position in stack");
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_complex ("segno"     , "%3d, %3d, %s", x_move->seq, x_move->line, x_move->label);
   x_beg   = x_move->line;
   x_end   = n;
   DEBUG_YKINE_MOVE   yLOG_complex ("bounds"    , "%3d, %3d", x_beg, x_end);
   /*---(add)----------------------------*/
   for (i = 0; i < c; ++i) {
      DEBUG_YKINE_MOVE   yLOG_value   ("lead note" , i);
      /*---(repeat title)----------------*/
      for (j = 0; j < g_nservo; ++j) {
         x_servo = &(g_servo_info [j]);
         if (x_servo == NULL)      continue;
         x_leg   = x_servo->leg;
         x_seg   = x_servo->seg;
         DEBUG_YKINE_MOVE   yLOG_complex ("add note"  , "%dj, %p, %dl, %ds", j, x_servo, x_leg, x_seg);
         ykine_move__repeatnote (x_servo, n, i + 1, x_beg);
      }
      /*---(repeat moves)----------------*/
      DEBUG_YKINE_MOVE   yLOG_value   ("MOVES"     , i);
      for (j = x_beg + 1; j < x_end; ++j) {
         rc = yPARSE_reload (&(myKINE.s_nline), &(myKINE.s_cline), j, "");
         DEBUG_YKINE_MOVE   yLOG_complex ("reload"    , "%d, %d", j, rc);
         myKINE.s_tline = j;
         /*---(prepare verb)----------------*/
         rc = yparse_peek_verb (NULL, x_verb);
         DEBUG_YKINE_MOVE   yLOG_complex ("verb"      , "%4d, %s", rc, x_verb);
         rc = ykine_scrp_verb  (x_verb);
         DEBUG_YKINE_SCRP   yLOG_value   ("verb"      , rc);
         --rce;  if (rc < 0) {
            DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         /*---(run)-------------------------*/
         rc = ykine_scrp_exec    ();
         DEBUG_YKINE_MOVE   yLOG_value   ("exec"      , rc);
         /*---(done)------------------------*/
      }
      /*---(check for empties)-----------*/
      for (j = 0; j < g_nservo; ++j) {
         x_servo = &(g_servo_info [j]);
         if (x_servo == NULL)      continue;
         x_move  = x_servo->tail;
         if (x_move->verb == YKINE_SEGN || x_move->verb == YKINE_REPT) {
            rc = ykine_move_delete (&x_move);
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_note    ("add repeat end markers (eretepir)");
   for (j = 0; j < g_nservo; ++j) {
      x_servo = &(g_servo_info [j]);
      if (x_servo == NULL)      continue;
      x_move  = x_servo->tail;
      if (x_move->verb == YKINE_SEGN) {
         rc = ykine_move_delete (&x_move);
      } else {
         x_leg   = x_servo->leg;
         x_seg   = x_servo->seg;
         DEBUG_YKINE_MOVE   yLOG_complex ("add note"  , "%dj, %p, %dl, %ds", j, x_servo, x_leg, x_seg);
         ykine_move__repeatnote (x_servo, n, -1, x_beg);
      }
      x_servo->segni [x_servo->nsegno] = NULL;
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
      x_major = '�';
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_schar   (x_minor);
   --rce;  if (x_minor > 'z' || x_minor < 0) {
      x_minor = '�';
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
   if (x_minor > 'z' || x_minor < 0)  x_minor = '�';
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
ykine_scrp_section      (int n, uchar *a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_tail      = NULL;
   float       x_max       =  0.0;
   float       x_dur       =  0.0;
   char        x_leg       =    0;
   char        x_seg       =    0;
   float       d, x, z, y;
   char        PURE_RC     =    0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get label)----------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("args"      , "%3d, %s", n, a_verb);
   if (strcmp (a_verb , "-") == 0)   strlcpy (a_verb , "(empty)", LEN_LABEL);
   yPARSE_popstr (x_label);
   if (strcmp (x_label, "")  == 0)   strlcpy (x_label, "(empty)", LEN_LABEL);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_label"   , x_label);
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
         d       = x_tail->pure_d;
         x       = x_tail->pure_x;
         z       = x_tail->pure_z;
         y       = x_tail->pure_y;
      }
      /*---(add filler)----------*/
      DEBUG_YKINE_SCRP   yLOG_complex ("adding"    , "%-10.10s, %6.1fs", x_servo->label, x_dur);
      ykine_move_create (x_servo, YKINE_SERVO, YKINE_NOOP, myKINE.s_tline, "", '-', YKINE_NONE, x_dur);
      ykine_move_add_pure  (x_servo, 0, d, x, z, y);
      ykine_move_add_adapt (x_servo, 0, d, x, z, y);
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
ykine_scrp_miss         (int n, uchar *a_verb)
{
   return 0;
}

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
   char        PURE_RC     =    0;
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
   if (g_timing.beats < 0.0)  {
      DEBUG_YKINE_SCRP  yLOG_note    ("accelerated must convert to normal");
      g_timing.beats = 5.0;
      strlcpy (g_timing.request, "", LEN_LABEL);
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("pace"      , myKINE.s_pace);
   s = g_timing.beats * myKINE.s_pace;
   DEBUG_YKINE_SCRP  yLOG_double  ("s"         , s);
   /*---(get servo)-------------------*/
   for (i = 0; i < g_nservo; ++i) {
      if (g_servo_info [i].scrp != 'y')  continue;
      x_servo = &(g_servo_info [i]);
      DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
      if (x_servo->tail != NULL) {
         x = x_servo->tail->pure_x;
         z = x_servo->tail->pure_z;
         y = x_servo->tail->pure_y;
         DEBUG_YKINE_SCRP  yLOG_complex ("endpoint"  , "%6.1fx, %6.1fz, %6.1fy", x, z, y);
      }
      if (x_verb == YKINE_LEGE) {
         x_servo = &(g_servo_info [i-2]);
         if (x_servo->tail != NULL)  f = g_end.fd  = x_servo->tail->pure_d;
         x_servo = &(g_servo_info [i-1]);
         if (x_servo->tail != NULL)  p = g_end.pd  = x_servo->tail->pure_d;
         x_servo = &(g_servo_info [i-0]);
         if (x_servo->tail != NULL)  t = g_end.td  = x_servo->tail->pure_d;
         x_leg = g_servo_info [i].leg;
         ykine_accel_immediate   (x_verb, -10, x_leg, g_timing.beats, "error entry");
         ykine_move_add_pure (x_servo, PURE_RC, 0.0, x, z, y); /* update */
      } else {
         x_servo = &(g_servo_info [i]);
         ykine_move_create (x_servo, YKINE_SERVO, x_verb, myKINE.s_tline, "error entry", '-', YKINE_NONE, s);
         ykine_move_add_pure (x_servo, PURE_RC, 0.0, x, z, y);
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
   rc = s_verb_info [myKINE.s_iverb].call (myKINE.s_tline, s_verb_info [myKINE.s_iverb].terse);
   DEBUG_YKINE_SCRP  yLOG_value   ("call"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> setup yPARSE handlers ------------------[--------[--------]-*/
yKINE_handlers          (void)
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
      yPARSE_handler_max ('�', s_verb_info [i].name, 0.0, "", s_verb_info [i].mask, s_verb_info [i].call, NULL, "", "", s_verb_info [i].desc);
   }
   yPARSE_delimiters  (YPARSE_FUNCTION);
   yPARSE_set_reuse ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yKINE_scrp_prepper      (char a_pass)
{
   DEBUG_YKINE_SCRP yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP yLOG_schar   (a_pass);
   myKINE.s_pass = a_pass;
   yPARSE_set_reuse ();
   DEBUG_YKINE_SCRP yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yKINE_scrp_finisher     (char a_pass)
{
   float       x_len     =  0.0;
   int         i         =    0;
   float       x_sec       =  0.0;
   DEBUG_YKINE_SCRP yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP yLOG_value   ("a_pass"    , a_pass);
   /*> DEBUG_YKINE_SCRP yLOG_senter  (__FUNCTION__);                                  <*/
   /*> DEBUG_YKINE_SCRP yLOG_schar   (a_pass);                                        <*/
   x_len = 0.0;
   for (i = 0; i < g_nservo; ++i) {
      DEBUG_YKINE_SCRP yLOG_value   ("servo"     , i);
      yKINE_move_tail (i, &x_sec, NULL);
      DEBUG_YKINE_SCRP yLOG_double  ("x_sec"     , x_sec);
      /*> printf ("#%-2d, %-10.10s, %fs\n", i, g_servo_info [i].label, x_sec);        <*/
      if (x_sec > x_len)  x_len = x_sec;
      DEBUG_YKINE_SCRP yLOG_double  ("x_len"     , x_len);
   }
   DEBUG_YKINE_SCRP yLOG_note    ("all servos done");
   /*> if (a_len != NULL)  *a_len = x_len;                                            <*/
   myKINE.scrp_len = x_len;
   /*> if (a_len != NULL)  *a_len = x_len;                                            <*/
   /*> printf ("so, %fs and %fs\n", x_len, myKINE.scrp_len);                          <*/
   /*> exit (1);                                                                      <*/
   ykine_hint_final (myKINE.scrp_len);
   myKINE.s_pass = 0;
   DEBUG_YKINE_SCRP yLOG_double  ("x_len"     , x_len);
   yKEYS_progress_length (0.0, x_len);
   /*> yKINE_sect_rpt   ();                                                           <*/
   /*> DEBUG_YKINE_SCRP yLOG_sexit   (__FUNCTION__);                                  <*/
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
   char        x_msg       [LEN_RECD];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "SCRP unit        : question not understood", LEN_RECD);
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
