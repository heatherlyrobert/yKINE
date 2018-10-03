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
   char        name        [LEN_LABEL];     /* full verb name                 */
   char        active;                      /* allowed for use                */
   char        servo;                       /* has a servo specifier field    */
   char        target;                      /* leg/seg targeting override     */
   char        from;                        /* pure vs from coding            */
   char        (*call)     (void);          /* function pointer               */
   char        desc        [LEN_STR  ];     /* english description            */
};
tVERBS   s_verb_info    [MAX_VERBS] = {
   /* ===[[ inverse kinematics ]]================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "ik_pure"      , 'y' , 'y' , YKINE_INVERSE, YKINE_PURE, ykine_scrp_ik         , "set an exact endpoint in 3d space"                 },
   { "ik_down"      , '-' , 'y' , YKINE_INVERSE, YKINE_DOWN, ykine_scrp_ik         , "set an exact endpoint, except rel to ground (y)"   },
   { "ik_from"      , 'y' , 'y' , YKINE_INVERSE, YKINE_FROM, ykine_scrp_ik         , "set a relative endpoint based on last position"    },
   /* ===[[ forward kinematics ]]================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "fk_pure"      , 'y' , 'y' , YKINE_FORWARD, YKINE_PURE, ykine_scrp_fk         , "set absolute joint angles on all three joints"     },
   { "fk_from"      , 'y' , 'y' , YKINE_FORWARD, YKINE_FROM, ykine_scrp_fk         , "set relative joint angles based on last angle"     },
   /* ===[[ body zero-point ]]===================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "ze_pure"      , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, ykine_scrp_zero       , "set absolute body position in 3d space"            },
   { "ze_from"      , 'y' , '-' , YKINE_ZERO   , YKINE_FROM, ykine_scrp_zero       , "set relative body position based on last position" },
   { "zp_pure"      , 'y' , '-' , YKINE_ZERO   , YKINE_PURE, ykine_scrp_zpolar     , "set relative body position based on last position" },
   { "zp_from"      , 'y' , '-' , YKINE_ZERO   , YKINE_FROM, ykine_scrp_zpolar     , "set relative body position based on last position" },
   /* ===[[ body orientation ]]==================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "or_pure"      , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, ykine_scrp_orient     , "set absolute body orientation angles"              },
   { "or_from"      , 'y' , '-' , YKINE_ORIENT , YKINE_FROM, ykine_scrp_orient     , "set relative body orientation from last position"  },
   { "op_pure"      , 'y' , '-' , YKINE_ORIENT , YKINE_PURE, ykine_scrp_opolar     , "set relative body position based on last position" },
   { "op_from"      , 'y' , '-' , YKINE_ORIENT , YKINE_FROM, ykine_scrp_opolar     , "set relative body position based on last position" },
   /* ===[[ music notation ]]====================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "meter"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, ykine_scrp_repeat     , "time signature for rhythm/beat"                    },
   { "tempo"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, ykine_scrp_repeat     , "pace of music in beats-per-minute"                 },
   { "segno"        , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, ykine_scrp_segno      , "mark a place in the music for later repeats"       },
   { "repeat"       , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, ykine_scrp_repeat     , "repeat a particular section of movements"          },
   { "ripetere"     , 'y' , 'y' , YKINE_NONE   , YKINE_NONE, ykine_scrp_repeat     , "repeat a particular section of movements"          },
   { "stanza"       , '-' , 'y' , YKINE_NONE   , YKINE_NONE, NULL                  , "mark the beginning of a particular stanza"         },
   { "passaggio"    , '-' , 'y' , YKINE_NONE   , YKINE_NONE, NULL                  , "mark the beginning of a particular passage"        },
   { "sezione"      , '-' , 'y' , YKINE_NONE   , YKINE_NONE, NULL                  , "mark the beginning of a particular section"        },
   /* ===[[ turtle graphics ]]===================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "tu_speed"     , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_turtle_speed    , "set the speed of the turtle movement"              },
   { "tu_home"      , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_turtle_home     , "move the turtle to 0x, 0z at the same height"      },
   { "tu_move"      , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_turtle_move     , "move the turtle on existing direction"             },
   { "tu_goto"      , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "time signature for rhythm/beat"                    },
   { "tu_head"      , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_turtle_head     , "change the absolute direction of the turtle"       },
   { "tu_turn"      , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_turtle_turn     , "adjust the direction of the turtle from current"   },
   { "tu_lower"     , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "time signature for rhythm/beat"                    },
   { "tu_raise"     , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "time signature for rhythm/beat"                    },
   { "tu_push"      , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "time signature for rhythm/beat"                    },
   { "tu_pop"       , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "time signature for rhythm/beat"                    },
   /* ===[[ gait framework ]]====================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "06_GAIT_BEG"  , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_gait_06_beg     , "begin a 6-step gait description"                   },
   { "06_GAIT_END"  , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_gait_06_end     , "end a 6-step gait description"                     },
   { "12_GAIT_BEG"  , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_gait_12_beg     , "begin a 12-step gait description"                  },
   { "12_GAIT_END"  , 'y' , '-' , YKINE_NONE   , YKINE_NONE, ykine_gait_12_end     , "end a 12-step gait description"                    },
   /* ===[[ complex gaits ]]=====================================================*/
   /* verb----------- actv- servo targ---------- rel-------- call------------------- description---------------------------------------- */
   { "walk"         , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "repeat a specific number of steps"                 },
   { "circle"       , '-' , '-' , YKINE_NONE   , YKINE_NONE, NULL                  , "repeat a specific number of steps"                 },
   /* done-------------------*/
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



/*====================------------------------------------====================*/
/*===----                         file access                          ----===*/
/*====================------------------------------------====================*/
static void      o___ACCESS__________________o (void) {;}


char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
yKINE_servo        (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(cycle)--------------------------*/
   /*> printf ("start check\n");                                                      <*/
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (g_servo_info [i].label [0] == 'e')   break;
      /*> printf ("checking %s¦", g_servo_info[i].label);                                <*/
      if (a_source [0] != g_servo_info [i].label [0])       continue;
      if (strcmp (a_source, g_servo_info [i].label) != 0)   continue;
      return i;
   }
   return -1;
}

