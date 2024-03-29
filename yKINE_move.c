/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


tMOVE      *m_head;
tMOVE      *m_tail;
int         m_count;



static tSERVO  *s_servo  = NULL;
static tMOVE   *s_curr   = NULL;
static int      s_finds  =    0;
static int      s_shorts =    0;



char
ykine_move_init         (void)
{
   m_head   = NULL;
   m_tail   = NULL;
   m_count  =    0;
   s_servo  = NULL;
   s_curr   = NULL;
   s_finds  =    0;
   s_shorts =    0;
   return 0;
}

/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void      o___MALLOC__________________o (void) {;}

char         /*-> create a new blank move ------------[ leaf   [fe.KB4.224.80]*/ /*-[12.0000.123.A]-*/ /*-[--.---.---.--]-*/
ykine__move_new    (tMOVE **a_move)
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  creates a new move object which is completely blank except for being
    *  linked into the master move doubly-linked list wich allows a single
    *  point of control over all moves, regardless of type or assignment
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMOVE      *x_new       = NULL;
   int         x_tries     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("can not be null");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_spoint  (*a_move);
   --rce;  if (*a_move != NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("must be null");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(default)------------------------*/
   *a_move = NULL;
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10) {
      x_new = (tMOVE *) malloc (sizeof (tMOVE));
      ++x_tries;
   }
   DEBUG_YKINE_MOVE   yLOG_sint    (x_tries);
   DEBUG_YKINE_MOVE   yLOG_spoint  (x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("could not malloc");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(overall)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("overall");
   x_new->seq      = 0;
   x_new->type     = YKINE_NULL;
   x_new->verb     = YKINE_NOOP; 
   x_new->servo    = NULL;
   x_new->line     = 0;
   strlcpy (x_new->label, "", LEN_LABEL);
   x_new->part     = '-';
   x_new->cell     = '-';
   /*---(timing)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("time");
   x_new->dur      =  0.0f;
   x_new->secs     =  0.0f;
   x_new->other    =  0;
   /*---(pure)---------------------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("loc");
   x_new->pure_rc  =     0;
   x_new->pure_d   =  0.0f;
   x_new->pure_x    =  0.0f;
   x_new->pure_z    =  0.0f;
   x_new->pure_y    =  0.0f;
   x_new->pure_xz   =  0.0f;
   /*---(adapt)--------------------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("loc");
   x_new->adapt_rc =     0;
   x_new->adapt_d  =  0.0f;
   x_new->adapt_x  =  0.0f;
   x_new->adapt_z  =  0.0f;
   x_new->adapt_y  =  0.0f;
   x_new->adapt_xz =  0.0f;
   /*---(master linked list)-------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("link");
   x_new->m_prev   = NULL;
   x_new->m_next   = NULL;
   if (m_tail == NULL) {
      m_head          = x_new;
      m_tail          = x_new;
   } else {
      x_new->m_prev   = m_tail;
      m_tail->m_next  = x_new;
      m_tail          = x_new;
   }
   x_new->s_prev   = NULL;
   x_new->s_next   = NULL;
   ++m_count;
   DEBUG_YKINE_MOVE   yLOG_sint    (m_count);
   /*---(save)---------------------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("save");
   *a_move = x_new;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> desproy an existing move ----------[ leaf   [fe.KB4.224.80]*/ /*-[12.0000.123.A]-*/ /*-[--.---.---.--]-*/
ykine__move_free   (tMOVE **a_move)
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  clears and destroys a single move entry as well as removing it from the
    *  master move doubly-linked list.
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMOVE      *x_move      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("can not be null");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_spoint  (*a_move);
   --rce;  if (*a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("can not be null");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_move = *a_move;
   /*---(remove from dependency list)----*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("remove prev dir");
   if (x_move->m_next != NULL) x_move->m_next->m_prev = x_move->m_prev;
   else                        m_tail                 = x_move->m_prev;
   DEBUG_YKINE_MOVE   yLOG_snote   ("remove next dir");
   if (x_move->m_prev != NULL) x_move->m_prev->m_next = x_move->m_next;
   else                        m_head                 = x_move->m_next;
   --m_count;
   DEBUG_YKINE_MOVE   yLOG_sint    (m_count);
   /*---(free)---------------------------*/
   DEBUG_YKINE_MOVE   yLOG_snote   ("free and ground");
   free (x_move);
   *a_move = NULL;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   return  0;
}



/*====================------------------------------------====================*/
/*===----                        move objects                          ----===*/
/*====================------------------------------------====================*/
static void      o___OBJECTS_________________o (void) {;}

char         /*--> create a new move ---------------------[ ------ [ ------ ]-*/
ykine_move_create  (
      /*----------+-----------+-----------------------------------------------*/
      tSERVO     *a_servo     ,   /* servo                                    */
      char        a_type      ,   /* type of move (pause, servo, ...)         */
      char        a_verb      ,   /* verb used for servo operation            */
      int         a_line      ,   /* source line                              */
      char       *a_label     ,   /* step label                               */
      char        a_part      ,   /* for grouping into accels                 */
      char        a_cell      ,   /* for grouping into cells                  */
      float       a_sec       )   /* duration                                 */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMOVE      *x_move      = NULL;          /* new requires entry             */
   float       x_sec       =  0.0;
   /*> float       x, z, y;                                                           <*/
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_info    ("a_label"   , a_label);
   DEBUG_YKINE_MOVE   yLOG_char    ("a_type"    , a_type);
   DEBUG_YKINE_MOVE   yLOG_double  ("a_sec"     , a_sec);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_point   ("a_servo"   , a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_MOVE   yLOG_note    ("can not add a move to a null servo");
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_info    ("name"      , a_servo->label);
   /*---(get previous)----------------*/
   /*> ykine_move_savedtail (a_servo, NULL, NULL, &x, &z, &y, NULL);                   <*/
   /*---(create a new requires)-------*/
   DEBUG_YKINE_MOVE   yLOG_note    ("add a new move object");
   rc = ykine__move_new (&x_move);
   --rce;  if (rc < 0 || x_move == NULL)   {
      DEBUG_YKINE_MOVE yLOG_exit      (__FUNCTION__);
      return rce; 
   }
   /*---(assign basics)---------------*/
   DEBUG_YKINE_MOVE   yLOG_note    ("assign basic values");
   if      (a_type == YKINE_NOTE)  x_move->type     = a_type;
   else if (a_sec  == 0.0            )  x_move->type     = YKINE_INIT;
   else                                 x_move->type     = a_type;
   x_move->verb     = a_verb;
   x_move->servo    = a_servo;
   if (a_label != NULL)  strlcpy (x_move->label, a_label, LEN_LABEL);
   x_move->part     = a_part;
   x_move->cell     = a_cell;
   x_move->line     = a_line;
   x_move->dur      = a_sec;
   /*---(hook it up to servo)---------*/
   DEBUG_YKINE_MOVE   yLOG_note    ("hook up move into servo doubly-linked list");
   if (a_servo->tail == NULL) {
      DEBUG_YKINE_MOVE   yLOG_note    ("first entry in list");
      a_servo->head         = x_move;
      a_servo->tail         = x_move;
      a_servo->count        = 1;
      DEBUG_YKINE_MOVE   yLOG_note    ("update sec/deg based on being first move");
      x_move->seq           = 1;
      x_move->secs          = a_sec;
   } else {
      DEBUG_YKINE_MOVE   yLOG_note    ("add to tail");
      x_move->s_prev        = a_servo->tail;
      a_servo->tail->s_next = x_move;
      a_servo->tail         = x_move;
      ++(a_servo->count);
      DEBUG_YKINE_MOVE   yLOG_note    ("update sec/deg based on previous move");
      x_move->seq           = x_move->s_prev->seq + 1;
      x_sec                 = x_move->s_prev->secs;
      x_move->secs          = x_sec + a_sec;
   }
   /*---(display stats)------------------*/
   DEBUG_YKINE_MOVE   yLOG_value   ("count"     , a_servo->count);
   DEBUG_YKINE_MOVE   yLOG_value   ("seq"       , x_move->seq);
   DEBUG_YKINE_MOVE   yLOG_double  ("secs"      , x_move->secs);
   /*---(update globals)-----------------*/
   DEBUG_YKINE_MOVE   yLOG_double  ("scrp_len"  , myKINE.scrp_len);
   if (x_move->secs > myKINE.scrp_len) {
      myKINE.scrp_len = x_move->secs;
      DEBUG_YKINE_MOVE   yLOG_note    ("end time greater than current length");
      DEBUG_YKINE_MOVE   yLOG_double  ("scrp_len"  , myKINE.scrp_len);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> add a location to a move object -------[ ------ [ ------ ]-*/
ykine_move_add_zero       (tSERVO *a_servo, char a_rc, float a_xpos, float a_zpos, float a_ypos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_sint    (a_rc);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_xpos);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_zpos);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_ypos);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_snote   (a_servo->label);
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo->tail);
   --rce;  if (a_servo->tail == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign basics)---------------*/
   a_servo->tail->pure_rc   = a_servo->tail->adapt_rc   = a_rc;
   a_servo->tail->pure_d    = a_servo->tail->adapt_d    = 0.0;
   a_servo->tail->pure_x    = a_servo->tail->adapt_x    = a_xpos;
   a_servo->tail->pure_z    = a_servo->tail->adapt_z    = a_zpos;
   a_servo->tail->pure_y    = a_servo->tail->adapt_y    = a_ypos;
   a_servo->tail->pure_xz   = a_servo->tail->adapt_xz   = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> add a location to a move object -------[ ------ [ ------ ]-*/
ykine_move_add_orient      (tSERVO *a_servo, char a_rc, float a_deg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_sint    (a_rc);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_deg);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_snote   (a_servo->label);
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo->tail);
   --rce;  if (a_servo->tail == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign basics)---------------*/
   a_servo->tail->pure_rc   = a_servo->tail->adapt_rc   = a_rc;
   a_servo->tail->pure_d    = a_servo->tail->adapt_d    = a_deg;
   a_servo->tail->pure_x    = a_servo->tail->adapt_x    = 0.0;
   a_servo->tail->pure_z    = a_servo->tail->adapt_z    = 0.0;
   a_servo->tail->pure_y    = a_servo->tail->adapt_y    = 0.0;
   a_servo->tail->pure_xz   = a_servo->tail->adapt_xz   = 0.0;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> add a location to a move object -------[ ------ [ ------ ]-*/
ykine_move_add_pure        (tSERVO *a_servo, char a_rc, float a_deg, float a_xpos, float a_zpos, float a_ypos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_sint    (a_rc);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_deg);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_xpos);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_zpos);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_ypos);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_snote   (a_servo->label);
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo->tail);
   --rce;  if (a_servo->tail == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign basics)---------------*/
   a_servo->tail->pure_rc   = a_rc;
   a_servo->tail->pure_d    = a_deg;
   a_servo->tail->pure_x    = a_xpos;
   a_servo->tail->pure_z    = a_zpos;
   a_servo->tail->pure_y    = a_ypos;
   a_servo->tail->pure_xz   = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_servo->tail->pure_xz);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> add a location to a move object -------[ ------ [ ------ ]-*/
ykine_move_add_adapt       (tSERVO *a_servo, char a_rc, float a_deg, float a_xpos, float a_zpos, float a_ypos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_sint    (a_rc);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_deg);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_xpos);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_zpos);
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_ypos);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_snote   (a_servo->label);
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo->tail);
   --rce;  if (a_servo->tail == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign basics)---------------*/
   a_servo->tail->adapt_rc  = a_rc;
   a_servo->tail->adapt_d   = a_deg;
   a_servo->tail->adapt_x   = a_xpos;
   a_servo->tail->adapt_z   = a_zpos;
   a_servo->tail->adapt_y   = a_ypos;
   a_servo->tail->adapt_xz  = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   DEBUG_YKINE_MOVE   yLOG_sdouble (a_servo->tail->adapt_xz);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_move__repeatnote_OLD (tSERVO *a_servo, int a_nline, int a_count, int a_segno)
{
   /*---(locals)-----------+-----------+-*/
   char        x_label     [LEN_LABEL];
   float       d, x, z, y;
   char        PURE_RC     =    0;
   /*---(labels)-------------------------*/
   if (a_count >= 0) {
      sprintf (x_label, "repeat %d,%d", a_segno, a_count + 1);
      DEBUG_YKINE_MOVE   yLOG_info    ("RIPETERE"  , x_label);
   } else {
      sprintf (x_label, "taeper %d", a_segno);
      DEBUG_YKINE_MOVE   yLOG_note    ("ERETEPIR to be created");
   }
   /*---(moves)--------------------------*/
   ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
   ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
   ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
   if (a_servo->seg == YKINE_TIBI) {
      ++a_servo;
      ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
      ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
      --a_servo;
      --a_servo;
      ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
      ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
      --a_servo;
      ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
      ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
   }
   else if (a_servo->seg == YKINE_FEMU) {
      ++a_servo;
      ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
      ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
      ++a_servo;
      ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
      ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
      ++a_servo;
      ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
      ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
      ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_move__repeatnote (tSERVO *a_servo, int a_nline, int a_count, int a_segno)
{
   /*---(locals)-----------+-----------+-*/
   char        x_label     [LEN_LABEL];
   float       d, x, z, y;
   char        PURE_RC     =    0;
   /*---(labels)-------------------------*/
   if (a_count >= 0) {
      sprintf (x_label, "repeat %d,%d", a_segno, a_count + 1);
      DEBUG_YKINE_MOVE   yLOG_info    ("RIPETERE"  , x_label);
   } else {
      sprintf (x_label, "taeper %d", a_segno);
      DEBUG_YKINE_MOVE   yLOG_note    ("ERETEPIR to be created");
   }
   /*---(moves)--------------------------*/
   ykine_scrp_prev   (a_servo->tail, &d, &x, &z, &y);
   ykine_move_create (a_servo, YKINE_NOTE, YKINE_REPT, a_nline, x_label, '-', YKINE_NONE, 0.0);
   ykine_move_add_pure (a_servo, PURE_RC, 0.0, x, z, y);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> repeat from the last segno ------------[ ------ [ ------ ]-*/
ykine_move_repeat      (tSERVO *a_servo, int a_times)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;          /* return code for errors         */
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_seg       = NULL;
   tMOVE      *x_beg       = NULL;
   tMOVE      *x_end       = NULL;
   int         x_line      =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_point   ("a_servo"   , a_servo);
   DEBUG_YKINE_MOVE   yLOG_value   ("a_times"   , a_times);
   /*---(defense)------------------------*/
   --rce;  if (a_servo        == NULL) {
      DEBUG_YKINE_MOVE   yLOG_note    ("not a valid servo");
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_info    ("servo"     , a_servo->label);
   --rce;  if (a_servo->tail  == NULL) {
      DEBUG_YKINE_MOVE   yLOG_note    ("no moves on servo, nothing to repeat");
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_value   ("nsegno"    , a_servo->nsegno);
   --rce;  if (a_servo->nsegno <= 0) {
      DEBUG_YKINE_MOVE   yLOG_note    ("no segni in servo stack");
      DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(adjust segno count)-------------*/
   --(a_servo->nsegno);
   /*---(save ripetere line)-------------*/
   x_line = myKINE.s_nline;
   /*---(find end)-----------------------*/
   x_end  = a_servo->tail;
   DEBUG_YKINE_MOVE   yLOG_complex ("x_end"     , "%3d, %3d, %s", x_end->seq, x_end->line, x_end->label);
   /*---(find segno)---------------------*/
   x_seg  = a_servo->segni [a_servo->nsegno];
   DEBUG_YKINE_MOVE   yLOG_complex ("segno"     , "%3d, %3d, %s", x_seg->seq, x_seg->line, x_seg->label);
   --rce;  if (x_seg == NULL) {
      DEBUG_YKINE_MOVE   yLOG_note    ("bad segni at current position in stack");
      ykine_move_delete (&x_end);
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_complex ("segno"     , "%3d, %3d, %s", x_seg->seq, x_seg->line, x_seg->label);
   --rce;  if (x_seg == x_end) {
      DEBUG_YKINE_MOVE   yLOG_note    ("empty loop on servo, nothing to do");
      a_servo->segni [a_servo->nsegno] = NULL;
      ykine_move_delete (&x_seg);
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find beg)-----------------------*/
   x_beg = x_seg->s_next;
   DEBUG_YKINE_MOVE   yLOG_complex ("x_beg"     , "%3d, %3d, %s", x_beg->seq, x_beg->line, x_beg->label);
   /*---(verify)-------------------------*/
   x_curr = x_beg;
   DEBUG_YKINE_MOVE   yLOG_note    ("VERIFY MOVES TO INCLUDE");
   while (x_curr != NULL) {
      DEBUG_YKINE_MOVE   yLOG_complex ("x_curr"    , "%x, %3d, %3d", x_curr, x_curr->seq, x_curr->line);
      x_curr = x_curr->s_next;
      if (x_curr == x_end)  break;
   }
   /*---(add)----------------------------*/
   for (i = 0; i < a_times; ++i) {
      ykine_move__repeatnote (a_servo, x_line, i, x_seg->seq);
      a_servo->tail->other = x_seg->seq;
      x_curr = x_beg;
      DEBUG_YKINE_MOVE   yLOG_complex ("x_curr (b)", "%x, %3d, %3d", x_curr, x_curr->seq, x_curr->line);
      while (x_curr != NULL) {
         if (x_curr->type != YKINE_NOTE) {
            strlcpy (x_label, a_servo->label, 3);
            DEBUG_YKINE_MOVE   yLOG_info    ("x_label"   , x_label);
            rc = yPARSE_reload (&(myKINE.s_nline), &(myKINE.s_cline), x_curr->line, x_label);
            DEBUG_YKINE_MOVE   yLOG_value   ("reload"    , rc);
            myKINE.s_tline = x_curr->line;
            if (rc >= 1)  rc = ykine_scrp_exec    ();
            DEBUG_YKINE_MOVE   yLOG_value   ("exec"      , rc);
         }
         if (x_curr == x_end)  break;
         DEBUG_YKINE_MOVE   yLOG_complex ("x_curr (b)", "%x, %3d, %3d", x_curr, x_curr->seq, x_curr->line);
         x_curr = x_curr->s_next;
         DEBUG_YKINE_MOVE   yLOG_complex ("x_curr (n)", "%x, %3d, %3d", x_curr, x_curr->seq, x_curr->line);
      }
   }
   ykine_move__repeatnote (a_servo, x_line, -1, x_seg->seq);
   a_servo->tail->other = x_seg->seq;
   /*---(clear)--------------------------*/
   a_servo->segni [a_servo->nsegno] = NULL;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> remove a move -------------------------[ ------ [ ------ ]-*/
ykine_move_delete  (tMOVE **a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMOVE      *x_move      = NULL;
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("can not be null");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_spoint  (*a_move);
   --rce;  if (*a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("can not be null");
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_move = *a_move;
   /*---(update timings downstream)------*/
   x_next = x_move->s_next;
   DEBUG_YKINE_MOVE   yLOG_point   ("x_next"    , x_next);
   while (x_next != NULL) {
      DEBUG_YKINE_MOVE   yLOG_value   ("sec less"  , x_move->dur);
      x_next->secs -= x_move->dur;
      x_next = x_next->s_next;
      DEBUG_YKINE_MOVE   yLOG_point   ("x_next"    , x_next);
   }
   /*---(mark deleted)-------------------*/
   DEBUG_YKINE_MOVE   yLOG_note    ("wipe move header values");
   x_move->type  = YKINE_DEL;
   x_move->secs  = 0.0;
   x_move->dur   = 0.0;
   if (x_move->s_prev != NULL) {
      x_move->pure_d  = x_move->s_prev->pure_d;
   }
   /*---(remove from servo list)---------*/
   DEBUG_YKINE_MOVE   yLOG_note    ("remove from backwards/prev direction");
   if (x_move->s_next != NULL) x_move->s_next->s_prev = x_move->s_prev;
   else                        x_move->servo->tail    = x_move->s_prev;
   DEBUG_YKINE_MOVE   yLOG_note    ("remove from forewards/next direction");
   if (x_move->s_prev != NULL) x_move->s_prev->s_next = x_move->s_next;
   else                        x_move->servo->head    = x_move->s_next;
   DEBUG_YKINE_MOVE   yLOG_note    ("decrement count");
   --x_move->servo->count;
   /*---(free memory)--------------------*/
   rc = ykine__move_free (a_move);
   DEBUG_YKINE_MOVE   yLOG_value   ("free"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> remove a move -------------------------[ ------ [ ------ ]-*/
ykine_move_clear_servo  (tSERVO *a_servo)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_prev      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_point   ("a_servo"   , a_servo);
   --rce;  if (a_servo == NULL) {
      DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_MOVE   yLOG_complex ("label"     , "%d/%d %s", a_servo->leg, a_servo->seg, a_servo->label);
   /*---(prepare)------------------------*/
   x_curr = a_servo->tail;
   DEBUG_YKINE_MOVE   yLOG_point   ("head"      , x_curr);
   while (x_curr != NULL) {
      x_prev = x_curr->s_prev;
      rc = ykine_move_delete (&x_curr);
      DEBUG_YKINE_MOVE   yLOG_value   ("delete"    , rc);
      if (rc < 0) {
         DEBUG_YKINE_MOVE   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      x_curr = x_prev;
      DEBUG_YKINE_MOVE   yLOG_point   ("prev"      , x_curr);
   }
   /*---(compete)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_move_savedtail     (tSERVO *a_servo, float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMOVE      *x_tail      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   if (a_sec != NULL)  *a_sec = 0.0;
   if (a_deg != NULL)  *a_deg = 0.0;
   if (x     != NULL)  *x     = 0.0;
   if (z     != NULL)  *z     = 0.0;
   if (y     != NULL)  *y     = 0.0;
   if (xz    != NULL)  *xz    = 0.0;
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_servo);
   --rce;  if (a_servo == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(previous)-----------------------*/
   x_tail = a_servo->tail;
   DEBUG_YKINE_MOVE   yLOG_spoint  (x_tail);
   /*---(if none)------------------------*/
   --rce;  if (x_tail == NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("none, so returning zeros");
      DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
      DEBUG_YKINE_MOVE   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   }
   /*---(exxists)------------------------*/
   else {
      DEBUG_YKINE_MOVE   yLOG_sint    (x_tail->line);
      DEBUG_YKINE_MOVE   yLOG_snote   (x_tail->label);
      if (a_sec != NULL)  *a_sec = x_tail->secs;
      if (a_deg != NULL)  *a_deg = x_tail->pure_d;
      if (x     != NULL)  *x     = x_tail->pure_x;
      if (z     != NULL)  *z     = x_tail->pure_z;
      if (y     != NULL)  *y     = x_tail->pure_y;
      if (xz    != NULL)  *xz    = x_tail->pure_xz;
      DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
      DEBUG_YKINE_MOVE   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", x_tail->secs, x_tail->pure_d, x_tail->pure_x, x_tail->pure_z, x_tail->pure_y, x_tail->pure_xz);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_move_savedcurr    (tMOVE *a_move, float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   if (a_sec != NULL)  *a_sec = 0.0;
   if (a_deg != NULL)  *a_deg = 0.0;
   if (x     != NULL)  *x     = 0.0;
   if (z     != NULL)  *z     = 0.0;
   if (y     != NULL)  *y     = 0.0;
   if (xz    != NULL)  *xz    = 0.0;
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(return data)--------------------*/
   DEBUG_YKINE_MOVE   yLOG_sint    (a_move->line);
   DEBUG_YKINE_MOVE   yLOG_snote   (a_move->label);
   if (a_sec != NULL)  *a_sec = a_move->secs;
   if (a_deg != NULL)  *a_deg = a_move->pure_d;
   if (x     != NULL)  *x     = a_move->pure_x;
   if (z     != NULL)  *z     = a_move->pure_z;
   if (y     != NULL)  *y     = a_move->pure_y;
   if (xz    != NULL)  *xz    = a_move->pure_xz;
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", a_move->secs, a_move->pure_d, a_move->pure_x, a_move->pure_z, a_move->pure_y, a_move->pure_xz);
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_move_savedprev    (tMOVE *a_move, float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMOVE      *x_prev      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_MOVE   yLOG_senter  (__FUNCTION__);
   if (a_sec != NULL)  *a_sec = 0.0;
   if (a_deg != NULL)  *a_deg = 0.0;
   if (x     != NULL)  *x     = 0.0;
   if (z     != NULL)  *z     = 0.0;
   if (y     != NULL)  *y     = 0.0;
   if (xz    != NULL)  *xz    = 0.0;
   /*---(defense)------------------------*/
   DEBUG_YKINE_MOVE   yLOG_spoint  (a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_MOVE   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(previous)-----------------------*/
   if (a_move->s_prev != NULL) {
      DEBUG_YKINE_MOVE   yLOG_snote   ("prev good");
      x_prev = a_move->s_prev;
      if (a_sec != NULL)  *a_sec = x_prev->secs;
   } else  {
      DEBUG_YKINE_MOVE   yLOG_snote   ("no prev, use move");
      x_prev = a_move;
      if (a_sec != NULL)  *a_sec = 0.0;  /* all same, exact secs zero */
   }
   DEBUG_YKINE_MOVE   yLOG_spoint  (x_prev);
   /*---(return data)--------------------*/
   DEBUG_YKINE_MOVE   yLOG_sint    (x_prev->line);
   DEBUG_YKINE_MOVE   yLOG_snote   (x_prev->label);
   if (a_deg != NULL)  *a_deg = x_prev->pure_d;
   if (x     != NULL)  *x     = x_prev->pure_x;
   if (z     != NULL)  *z     = x_prev->pure_z;
   if (y     != NULL)  *y     = x_prev->pure_y;
   if (xz    != NULL)  *xz    = x_prev->pure_xz;
   DEBUG_YKINE_MOVE   yLOG_sexit   (__FUNCTION__);
   DEBUG_YKINE_MOVE   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", x_prev->secs, x_prev->pure_d, x_prev->pure_x, x_prev->pure_z, x_prev->pure_y, x_prev->pure_xz);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      current position                        ----===*/
/*====================------------------------------------====================*/
static void      o___CURRENT_________________o (void) {;}

char
ykine_exact_calc_length (float xp, float xc, float zp, float zc, float a_pct, float *l)
{
   float       lp, lc;
   lp = sqrt ((xp * xp) + (zp * zp));
   lc = sqrt ((xc * xc) + (zc * zc));
   ykine_exact_calc ('-', lp, lc, a_pct, l);
   DEBUG_YKINE_EXACT  yLOG_complex ("polar_len" , "%8.2fp, %8.2fc, %8.2fp, %8.2f", lp, lc, *l);
   return 0;
}

char
ykine_exact_calc_polar  (float l, float d, float *x, float *z)
{
   *x =  l * sin (d * DEG2RAD);
   *z = -l * cos (d * DEG2RAD);
   DEBUG_YKINE_EXACT  yLOG_complex ("polar_xz"  , "%8.2fx, %8.2fz", *x, *z);
   return 0;
}

char         /*--> calc the current deg for a servo ------[ ------ [ ------ ]-*/
ykine__exact_data        (tSERVO *a_servo, float a_sec)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* generic return code            */
   tMOVE      *x_curr      = NULL;
   float       x_pct       = 0.0;
   float       x_range     = 0.0;
   float       dp, dc, lp, lc, l;
   float       xp, xc, zp, zc, yp, yc;
   float       sp, sc;
   /*---(header)-------------------------*/
   DEBUG_YKINE_EXACT  yLOG_enter   (__FUNCTION__);
   /*---(verify current)-----------------*/
   rc = ykine__exact_find  (a_servo, a_sec);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_EXACT  yLOG_note    ("no valid move for time requested");
      DEBUG_YKINE_EXACT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_curr  = a_servo->curr;
   DEBUG_YKINE_EXACT  yLOG_complex ("verb"      , "%s at %d", x_curr->label, x_curr->line);
   /*---(get current values)-------------*/
   xc  = x_curr->pure_x;
   zc  = x_curr->pure_z;
   yc  = x_curr->pure_y;
   sc  = x_curr->secs;
   dc  = x_curr->pure_d;
   DEBUG_YKINE_EXACT  yLOG_complex ("current"   , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy", sc, dc, xc, zc, yc);
   /*---(get previous values)---------*/
   rc = ykine_move_savedprev  (x_curr, &sp, &dp, &xp, &zp, &yp, NULL);
   /*---(calc percent)-------------------*/
   x_range = sc - sp;
   if (x_range == 0.0)  x_pct   = 0.0;
   else                 x_pct   = (a_sec - sp) / x_range;
   DEBUG_YKINE_EXACT  yLOG_complex ("percent"   , "%8.2fb, %8.2fe, %8.2fr, %8.2fp", sp, sc, x_range, x_pct);
   ykine_exact_calc ('d', dp, dc, x_pct, &(a_servo->deg));
   DEBUG_YKINE_EXACT  yLOG_complex ("degrees"   , "%8.2fb, %8.2fe, %8.2fc", dp, dc, a_servo->deg);
   ykine_exact_calc ('-', yp, yc, x_pct, &(a_servo->yexp));
   if (strncmp (x_curr->label, "zp_", 3) == 0) {
      ykine_exact_calc_length (xp, xc, zp, zc, x_pct, &l);
      ykine_exact_calc_polar  (l, a_servo->deg, &(a_servo->xexp), &(a_servo->zexp));
   } else {
      DEBUG_YKINE_EXACT  yLOG_note    ("normal verb");
      ykine_exact_calc ('-', xp, xc, x_pct, &(a_servo->xexp));
      ykine_exact_calc ('-', zp, zc, x_pct, &(a_servo->zexp));
   }
   DEBUG_YKINE_EXACT  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", a_servo->xexp, a_servo->zexp, a_servo->yexp);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_EXACT  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     sequential search                        ----===*/
/*====================------------------------------------====================*/
static void      o___SEQUENCIAL______________o (void) {;}

char
ykine_servo__cursor     (char a_dir, char a_type, float *a, float *b, float *c, float *d)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_schar   (a_dir);
   /*---(prepare)------------------------*/
   if (a  != NULL)  *a  = 0.0;
   if (b  != NULL)  *b  = 0.0;
   if (c  != NULL)  *c  = 0.0;
   if (d  != NULL)  *d  = 0.0;
   /*---(defense)------------------------*/
   --rce;  if (s_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("servo no found");
      return rce;
   }
   /*---(update current)-----------------*/
   DEBUG_YKINE_SCRP   yLOG_schar   (a_dir);
   --rce;  switch (a_dir) {
   case YKINE_HEAD : s_curr = s_servo->head;  a_dir = YKINE_NEXT;   break;
   case YKINE_NEXT : if (s_curr != NULL)  s_curr = s_curr->s_next;  break;
   case YKINE_SAME : break;
   case YKINE_PREV : if (s_curr != NULL)  s_curr = s_curr->s_prev;  break;
   case YKINE_TAIL : s_curr = s_servo->tail;  a_dir = YKINE_PREV;   break;
   default  : /*---(problem)-------------*/
                     DEBUG_YKINE_SCRP   yLOG_snote   ("invalid dir");
                     return rce;
                     break;
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_curr);
   /*---(check for next)-----------------*/
   while (s_curr != NULL) {
      DEBUG_YKINE_SCRP   yLOG_schar   (s_curr->type);
      if (s_curr->type == YKINE_INIT )  break;
      if (s_curr->type == YKINE_SERVO)  break;
      if (s_curr->type == YKINE_WAIT )  break;
      switch (a_dir) {
      case YKINE_NEXT : s_curr = s_curr->s_next;  break;
      case YKINE_PREV : s_curr = s_curr->s_prev;  break;
      default         : break;
      }
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_curr);
   /*---(refuse null)--------------------*/
   --rce;  if (s_curr == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no more moves");
      return rce;
   }
   /*---(save position)------------------*/
   DEBUG_YKINE_SCRP   yLOG_svalue  ("found move", s_curr->seq);
   /*---(return values)------------------*/
   DEBUG_YKINE_SCRP   yLOG_schar   (a_type);
   --rce;  switch (a_type) {
   case YKINE_DEG :
      if (a  != NULL)  *a  = s_curr->secs;
      if (b  != NULL)  *b  = s_curr->pure_d;
      break;
   case YKINE_POS :
      if (a  != NULL)  *a  = s_curr->secs;
      if (b  != NULL)  *b  = s_curr->pure_x;
      if (c  != NULL)  *c  = s_curr->pure_z;
      if (d  != NULL)  *d  = s_curr->pure_y;
      break;
   default  :
      DEBUG_YKINE_SCRP   yLOG_snote   ("invalid type");
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_zero_cursor       (char a_dir, float *s, float *x, float *z, float *y)
{
   /*---(prepare)------------------------*/
   if (s_servo == NULL || s_servo->leg != YKINE_BODY || s_servo->seg != YKINE_FOCU) {
      ++s_finds;
      s_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   } else {
      ++s_shorts;
   }
   /*---(call)---------------------------*/
   return ykine_servo__cursor (a_dir, YKINE_POS, s, x, z, y);
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_cursor        (char a_dir, char a_leg, char a_seg, float *s, float *d)
{
   /*---(prepare)------------------------*/
   if (s_servo == NULL || s_servo->leg != a_leg || s_servo->seg != a_seg) {
      ++s_finds;
      s_servo = ykine_servo_pointer (a_leg, a_seg);
   } else {
      ++s_shorts;
   }
   /*---(call)---------------------------*/
   return ykine_servo__cursor (a_dir, YKINE_DEG, s, d, NULL, NULL);
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_tail         (int a_servo, float *s, float *d)
{
   if (a_servo < 0 || a_servo >= g_nservo)  return -1; 
   return yKINE_move_cursor (YKINE_TAIL, g_servo_info [a_servo].leg, g_servo_info [a_servo].seg, s, d);
}

char         /*--> retrieve data about current -----------[ ------ [ ------ ]-*/
ykine_move_curdata       (double *a_x, double *a_z, double *a_y)
{
   if (s_curr == NULL)   return -1;
   if (a_x != NULL)  *a_x = s_curr->pure_x;
   if (a_z != NULL)  *a_z = s_curr->pure_z;
   if (a_y != NULL)  *a_y = s_curr->pure_y;
   return 0;
}

/*> char         /+--> get the current deg for servo ---------[ ------ [ ------ ]-+/            <* 
 *> yKINE_servo_deg          (int a_leg, int a_seg, float *a_deg)                               <* 
 *> {                                                                                           <* 
 *>    tSERVO     *x_servo     = NULL;                                                          <* 
 *>    double      x_deg       = 0.0;                                                           <* 
 *>    /+> if (a_leg < YKINE_BODY || a_leg > YKINE_LR  )  return -1;                      <*    <* 
 *>     *> if (a_seg < YKINE_FEMU || a_seg > YKINE_TIBI)  return -2;                      <+/   <* 
 *>    x_servo = ykine_servo_pointer (a_leg, a_seg);                                            <* 
 *>    if (x_servo->curr == NULL) {                                                             <* 
 *>       if (a_seg == YKINE_FEMU)  x_deg  =  0.0;                                              <* 
 *>       if (a_seg == YKINE_PATE)  x_deg  =  0.0;                                              <* 
 *>       if (a_seg == YKINE_TIBI)  x_deg  =  0.0;                                              <* 
 *>       if (a_deg       != NULL)  *a_deg = x_deg;                                             <* 
 *>       return -1;                                                                            <* 
 *>    }                                                                                        <* 
 *>    if (a_deg       != NULL)  *a_deg = x_servo->deg;                                         <* 
 *>    if (x_servo->exact == 'y')  return 1;                                                    <* 
 *>    return 0;                                                                                <* 
 *> }                                                                                           <*/

/*> char         /+--> get the current details for servo -----[ ------ [ ------ ]-+/                                                                                      <* 
 *> yKINE_servo_move         (int a_leg, int a_seg, char *a_label, float *a_secb, float *a_sece, float *a_dur , float *a_degb, float *a_dege, int *a_seq , int *a_line)   <* 
 *> {                                                                                                                                                                     <* 
 *>    tSERVO     *x_servo     = NULL;                                                                                                                                    <* 
 *>    double      x_deg       = 0.0;                                                                                                                                     <* 
 *>    float       dp, sp;                                                                                                                                                <* 
 *>    if (a_leg < YKINE_RR   || a_leg > YKINE_LR  )  return -1;                                                                                                          <* 
 *>    if (a_seg < YKINE_FEMU || a_seg > YKINE_TIBI)  return -2;                                                                                                          <* 
 *>    x_servo = ykine_servo_pointer (a_leg, a_seg);                                                                                                                      <* 
 *>    if (x_servo == NULL)  return -1;                                                                                                                                   <* 
 *>    ykine_move_savedprev  (x_servo->curr, &sp, &dp, NULL, NULL, NULL, NULL);                                                                                           <* 
 *>    if (a_label     != NULL)  strlcpy (a_label, x_servo->curr->label, LEN_LABEL);                                                                                      <* 
 *>    if (a_secb      != NULL)  *a_secb = sp;                                                                                                                            <* 
 *>    if (a_sece      != NULL)  *a_sece = x_servo->curr->secs;                                                                                                           <* 
 *>    if (a_dur       != NULL)  *a_dur  = x_servo->curr->dur;                                                                                                            <* 
 *>    if (a_degb      != NULL)  *a_degb = dp;                                                                                                                            <* 
 *>    if (a_dege      != NULL)  *a_dege = x_servo->curr->pure_d;                                                                                                           <* 
 *>    if (a_seq       != NULL)  *a_seq  = x_servo->curr->seq;                                                                                                            <* 
 *>    if (a_line      != NULL)  *a_line = x_servo->curr->line;                                                                                                           <* 
 *>    if (x_servo->exact == 'y')  return 1;                                                                                                                              <* 
 *>    return 0;                                                                                                                                                          <* 
 *> }                                                                                                                                                                     <*/

/*> char         /+--> get the current deg for servo ---------[ ------ [ ------ ]-+/                                                <* 
 *> yKINE_servo_line         (int a_leg, int a_seg, float *a_x1, float *a_z1, float *a_y1, float *a_x2, float *a_z2, float *a_y2)   <* 
 *> {                                                                                                                               <* 
 *>    char        rce         =  -10;                                                                                              <* 
 *>    tSERVO     *x_servo     = 0;                                                                                                 <* 
 *>    double      x_deg       = 0.0;                                                                                               <* 
 *>    tMOVE      *x_curr      = NULL;                                                                                              <* 
 *>    tMOVE      *x_prev      = NULL;                                                                                              <* 
 *>    x_servo   = ykine_servo_pointer (a_leg, a_seg);                                                                              <* 
 *>    --rce;  if (x_servo == NULL)    return rce;                                                                                  <* 
 *>    x_curr = x_servo->curr;                                                                                                      <* 
 *>    --rce;  if (x_curr  == NULL)    return rce;                                                                                  <* 
 *>    x_prev = x_servo->curr->s_prev;                                                                                              <* 
 *>    while  (x_prev != NULL) {                                                                                                    <* 
 *>       if (x_prev->type == YKINE_INIT )  break;                                                                                  <* 
 *>       if (x_prev->type == YKINE_SERVO)  break;                                                                                  <* 
 *>       if (x_prev->type == YKINE_WAIT )  break;                                                                                  <* 
 *>       x_prev = x_prev->s_prev;                                                                                                  <* 
 *>    }                                                                                                                            <* 
 *>    if (x_prev        == NULL)      return -4;                                                                                   <* 
 *>    if (a_x1 != NULL)  *a_x1 = x_prev->pure_x;                                                                                    <* 
 *>    if (a_z1 != NULL)  *a_z1 = x_prev->pure_z;                                                                                    <* 
 *>    if (a_y1 != NULL)  *a_y1 = x_prev->pure_y;                                                                                    <* 
 *>    if (a_x2 != NULL)  *a_x2 = x_curr->pure_x;                                                                                    <* 
 *>    if (a_z2 != NULL)  *a_z2 = x_curr->pure_z;                                                                                    <* 
 *>    if (a_y2 != NULL)  *a_y2 = x_curr->pure_y;                                                                                    <* 
 *>    return 0;                                                                                                                    <* 
 *> }                                                                                                                               <*/



/*====================------------------------------------====================*/
/*===----                       reporting                              ----===*/
/*====================------------------------------------====================*/
static void      o___REPORTS_________________o (void) {;}

char
yKINE_move_rpt     (void)
{
   int         i           =    0;
   int         j           =    0;
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_move      = NULL;
   int         x_count     =    0;
   int         x_level     =    0;
   char        x_prefix    [LEN_LABEL];
   char        t           [LEN_LABEL];
   char        x_verb      [LEN_LABEL];
   char        x_turtle    =  '-';
   printf ("yKINE scripting report of all servo moves\n");
   printf ("\n");
   for (i = 0; i < g_nservo; ++i) {
      x_servo = g_servo_info + i;
      printf ("%2d) %s\n", i, x_servo->label);
      x_move  = x_servo->head;
      x_count = 0;
      x_level = 0;
      strlcpy (x_prefix, "", LEN_LABEL);
      while (x_move != NULL) {
         /*---(heads)--------------------*/
         if (x_count % 45 == 0)  printf ("\n   seq-  t  ---verb--------  ---label-------  line  n  --dur---  --secs--     pure  --degs--  --xpos--  --zpos--  --ypos--  ---xz---     adapt --degs--  --xpos--  --zpos--  --ypos--  ---xz---\n");
         if (x_count %  5 == 0)  printf ("\n");
         /*---(indents)------------------*/
         strlcpy (x_prefix, "", LEN_LABEL);
         ykine_scrp_by_code (x_move->verb, NULL, t, NULL);
         x_turtle = '-';
         if (strcmp (t, "zero") == 0 || strcmp (t, "turn") == 0 || strcmp (t, "head") == 0)  {
            if (strcmp (x_move->label, ""      ) != 0) {
               strlcpy (t, "turtle"     , LEN_LABEL);
               x_turtle = 'y';
            }
         }
         if (strcmp (t, "segno") == 0) {
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "��", LEN_LABEL);
            ++x_level;
         } else if (strncmp (x_move->label, "taeper", 6) == 0) {
            --x_level;
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "��", LEN_LABEL);
         } else if (strcmp (t, "ripetere") == 0) {
            --x_level;
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "��", LEN_LABEL);
            ++x_level;
         } else {
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "��", LEN_LABEL);
         }
         sprintf (x_verb, "%s%s", x_prefix, t);
         /*---(keys)---------------------*/
         printf ("   %4d  %c  %-15.15s  %-15.15s  %4d  %c",
               x_move->seq, x_move->type, x_verb, x_move->label,
               x_move->line, x_move->cell);
         /*---(timing)-------------------*/
         if (x_move->dur == 0.0)       printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->dur);
         if (x_move->secs == 0.0)      printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->secs);
         /*---(pure)---------------------*/
         printf ("   ");
         if (x_move->pure_rc   == 0.0) printf ("    --");
         else                          printf ("  %4d"     , x_move->pure_rc);
         if (x_turtle          == 'y') printf ("  %8.2lf"  , x_move->pure_d);
         else if (x_move->pure_d == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->pure_d);
         if (x_move->pure_x    == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->pure_x);
         if (x_move->pure_z    == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->pure_z);
         if (x_move->pure_y    == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->pure_y);
         if (x_move->pure_xz   == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->pure_xz);
         /*---(adapt)--------------------*/
         printf ("   ");
         if (x_move->adapt_rc  == 0.0) printf ("    --");
         else                          printf ("  %4d"     , x_move->adapt_rc);
         if (x_move->adapt_d   == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->adapt_d);
         if (x_move->adapt_x   == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->adapt_x);
         if (x_move->adapt_z   == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->adapt_z);
         if (x_move->adapt_y   == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->adapt_y);
         if (x_move->adapt_xz  == 0.0) printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->adapt_xz);
         /*---(end-of-line)--------------*/
         printf ("\n");
         x_move  = x_move->s_next;
         ++x_count;
         /*---(done)---------------------*/
      }
      printf ("\n");
   }
   printf ("\nend-of-report\n\n");
   fflush (stdout);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_move        (char *a_question, int a_leg, int a_seg, int a_cnt)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        t           [LEN_LABEL] = "";
   char        x_msg       [LEN_RECD ];
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_move      = NULL;
   tMOVE      *x_next      = NULL;
   tMOVE      *x_prev      = NULL;
   int         c           =   -1;
   float       dp, sp;
   float       x_dist      =  0.0;
   float       x, z, y;
   int         x_fore      =    0;
   int         x_back      =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "MOVE unit      : question not understood", LEN_RECD);
   /*---(supporting data)----------------*/
   if (a_leg >= 0) {
      /*---(get servo)-------------------*/
      x_servo = ykine_servo_pointer (a_leg, a_seg);
      if (x_servo == NULL) {
         sprintf  (ykine__unit_answer, "MOVE unit      : could not locate leg %d/seq %d", a_leg, a_seg);
         return ykine__unit_answer;
      }
      /*---(get move)--------------------*/
      if (a_cnt < 0)  x_move = x_servo->tail;
      else {
         x_move = x_servo->head;
         /*> if (x_move  == NULL) {                                                          <* 
          *>    sprintf  (ykine__unit_answer, "MOVE unit      : no moves for this servo");   <* 
          *>    return ykine__unit_answer;                                                   <* 
          *> }                                                                               <*/
         for (i = 0; i < a_cnt; ++i) {
            if (x_move == NULL)  break;
            x_move = x_move->s_next;
         }
      }
      /*> if (x_move == NULL) {                                                       <* 
       *>    sprintf  (ykine__unit_answer, "MOVE unit      : past end of moves");     <* 
       *>    return ykine__unit_answer;                                               <* 
       *> }                                                                           <*/
      /*---(done)------------------------*/
   }
   if (x_move != NULL) {
      ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      ykine_move_savedprev (x_move, &sp, &dp, NULL, NULL, NULL, NULL);
      x_prev = x_move->s_prev;
      if (x_prev != NULL) {
         x = x_move->pure_x - x_prev->pure_x;
         z = x_move->pure_z - x_prev->pure_z;
         y = x_move->pure_y - x_prev->pure_y;
         x_dist = sqrt ((x * x) + (z * z) + (y * y));
      }
   }
   /*---(answer)-------------------------*/
   if (strcmp (a_question, "m_count" ) == 0) {
      x_next = m_head;  while (x_next != NULL)  { ++x_fore; x_next = x_next->m_next; }
      x_prev = m_tail;  while (x_prev != NULL)  { ++x_back; x_prev = x_prev->m_prev; }
      sprintf (ykine__unit_answer, "MOVE m_counts  : %2df  %2db  %2dc   %ch   %ct", x_fore, x_back, m_count, (m_head != NULL) ? 'Y' : '_', (m_tail != NULL) ? 'Y' : '_');
   }
   else if (strcmp (a_question, "s_count" ) == 0) {
      x_next = x_servo->head;  while (x_next != NULL)  { ++x_fore; x_next = x_next->s_next; }
      x_prev = x_servo->tail;  while (x_prev != NULL)  { ++x_back; x_prev = x_prev->s_prev; }
      sprintf (ykine__unit_answer, "MOVE s_counts  : %2df  %2db  %2dc   %ch   %ct", x_fore, x_back, x_servo->count, (x_servo->head != NULL) ? 'Y' : '_', (x_servo->tail != NULL) ? 'Y' : '_');
   }
   else if (strcmp (a_question, "m_last"  ) == 0) {
      x_move = m_tail;
      if (x_move != NULL)  ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE m_last    : %3d %c %2d %-5.5s %c %-20.20s %6.3lfs %6.1lfd", -1, '-', -1, "-", '-', "-", 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE m_last    : %3d %c %2d %-5.5s %c %-20.20s %6.3lfs %6.1lfd", x_move->line, x_move->type, x_move->verb, t, x_move->cell, x_move->label, x_move->secs, x_move->pure_d);
   }
   else if (strcmp (a_question, "m_lastpu") == 0) {
      x_move = m_tail;
      if (x_move != NULL)  ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE m_last pu : %3d %c %2d %-5.5s %c  %6.1lfs  %4dr  %6.1lfd  %6.1lfx  %6.1lfz  %6.1lfy  %6.1lfxz", -1, '-', -1, "-", '-', 0.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE m_last pu : %3d %c %2d %-5.5s %c  %6.1lfs  %4dr  %6.1lfd  %6.1lfx  %6.1lfz  %6.1lfy  %6.1lfxz", x_move->line, x_move->type, x_move->verb, t, x_move->cell, x_move->secs, x_move->pure_d, x_move->pure_rc, x_move->pure_x, x_move->pure_z, x_move->pure_y, x_move->pure_xz);
   }
   else if (strcmp (a_question, "m_lastad") == 0) {
      x_move = m_tail;
      if (x_move != NULL)  ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE m_last ad : %3d %c %2d %-5.5s %c  %6.1lfs  %4dr  %6.1lfd  %6.1lfx  %6.1lfz  %6.1lfy  %6.1lfxz", -1, '-', -1, "-", '-', 0.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE m_last ad : %3d %c %2d %-5.5s %c  %6.1lfs  %4dr  %6.1lfd  %6.1lfx  %6.1lfz  %6.1lfy  %6.1lfxz", x_move->line, x_move->type, x_move->verb, t, x_move->cell, x_move->secs, x_move->adapt_d, x_move->adapt_rc, x_move->adapt_x, x_move->adapt_z, x_move->adapt_y, x_move->adapt_xz);
   }
   else if (strcmp (a_question, "m_loc"   ) == 0) {
      x_move = m_tail;
      if (x_move != NULL)  ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE m_loc     : %6.3lfs %6.1lfd %6.1lfx %6.1lfz %6.1lfy", 0.0, 0.0, 0.0, 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE m_loc     : %6.3lfs %6.1lfd %6.1lfx %6.1lfz %6.1lfy", x_move->secs, x_move->pure_d, x_move->pure_x, x_move->pure_z, x_move->pure_y);
   }
   else if (strcmp (a_question, "s_last"  ) == 0) {
      x_move = x_servo->tail;
      if (x_move != NULL)  ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE s_last    : %3d %c %2d %-5.5s %c %-20.20s %6.3lfs %6.1lfd", -1, '-', -1, "-", '-', "-", 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE s_last    : %3d %c %2d %-5.5s %c %-20.20s %6.3lfs %6.1lfd", x_move->line, x_move->type, x_move->verb, t, x_move->cell, x_move->label, x_move->secs, x_move->pure_d);
   }
   else if (strcmp (a_question, "s_loc"   ) == 0) {
      x_move = x_servo->tail;
      if (x_move != NULL)  ykine_scrp_by_code   (x_move->verb, t, NULL, NULL);
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE s_loc     : %6.3lfs %6.1lfd %6.1lfx %6.1lfz %6.1lfy", 0.0, 0.0, 0.0, 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE s_loc     : %6.3lfs %6.1lfd %6.1lfx %6.1lfz %6.1lfy", x_move->secs, x_move->pure_d, x_move->pure_x, x_move->pure_z, x_move->pure_y);
   }
   else if (strcmp (a_question, "specific") == 0) {
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE specific  : %3d %c %2d %-5.5s %c %-20.20s %6.3lfs %6.1lfd", -1, '-', -1, "-", '-', "-", 0.0, 0.0);
      else                 sprintf (ykine__unit_answer, "MOVE specific  : %3d %c %2d %-5.5s %c %-20.20s %6.3lfs %6.1lfd", x_move->line, x_move->type, x_move->verb, t, x_move->cell, x_move->label, x_move->secs, x_move->pure_d);
   }
   else if (strcmp (a_question, "header"  ) == 0) {
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE header    : --/-- - -- ----- - --------s --------e --------d");
      else                 sprintf (ykine__unit_answer, "MOVE header    : %2d/%2d %c %2d %-5.5s %c %8.3lfs %8.3lfe %8.3lfd", x_move->seq, x_servo->count, x_move->type, x_move->verb, t, x_move->cell, sp, x_move->secs, x_move->dur);
   }
   else if (strcmp (a_question, "detail"  ) == 0) {
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE detail    : --------d --------x --------z --------y --------o");
      else                 sprintf (ykine__unit_answer, "MOVE detail    : %8.1lfd %8.1lfx %8.1lfz %8.1lfy %8do", x_move->pure_d, x_move->pure_x, x_move->pure_z, x_move->pure_y, x_move->other);
   }
   else if (strcmp (a_question, "ticked"  ) == 0) {
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE tick (--) : -- ----   ----- -    -s    -e    -d        -d      -x      -z      -y");
      else                 sprintf (ykine__unit_answer, "MOVE tick (%2d) : %-2.2s %-4.4s   %-5.5s %c %4.1lfs %4.1lfe %4.1lfd   %6.1lfd %6.1lfx %6.1lfz %6.1lfy", x_move->seq, yKINE_legcaps (a_leg), yKINE_segfour (a_seg), t, x_move->cell, sp, x_move->secs, x_move->dur, x_move->pure_d, x_move->pure_x, x_move->pure_z, x_move->pure_y);
   }
   else if (strcmp (a_question, "accel"   ) == 0) {
      if (x_move == NULL)  sprintf (ykine__unit_answer, "MOVE accel     :      -.-s      -.-d      -.-x      -.-z      -.-y      -.-l  -  -");
      else                 sprintf (ykine__unit_answer, "MOVE accel     : %8.1lfs %8.1lfd %8.1lfx %8.1lfz %8.1lfy %8.1lfl  %c  %c", x_move->dur, x_move->pure_d, x_move->pure_x, x_move->pure_z, x_move->pure_y, x_dist, x_move->part, x_move->cell);
   }
   else if (strcmp (a_question, "counts"  ) == 0) {
      strlcpy (ykine__unit_answer, "MOVE counts    : ", LEN_RECD);
      for (i = 0; i < g_nservo; ++i) {
         if (i > 0 && i < 42 && i % 4 == 0)  strlcat (ykine__unit_answer, " ", LEN_RECD);
         if (i == g_nservo - 1)              strlcat (ykine__unit_answer, " ", LEN_RECD);
         if (g_servo_info [i].count <= 9)  sprintf (x_msg, "%c", g_servo_info [i].count + '0');
         else                              sprintf (x_msg, "%c", g_servo_info [i].count - 10 + 'a');
         strlcat (ykine__unit_answer, x_msg, LEN_RECD);
      }
   }
   else if (strcmp (a_question, "current" ) == 0) {
      ykine_move_savedprev  (s_curr, &sp, &dp, NULL, NULL, NULL, NULL);
      if (s_curr == NULL)  sprintf (ykine__unit_answer, "MOVE current   : no current move");
      else {
         ykine_scrp_by_code (s_curr->verb, t, NULL, NULL);
         sprintf (ykine__unit_answer, "MOVE current   : %2d/%2d %c %2d %-5.5s %8.3lf %8.3lf %8.3lf", s_curr->seq, s_servo->count, s_curr->type, s_curr->verb, t, sp, s_curr->secs, s_curr->dur);
      }
   }
   else if (strcmp (a_question, "exact"   ) == 0) {
      sprintf (ykine__unit_answer, "MOVE exact     : %c %8.1lfd, %8.1lfx, %8.1lfz, %8.1lfy", x_servo->exact, x_servo->deg, x_servo->xexp, x_servo->zexp, x_servo->yexp);
   }
   else if (strcmp (a_question, "calc"    ) == 0) {
      sprintf (ykine__unit_answer, "MOVE calc curr : %4.2fp, %8.2fd, %8.1fx, %8.1fz, %8.1fy", myKINE.pct, g_cur.deg, g_cur.ex , g_cur.ez , g_cur.ey );
   }
   /*> else if (strcmp (a_question, "accel_dist") == 0) {                                  <* 
    *>    strlcpy (x_msg, "", LEN_RECD);                                                    <* 
    *>    for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {                                 <* 
    *>       if (g_accel_info [i].dist == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);           <* 
    *>       else                          sprintf (t, " %4.1f", g_accel_info [i].dist);   <* 
    *>       strlcat  (x_msg, t, LEN_RECD);                                                 <* 
    *>    }                                                                                <* 
    *>    sprintf (ykine__unit_answer, "MOVE accel dist:%s", x_msg);                       <* 
    *> }                                                                                   <*/
   /*> else if (strcmp (a_question, "accel_durs") == 0) {                                 <* 
    *>    strlcpy (x_msg, "", LEN_RECD);                                                   <* 
    *>    for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {                                <* 
    *>       if (g_accel_info [i].dur  == 0.0)  strlcpy (t, "  -.-", LEN_LABEL);          <* 
    *>       else                          sprintf (t, " %4.1f", g_accel_info [i].dur);   <* 
    *>       strlcat  (x_msg, t, LEN_RECD);                                                <* 
    *>    }                                                                               <* 
    *>    sprintf (ykine__unit_answer, "MOVE accel durs:%s", x_msg);                      <* 
    *> }                                                                                  <*/
   /*> else if (strcmp (a_question, "accel_cums") == 0) {                                 <* 
    *>    strlcpy (x_msg, "", LEN_RECD);                                                   <* 
    *>    for (i = ACCEL_TURTLE; i <= DECEL_TURTLE; ++i) {                                <* 
    *>       if (g_accel_info [i].pct  == 0.0)  strlcpy (t, " -.--", LEN_LABEL);          <* 
    *>       else                          sprintf (t, " %4.2f", g_accel_info [i].pct);   <* 
    *>       strlcat  (x_msg, t, LEN_RECD);                                                <* 
    *>    }                                                                               <* 
    *>    sprintf (ykine__unit_answer, "MOVE accel cums:%s", x_msg);                      <* 
    *> }                                                                                  <*/
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