char  /*--> prepare for use ---------s-------------[ leaf   [ ------ ]-*/
ykine_scrp_init    (void)
{
   myKINE.scrp_len    = 0.0;
   myKINE.s_lines     =   0;
   strlcpy (myKINE.s_q, "", LEN_LABEL);
   myKINE.s_context   = NULL;
   return 0;
}

char  /* file reading driver ----------------------[--------[--------]-*/
ykine__scrp_prep   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(call clears)--------------------*/
   ykine_servo_prep  ();
   ykine_queue_purge ();
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
   /*---(defense)------------------------*/
   --rce;  if (a_char == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no value");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_snote   (a_char);
   /*---(cycle)--------------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      if (s_verb_info [i].active != 'y')                 continue;
      if (a_char [0] != s_verb_info [i].name  [0])       continue;
      if (strcmp (a_char, s_verb_info [i].name) != 0)    continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
      x_index = i;
      break;
   }
   --rce;  if (x_index <  0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("not found");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_sint    (x_index);
   /*---(save)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("saving globally");
   strncpy   (myKINE.s_verb, s_verb_info [x_index].name, LEN_LABEL);
   myKINE.s_iverb = x_index;
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
/*===----                        leg movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___MOVES___________________o (void) {;}


/*---(prefix)-------------------------*/
#define     FIELD_SVO      1
#define     FIELD_SEC      2
/*---(servo related)------------------*/
#define     FIELD_DEG      3
/*---(leg related)--------------------*/
#define     FIELD_FEMU     3
#define     FIELD_PATE     4
#define     FIELD_TIBI     5
/*---(position related)---------------*/
#define     FIELD_XPOS     3
#define     FIELD_ZPOS     4
#define     FIELD_YPOS     5
/*---(repeat related)-----------------*/
#define     FIELD_COUNT    2
#define     FIELD_TIMES    3
/*---(suffix)-------------------------*/
#define     FIELD_ARGS     6

static char  /*--> parse a move entry --------------------[ ------ [ ------ ]-*/
yKINE__scrp_move   (char *a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_count     = -1;
   double      x_degs      = -1;
   double      x_secs      = -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo)----*/
         x_count = ykine_servos (p);
         DEBUG_YKINE_SCRP  yLOG_value   ("count"     , x_count);
         --rce;  if (x_count < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_SEC   :  /*---(seconds)--*/
         x_secs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("seconds"   , x_secs);
         break;
      case  FIELD_DEG   :  /*---(degrees)--*/
         x_degs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("degrees"   , x_degs);
         for (j = 0; j < g_nservo; ++j) {
            if (g_servo_info [j].scrp != 'y') continue;
            ykine_move_create (YKINE_MOVE_SERVO, g_servo_info + j, a_type, myKINE.s_lines, x_degs, x_secs);
         }
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         yKINE__scrp_args(p);
         break;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       parsing functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___PARSING_________________o (void) {;}

char         /*--> pull script record from stdin ---------[ ------ [ ------ ]-*/
ykine__parse_stdin      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(get record)---------------------*/
   fgets (myKINE.s_recd, LEN_RECD, stdin);
   --rce;  if (feof (stdin))  {
      DEBUG_YKINE_SCRP   yLOG_snote   ("end of file reached");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(clean)--------------------------*/
   myKINE.s_len = strlen (myKINE.s_recd);
   if (myKINE.s_len > 0)  myKINE.s_recd [--myKINE.s_len] = 0;
   DEBUG_YKINE_SCRP   yLOG_sint    (myKINE.s_len);
   DEBUG_YKINE_SCRP   yLOG_snote   (myKINE.s_recd);
   /*---(complete)--------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> pull script record from history -------[ ------ [ ------ ]-*/
ykine__parse_existing   (int a_line, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(retrieve)-----------------------*/
   rc = ykine_queue_getline (a_line);
   DEBUG_YKINE_SCRP   yLOG_value   ("getline"   , rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(update)-------------------------*/
   --rce;  if (strncmp (myKINE.s_recd + 12, "XX", 2) == 0) {
      DEBUG_YKINE_SCRP   yLOG_point   ("a_label"   , a_label);
      if (a_label == NULL) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YKINE_SCRP   yLOG_info    ("a_label"   , a_label);
      myKINE.s_recd [12] = a_label [0];
      myKINE.s_recd [13] = a_label [1];
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> filter out empties and comments -------[ ------ [ ------ ]-*/
ykine__parse_check      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_sint    (myKINE.s_len);
   --rce;  if (myKINE.s_len <= 0)  {
      DEBUG_YKINE_SCRP   yLOG_snote   ("record empty/blank");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_schar   (myKINE.s_recd [0]);
   --rce;  if (myKINE.s_recd [0] == '#') {
      DEBUG_YKINE_SCRP   yLOG_snote   ("comment line, skipping");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, 0);
      return rce;
   }
   --rce;  if (myKINE.s_len <= 5)  {
      DEBUG_YKINE_SCRP   yLOG_snote   ("record too short");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_snote   ("good");
   /*---(complete)--------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char  /*--> prepare a IK/FK move parse ------------[ ------ [ ------ ]-*/
ykine_parse_prep       (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = ykine_scrp_verb (a_verb);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not identify verb");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(initialize)---------------------*/
   myKINE.s_count     = -1;
   myKINE.s_secs      = FAILED;
   myKINE.s_femu      = FAILED;
   myKINE.s_pate      = FAILED;
   myKINE.s_tibi      = FAILED;
   myKINE.s_xpos      = FAILED;
   myKINE.s_zpos      = FAILED;
   myKINE.s_ypos      = FAILED;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> parse a full record -------------------[ ------ [ ------ ]-*/
ykine__parse            (int a_line, char *a_recd, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   rc = ykine__scrp_prep     ();
   /*---(line number)--------------------*/
   if (a_line < 0) {
      ++myKINE.s_lines;
      DEBUG_YKINE_SCRP   yLOG_value   ("new line"  , myKINE.s_lines);
   } else if (a_line > 0) {
      DEBUG_YKINE_SCRP   yLOG_value   ("old line"  , a_line);
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("hidden execution");
      myKINE.s_hidden = 'y';
   }
   /*---(read)---------------------------*/
   if (a_recd == NULL) {
      if (a_line < 0) {
         rc = ykine__parse_stdin    ();
         DEBUG_YKINE_SCRP  yLOG_value   ("read"      , rc);
      } else {
         rc = ykine__parse_existing (a_line, a_label);
         DEBUG_YKINE_SCRP  yLOG_value   ("existing"  , rc);
      }
      if (rc < 0) {
         DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
   } else {
      strlcpy (myKINE.s_recd, a_recd, LEN_RECD);
   }
   /*---(check)--------------------------*/
   rc = ykine__parse_check ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(load the queue)-----------------*/
   strlcpy (myKINE.s_orig, myKINE.s_recd, LEN_RECD);
   rc = ykine_queue_recd  (myKINE.s_recd);
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(handle the verb)----------------*/
   rc = ykine_queue_popverb ();
   DEBUG_YKINE_SCRP  yLOG_value   ("verb"      , rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(save)---------------------------*/
   if (a_line < 0) {
      rc = ykine_queue_reusable ();
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 1;
}

char ykine_parse_file   (void)                      { return ykine__parse (-1, NULL, NULL); }
char ykine_parse_load   (char *a_recd)              { return ykine__parse (-1, a_recd, NULL); }
char ykine_parse_reload (int a_line, char *a_label) { return ykine__parse (a_line, NULL, a_label); }
char ykine_parse_hidden (char *a_recd)              { return ykine__parse ( 0, a_recd, NULL); }



/*====================------------------------------------====================*/
/*===----                          repeating                           ----===*/
/*====================------------------------------------====================*/
static void      o___REPEATS_________________o (void) {;}

char         /*--> parse a segno marker ------------------[ ------ [ ------ ]-*/
ykine_scrp_segno        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   tSERVO     *x_servo     = NULL;
   float       xp, zp, yp;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_queue_popservo ();
   DEBUG_YKINE_SCRP  yLOG_value   ("servo"     , rc);
   /*---(process)------------------------*/
   for (j = 0; j < g_nservo; ++j) {
      x_servo = &(g_servo_info [j]);
      if (x_servo == NULL)      continue;
      if (x_servo->scrp != 'y') continue;
      if (x_servo->seg == YKINE_FOCU) {
         DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
         rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
         rc = ykine_move_create (YKINE_MOVE_NOTE, x_servo, "segno", myKINE.s_lines, 0.0, 0.0);
         if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
         x_servo->segni [x_servo->nsegno] = x_servo->tail;
         ++(x_servo->nsegno);
      } else if (x_servo->seg == YKINE_TIBI) {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
            rc = ykine_move_create (YKINE_MOVE_NOTE, x_servo, "segno", myKINE.s_lines, 0.0, 0.0);
            if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
            x_servo->segni [x_servo->nsegno] = x_servo->tail;
            ++(x_servo->nsegno);
            --x_servo;
         }
      } else {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
            ykine_move_create (YKINE_MOVE_NOTE, x_servo, "segno", myKINE.s_lines, 0.0, 0.0);
            if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
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
   float       c           = 0;
   float       xp, zp, yp;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_queue_popservo ();
   DEBUG_YKINE_SCRP  yLOG_value   ("servo"     , rc);
   /*---(get count)-------------------*/
   if (rc >= 0)  rc  = ykine_queue_popval   (0.0, &c);
   DEBUG_YKINE_SCRP  yLOG_value   ("c"         , c);
   for (j = 0; j < g_nservo; ++j) {
      x_servo = &(g_servo_info [j]);
      if (x_servo->scrp != 'y') continue;
      if (x_servo->seg == YKINE_FOCU) {
         DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
         rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
         rc = ykine_move_create (YKINE_MOVE_NOTE, x_servo, "ripetere", myKINE.s_lines, 0.0, 0.0);
         if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
         if (rc >= 0)  ykine_move_repeat     (x_servo    , (int) c);
         rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
         ykine_move_create (YKINE_MOVE_NOTE, x_servo, "eretepir", myKINE.s_lines, 0.0, 0.0);
         if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
      } else if (x_servo->seg == YKINE_TIBI) {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
            ykine_move_create (YKINE_MOVE_NOTE, x_servo, "ripetere", myKINE.s_lines, 0.0, 0.0);
            if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
            if (rc >= 0)  ykine_move_repeat     (x_servo    , (int) c);
            rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
            ykine_move_create (YKINE_MOVE_NOTE, x_servo, "eretepir", myKINE.s_lines, 0.0, 0.0);
            if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
            --x_servo;
         }
      } else {
         for (i = 0; i < 3; ++i) {
            DEBUG_YKINE_SCRP   yLOG_info    ("label"     , x_servo->label);
            rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
            ykine_move_create (YKINE_MOVE_NOTE, x_servo, "ripetere", myKINE.s_lines, 0.0, 0.0);
            if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
            if (rc >= 0)  ykine_move_repeat     (x_servo    , (int) c);
            rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
            ykine_move_create (YKINE_MOVE_NOTE, x_servo, "eretepir", myKINE.s_lines, 0.0, 0.0);
            if (rc == 0)  rc = ykine_move_addloc (x_servo, xp, zp, yp);
            ++x_servo;
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> static char  /+--> parse a high level repeat -------------[ ------ [ ------ ]-+/       <* 
 *> yKINE__scrp_dsegno (void)                                                              <* 
 *> {                                                                                      <* 
 *>    /+---(locals)-----------+-----------+-+/                                            <* 
 *>    char        rce         = -10;                /+ return code for errors    +/       <* 
 *>    char        rc          = 0;                                                        <* 
 *>    char       *p           = NULL;                                                     <* 
 *>    int         i           = 0;                                                        <* 
 *>    int         j           = 0;                                                        <* 
 *>    int         x_len       = 0;                                                        <* 
 *>    int         x_servo     = -1;                                                       <* 
 *>    int         x_times     = -1;                                                       <* 
 *>    /+---(header)-------------------------+/                                            <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                     <* 
 *>    /+---(read fields)--------------------+/                                            <* 
 *>    for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {                                      <* 
 *>       /+---(parse field)-----------------+/                                            <* 
 *>       DEBUG_YKINE_SCRP   yLOG_note    ("read next field");                             <* 
 *>       p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);                            <* 
 *>       --rce;  if (p == NULL) {                                                         <* 
 *>          DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");                   <* 
 *>          DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                               <* 
 *>          break;                                                                        <* 
 *>       }                                                                                <* 
 *>       strltrim (p, ySTR_BOTH, LEN_RECD);                                               <* 
 *>       x_len = strlen (p);                                                              <* 
 *>       DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);                                <* 
 *>       /+---(handle)----------------------+/                                            <* 
 *>       switch (i) {                                                                     <* 
 *>       case  FIELD_SVO   :  /+---(servo to repeat)----+/                                <* 
 *>          x_servo = ykine_servos (p);                                                   <* 
 *>          --rce;  if (x_servo < 0) {                                                    <* 
 *>             DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");                <* 
 *>             DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                             <* 
 *>             return rce;                                                                <* 
 *>          }                                                                             <* 
 *>          break;                                                                        <* 
 *>       case  FIELD_TIMES :  /+---(times to repeat)----+/                                <* 
 *>          x_times = atoi (p);                                                           <* 
 *>          DEBUG_YKINE_SCRP  yLOG_value   ("x_times"   , x_times);                       <* 
 *>          --rce;  if (x_times < 1 || x_times > 100) {                                   <* 
 *>             DEBUG_YKINE_SCRP  yLOG_warn    ("times"     , "out of range (1 - 100)");   <* 
 *>             DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                             <* 
 *>             return rce;                                                                <* 
 *>          }                                                                             <* 
 *>          for (j = 0; j < g_nservo; ++j) {                                              <* 
 *>             if (g_servo_info [j].scrp != 'y') continue;                                <* 
 *>             yKINE_move_dalsegno (g_servo_info + j, x_times);                           <* 
 *>          }                                                                             <* 
 *>          break;                                                                        <* 
 *>       case  FIELD_COUNT :  /+---(moves to repeat)----+/                                <* 
 *>          break;                                                                        <* 
 *>       case  FIELD_ARGS  :  /+---(args)-----+/                                          <* 
 *>          yKINE__scrp_args(p);                                                          <* 
 *>          break;                                                                        <* 
 *>       }                                                                                <* 
 *>       DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");                              <* 
 *>    }                                                                                   <* 
 *>    DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");                            <* 
 *>    /+---(complete)-----------------------+/                                            <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                     <* 
 *>    return 0;                                                                           <* 
 *> }                                                                                      <*/

/*====================------------------------------------====================*/
/*===----                        script driver                         ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVER__________________o (void) {;}

char
ykine_scrp_exec         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   char        x_active    =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   x_active = s_verb_info [myKINE.s_iverb].active;
   DEBUG_YKINE_SCRP  yLOG_char    ("active"    , x_active);
   if (x_active != 'y') {
      DEBUG_YKINE_SCRP  yLOG_note    ("verb not active");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(run)----------------------------*/
   rc = s_verb_info [myKINE.s_iverb].call ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /* file reading driver ----------------------[--------[--------]-*/
yKINE_script       (float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   char        x_verb      [20] = "";;
   int         i           =    0;
   float       x_len       =  0.0;
   float       x_sec       =  0.0;
   char        x_type      =  '-';
   char        x_active    =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   ykine_scrp_init  ();
   /*---(read lines)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("read lines");
   while (1) {
      rc = ykine_parse_file ();
      DEBUG_YKINE_SCRP  yLOG_value   ("read"      , rc);
      if (rc == 0)               continue;
      if (rc < 0) {
         DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
         break;
      }
      /*---(handle verb)-----------------*/
      rc = ykine_scrp_exec ();
      DEBUG_YKINE_SCRP  yLOG_value   ("exec"      , rc);
      /*> x_active = s_verb_info [myKINE.s_iverb].active;                             <* 
       *> DEBUG_YKINE_SCRP  yLOG_char    ("active"    , x_active);                    <* 
       *> if (x_active == 'y') {                                                      <* 
       *>    rc = s_verb_info [myKINE.s_iverb].call ();                               <* 
       *> }                                                                           <*/
   }
   /*---(fix length)---------------------*/
   x_len = 0.0;
   for (i = 0; i < g_nservo; ++i) {
      yKINE_move_last_servo (i, &x_sec, NULL);
      /*> printf ("#%-2d, %-10.10s, %fs\n", i, g_servo_info [i].label, x_sec);        <*/
      if (x_sec > x_len)  x_len = x_sec;
   }
   myKINE.scrp_len = x_len;
   if (a_len != NULL)  *a_len = x_len;
   /*> printf ("so, %fs and %fs\n", x_len, myKINE.scrp_len);                          <*/
   /*> exit (1);                                                                      <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

/*> char                                                                                                      <* 
 *> ykine_parse_fields_pos  (void)                                                                            <* 
 *> {                                                                                                         <* 
 *>    return ykine_parse_fields (&(myKINE.s_beats), &(myKINE.s_xpos), &(myKINE.s_zpos), &(myKINE.s_ypos));   <* 
 *> }                                                                                                         <*/

/*> char                                                                                                      <* 
 *> ykine_parse_fields_deg  (void)                                                                            <* 
 *> {                                                                                                         <* 
 *>    return ykine_parse_fields (&(myKINE.s_beats), &(myKINE.s_femu), &(myKINE.s_pate), &(myKINE.s_tibi));   <* 
 *> }                                                                                                         <*/

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
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
