/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


tMOVE      *m_head;
tMOVE      *m_tail;
int         m_count;



/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void      o___MALLOC__________________o (void) {;}

static tMOVE*  /*--> create a new, blank move --------------[ leaf   [ ------ ]-*/
yKINE__move_new    (void)
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  creates a new move object which is completely blank except for being
    *  linked into the master move doubly-linked list wich allows a single
    *  point of control over all moves, regardless of type or assignment
    */
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_new       = NULL;
   int         x_tries     = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(create)-------------------------*/
   while (x_new == NULL && x_tries < 10) {
      x_new = (tMOVE *) malloc (sizeof (tMOVE));
      ++x_tries;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("tries"     , x_tries);
   DEBUG_YKINE_SCRP   yLOG_point   ("x_new"     , x_new);
   if (x_new == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("could not malloc a new move object");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(overall)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default overall values");
   x_new->servo   = NULL;
   x_new->type    = YKINE_MOVE_NULL;
   x_new->seq     = 0;
   strlcpy (x_new->label, "", LEN_LABEL);
   /*---(position)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default positions");
   x_new->degs    =  0.0f;
   /*---(timing)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default timings");
   x_new->dur     =  0.0f;
   x_new->secs    =  0.0f;
   x_new->other   =  0;
   /*---(location)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default locations");
   x_new->x_pos   =  0.0f;
   x_new->z_pos   =  0.0f;
   x_new->y_pos   =  0.0f;
   x_new->xz_len  =  0.0f;
   /*---(master linked list)-------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("tie into master doubly-linked move list");
   x_new->m_prev  = NULL;
   x_new->m_next  = NULL;
   if (m_tail == NULL) {
      m_head         = x_new;
      m_tail         = x_new;
   } else {
      x_new->m_prev  = m_tail;
      m_tail->m_next = x_new;
      m_tail         = x_new;
   }
   ++m_count;
   /*---(servo linked list)--------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default servo linked list");
   x_new->s_prev  = NULL;
   x_new->s_next  = NULL;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return x_new;
}

static tMOVE*  /*--> destroy a single move -----------------[ leaf   [ ------ ]-*/
yKINE__move_free   (tMOVE *a_move)
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  clears and destroys a single move entry as well as removing it from the
    *  master move doubly-linked list.
    */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_point   ("a_move"    , a_move);
   /*---(defenses)-----------------------*/
   if (a_move      == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not free a null move");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(remove from dependency list)----*/
   DEBUG_YKINE_SCRP   yLOG_note    ("remove from backwards/prev direction");
   if (a_move->m_next != NULL) a_move->m_next->m_prev = a_move->m_prev;
   else                        m_tail                 = a_move->m_prev;
   DEBUG_YKINE_SCRP   yLOG_note    ("remove from forewards/next direction");
   if (a_move->m_prev != NULL) a_move->m_prev->m_next = a_move->m_next;
   else                        m_head                 = a_move->m_next;
   DEBUG_YKINE_SCRP   yLOG_note    ("decrement count");
   --m_count;
   /*---(free)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("free move object");
   free (a_move);
   a_move = NULL;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return  a_move;
}



/*====================------------------------------------====================*/
/*===----                        move objects                          ----===*/
/*====================------------------------------------====================*/
static void      o___OBJECTS_________________o (void) {;}

char         /*--> create a new move ---------------------[ ------ [ ------ ]-*/
ykine_move_create  (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type      ,   /* type of move (pause, servo, ...)         */
      tSERVO     *a_servo     ,   /* servo                                    */
      char       *a_label     ,   /* step label                               */
      int         a_line      ,   /* source line                              */
      float       a_deg       ,   /* end position                             */
      float       a_sec       )   /* duration                                 */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   tMOVE      *x_move      = NULL;          /* new requires entry             */
   float       d           =  0.0;
   float       x, z, y;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_info    ("a_label"   , a_label);
   DEBUG_YKINE_SCRP   yLOG_char    ("a_type"    , a_type);
   DEBUG_YKINE_SCRP   yLOG_double  ("a_deg"     , a_deg);
   DEBUG_YKINE_SCRP   yLOG_double  ("a_sec"     , a_sec);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_servo"   , a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not add a move to a null servo");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_info    ("name"      , a_servo->label);
   /*---(get previous)----------------*/
   ykine_move_savedloc (a_servo, NULL, NULL, &x, &z, &y, NULL);
   /*---(create a new requires)-------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("add a new move object");
   x_move          = yKINE__move_new ();
   --rce;  if (x_move == NULL)   {
      DEBUG_YKINE_SCRP yLOG_exit      (__FUNCTION__);
      return rce; 
   }
   /*---(assign basics)---------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("assign basic values");
   if      (a_type == YKINE_MOVE_NOTE)  x_move->type     = a_type;
   else if (a_sec  == 0.0            )  x_move->type     = YKINE_MOVE_INIT;
   else                                 x_move->type     = a_type;
   x_move->servo    = a_servo;
   strlcpy (x_move->label, a_label, LEN_LABEL);
   x_move->line     = a_line;
   x_move->degs  = a_deg;
   x_move->dur  = a_sec;
   /*---(hook it up to servo)---------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("hook up move into servo doubly-linked list");
   if (a_servo->tail == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("first entry in list");
      a_servo->head         = x_move;
      a_servo->tail         = x_move;
      a_servo->count        = 1;
      DEBUG_YKINE_SCRP   yLOG_note    ("update sec/deg based on being first move");
      x_move->seq           = 1;
      x_move->secs       = a_sec;
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("add to tail");
      x_move->s_prev        = a_servo->tail;
      a_servo->tail->s_next = x_move;
      a_servo->tail         = x_move;
      ++(a_servo->count);
      DEBUG_YKINE_SCRP   yLOG_note    ("update sec/deg based on previous move");
      x_move->seq           = x_move->s_prev->seq + 1;
      d                     = x_move->s_prev->secs;
      x_move->secs          = d + a_sec;
   }
   /*---(carry last values)--------------*/
   ykine_move_addloc (a_servo, x, z, y);
   /*---(display stats)------------------*/
   DEBUG_YKINE_SCRP   yLOG_value   ("count"     , a_servo->count);
   DEBUG_YKINE_SCRP   yLOG_value   ("seq"       , x_move->seq);
   DEBUG_YKINE_SCRP   yLOG_double  ("secs"   , x_move->secs);
   /*---(update globals)-----------------*/
   DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , myKINE.scrp_len);
   if (x_move->secs > myKINE.scrp_len) {
      myKINE.scrp_len = x_move->secs;
      DEBUG_YKINE_SCRP   yLOG_note    ("end time greater than current length");
      DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , myKINE.scrp_len);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> add a location to a move object -------[ ------ [ ------ ]-*/
ykine_move_addloc        (tSERVO *a_servo, float a_xpos, float a_zpos, float a_ypos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_sdouble (a_xpos);
   DEBUG_YKINE_SCRP   yLOG_sdouble (a_zpos);
   DEBUG_YKINE_SCRP   yLOG_sdouble (a_ypos);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_snote   (a_servo->label);
   /*---(assign basics)---------------*/
   a_servo->tail->x_pos    = a_xpos;
   a_servo->tail->z_pos    = a_zpos;
   a_servo->tail->y_pos    = a_ypos;
   a_servo->tail->xz_len   = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   DEBUG_YKINE_SCRP   yLOG_sdouble (a_servo->tail->xz_len);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_move__repeatnote (tSERVO *a_servo, int a_nline, int a_count, int a_segno)
{
   /*---(locals)-----------+-----------+-*/
   char        x_label     [LEN_LABEL];
   /*---(labels)-------------------------*/
   if (a_count >= 0) {
      sprintf (x_label, "repeat %d,%d", a_segno, a_count + 1);
      DEBUG_YKINE_SCRP   yLOG_info    ("RIPETERE"  , x_label);
   } else {
      sprintf (x_label, "taeper %d", a_segno);
      DEBUG_YKINE_SCRP   yLOG_note    ("ERETEPIR to be created");
   }
   /*---(moves)--------------------------*/
   ykine_move_create (YKINE_MOVE_NOTE, a_servo, x_label, a_nline, 0.0, 0.0);
   if (a_servo->seg == YKINE_TIBI) {
      ykine_move_create (YKINE_MOVE_NOTE, a_servo - 1, x_label, a_nline, 0.0, 0.0);
      ykine_move_create (YKINE_MOVE_NOTE, a_servo - 2, x_label, a_nline, 0.0, 0.0);
   }
   else if (a_servo->seg == YKINE_FEMU) {
      ykine_move_create (YKINE_MOVE_NOTE, a_servo + 1, x_label, a_nline, 0.0, 0.0);
      ykine_move_create (YKINE_MOVE_NOTE, a_servo + 2, x_label, a_nline, 0.0, 0.0);
   }
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
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_point   ("a_servo"   , a_servo);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_times"   , a_times);
   /*---(defense)------------------------*/
   --rce;  if (a_servo        == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("not a valid servo");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_info    ("servo"     , a_servo->label);
   --rce;  if (a_servo->tail  == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("no moves on servo, nothing to repeat");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("nsegno"    , a_servo->nsegno);
   --rce;  if (a_servo->nsegno <= 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("no segni in servo stack");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(adjust segno count)-------------*/
   --(a_servo->nsegno);
   /*---(save ripetere line)-------------*/
   x_line = myKINE.s_nline;
   /*---(find end)-----------------------*/
   x_end  = a_servo->tail;
   DEBUG_YKINE_SCRP   yLOG_complex ("x_end"     , "%3d, %3d, %s", x_end->seq, x_end->line, x_end->label);
   /*---(find segno)---------------------*/
   x_seg  = a_servo->segni [a_servo->nsegno];
   --rce;  if (x_seg == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("bad segni at current position in stack");
      ykine_move_delete (x_end);
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_complex ("segno"     , "%3d, %3d, %s", x_seg->seq, x_seg->line, x_seg->label);
   --rce;  if (x_seg == x_end) {
      DEBUG_YKINE_SCRP   yLOG_note    ("empty loop on servo, nothing to do");
      a_servo->segni [a_servo->nsegno] = NULL;
      ykine_move_delete (x_seg);
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find beg)-----------------------*/
   x_beg = x_seg->s_next;
   DEBUG_YKINE_SCRP   yLOG_complex ("x_beg"     , "%3d, %3d, %s", x_beg->seq, x_beg->line, x_beg->label);
   /*---(add)----------------------------*/
   for (i = 0; i < a_times; ++i) {
      ykine_move__repeatnote (a_servo, x_line, i, x_seg->seq);
      a_servo->tail->other = x_seg->seq;
      x_curr = x_beg;
      while (x_curr != NULL) {
         DEBUG_YKINE_SCRP   yLOG_value   ("line"      , x_curr->line);
         if (x_curr->type != YKINE_MOVE_NOTE) {
            strlcpy (x_label, a_servo->label, 3);
            DEBUG_YKINE_SCRP   yLOG_info    ("x_label"   , x_label);
            rc = yPARSE_reload (&(myKINE.s_nline), &(myKINE.s_cline), x_curr->line, x_label);
            DEBUG_YKINE_SCRP   yLOG_value   ("reload"    , rc);
            if (rc == 1)  rc = ykine_scrp_exec    ();
            DEBUG_YKINE_SCRP   yLOG_value   ("exec"      , rc);
         }
         if (x_curr == x_end)  break;
         x_curr = x_curr->s_next;
      }
   }
   ykine_move__repeatnote (a_servo, x_line, -1, x_seg->seq);
   a_servo->tail->other = x_seg->seq;
   /*---(clear)--------------------------*/
   a_servo->segni [a_servo->nsegno] = NULL;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> remove a move -------------------------[ ------ [ ------ ]-*/
ykine_move_delete  (tMOVE *a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_move"    , a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update timings downstream)------*/
   x_next = a_move->s_next;
   DEBUG_YKINE_SCRP   yLOG_point   ("x_next"    , x_next);
   while (x_next != NULL) {
      DEBUG_YKINE_SCRP   yLOG_value   ("sec less"  , a_move->dur);
      x_next->secs -= a_move->dur;
      x_next = x_next->s_next;
      DEBUG_YKINE_SCRP   yLOG_point   ("x_next"    , x_next);
   }
   /*---(mark deleted)-------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("wipe move header values");
   a_move->type     = YKINE_MOVE_DEL;
   a_move->secs  = 0.0;
   a_move->dur  = 0.0;
   if (a_move->s_prev != NULL) {
      a_move->degs  = a_move->s_prev->degs;
   }
   /*---(remove from servo list)---------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("remove from backwards/prev direction");
   if (a_move->s_next != NULL) a_move->s_next->s_prev = a_move->s_prev;
   else                        a_move->servo->tail    = a_move->s_prev;
   DEBUG_YKINE_SCRP   yLOG_note    ("remove from forewards/next direction");
   if (a_move->s_prev != NULL) a_move->s_prev->s_next = a_move->s_next;
   else                        a_move->servo->head    = a_move->s_next;
   DEBUG_YKINE_SCRP   yLOG_note    ("decrement count");
   --a_move->servo->count;
   /*---(free memory)--------------------*/
   rc = yKINE__move_free (a_move);
   DEBUG_YKINE_SCRP   yLOG_value   ("free"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
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
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_servo"   , a_servo);
   --rce;  if (a_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_complex ("label"     , "%d/%d %s", a_servo->leg, a_servo->seg, a_servo->label);
   /*---(prepare)------------------------*/
   x_curr = a_servo->tail;
   DEBUG_YKINE_SCRP   yLOG_point   ("head"      , x_curr);
   while (x_curr != NULL) {
      x_prev = x_curr->s_prev;
      rc = ykine_move_delete (x_curr);
      DEBUG_YKINE_SCRP   yLOG_value   ("delete"    , rc);
      if (rc < 0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      x_curr = x_prev;
      DEBUG_YKINE_SCRP   yLOG_point   ("prev"      , x_curr);
   }
   /*---(compete)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_move_savedloc     (tSERVO *a_servo, float *a_sec, float *a_deg, float *x, float *z, float *y, float *xz)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMOVE      *x_tail      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_servo);
   --rce;  if (a_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(previous)-----------------------*/
   x_tail = a_servo->tail;
   DEBUG_YKINE_SCRP   yLOG_spoint  (x_tail);
   /*---(if none)------------------------*/
   if (x_tail == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("none, so returning zeros");
      if (a_sec != NULL)  *a_sec = 0.0;
      if (a_deg != NULL)  *a_deg = 0.0;
      if (x     != NULL)  *x     = 0.0;
      if (z     != NULL)  *z     = 0.0;
      if (y     != NULL)  *y     = 0.0;
      if (xz    != NULL)  *xz    = 0.0;
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      DEBUG_YKINE_SCRP   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   }
   /*---(exxists)------------------------*/
   else {
      DEBUG_YKINE_SCRP   yLOG_sint    (x_tail->line);
      DEBUG_YKINE_SCRP   yLOG_snote   (x_tail->label);
      if (a_sec != NULL)  *a_sec = x_tail->secs;
      if (a_deg != NULL)  *a_deg = x_tail->degs;
      if (x     != NULL)  *x     = x_tail->x_pos;
      if (z     != NULL)  *z     = x_tail->z_pos;
      if (y     != NULL)  *y     = x_tail->y_pos;
      if (xz    != NULL)  *xz    = x_tail->xz_len;
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      DEBUG_YKINE_SCRP   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", x_tail->secs, x_tail->degs, x_tail->x_pos, x_tail->z_pos, x_tail->y_pos, x_tail->xz_len);
   }
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
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_move);
   --rce;  if (a_move == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(previous)-----------------------*/
   if (a_move->s_prev != NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("prev good");
      x_prev = a_move->s_prev;
      if (a_sec != NULL)  *a_sec = x_prev->secs;
   } else  {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no prev, use move");
      x_prev = a_move;
      if (a_sec != NULL)  *a_sec = 0.0;
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (x_prev);
   /*---(return data)--------------------*/
   DEBUG_YKINE_SCRP   yLOG_sint    (x_prev->line);
   DEBUG_YKINE_SCRP   yLOG_snote   (x_prev->label);
   if (a_deg != NULL)  *a_deg = x_prev->degs;
   if (x     != NULL)  *x     = x_prev->x_pos;
   if (z     != NULL)  *z     = x_prev->z_pos;
   if (y     != NULL)  *y     = x_prev->y_pos;
   if (xz    != NULL)  *xz    = x_prev->xz_len;
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("saves"     , "%8.2fs, %8.2fd, %8.2fx, %8.2fz, %8.2fy, %8.2fxz", x_prev->secs, x_prev->degs, x_prev->x_pos, x_prev->z_pos, x_prev->y_pos, x_prev->xz_len);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      current position                        ----===*/
/*====================------------------------------------====================*/
static void      o___CURRENT_________________o (void) {;}

char         /*--> identify the current move -------------[ ------ [ ------ ]-*/
ykine__exact_check       (tMOVE *a_curr, float a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_beg       =    0;
   int         x_cur       =    0;
   int         x_end       =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   if (a_curr == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("null move");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, -2);
      return -2;
   }
   /*---(prepare)------------------------*/
   x_cur = round (a_sec * 1000.0);
   if (a_curr->s_prev != NULL)   x_beg = round (a_curr->s_prev->secs * 1000.0);
   else                          x_beg = 0.0;
   x_end = round (a_curr->secs * 1000.0);
   DEBUG_YKINE_SCRP   yLOG_sint    (a_curr->seq);
   /*---(first move)---------------------*/
   if (x_cur == x_beg && x_beg == 0.00) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("implied start");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return  2;
   }
   /*---(start time)---------------------*/
   if (x_cur == x_end && x_end == 0.00) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("init/start");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return  0;
   }
   /*---(check not yet)------------------*/
   if (x_cur >  x_end) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("not yet");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return  1;
   }
   /*---(check missed)-------------------*/
   if (x_cur <= x_beg) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("too far");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -1;
   }
   /*---(exactly)------------------------*/
   if (x_cur == x_end) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("exactly");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return  2;
   }
   /*---(found it)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("in range");
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> identify the current move -------------[ ------ [ ------ ]-*/
ykine__exact_find       (tSERVO *a_servo, float a_sec)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMOVE      *x_next      = NULL;
   int         x_seq       =   -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   a_servo->exact = '-';
   a_servo->deg   = 0.0;
   a_servo->xexp  = 0.0;
   a_servo->zexp  = 0.0;
   a_servo->yexp  = 0.0;
   if (a_servo->curr == NULL)  x_next = a_servo->head;
   else                        x_next = a_servo->curr;
   /*---(walk thru moves)----------------*/
   while (x_next != NULL) {
      rc = ykine__exact_check (x_next, a_sec);
      if (rc == 0 || rc == 2) {
         DEBUG_YKINE_SCRP   yLOG_note    ("success, save values");
         a_servo->curr  = x_next;
         if (rc == 2)  a_servo->exact = 'y';
         else          a_servo->exact = 'n';
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      if      (rc ==  1)   x_next = x_next->s_next;
      else if (rc == -1)   x_next = x_next->s_prev;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("failed, current move never found");
   DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, --rce);
   return rce;
}

#define     YKINE_LINEAR    'l'
#define     YKINE_POLAR     'p'

float
ykine__exact_calc        (float a_beg, float a_end, float a_pct)
{
   double      x_range     = 0.0;
   double      x_inc       = 0.0;
   double      x_pos       = 0.0;
   if (a_beg <= a_end) {
      x_range = a_end - a_beg;
      x_inc   = x_range * a_pct;
      x_pos   = a_beg + x_inc;
   } else {
      x_range = a_beg - a_end;
      x_inc   = x_range * a_pct;
      x_pos   = a_beg - x_inc;
   }
   return x_pos;
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
   float       dp, dc, d , lp, lc, l;
   float       xp, xc, zp, zc, yp, yc;
   float       sp, sc;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(verify current)-----------------*/
   rc = ykine__exact_find  (a_servo, a_sec);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("no valid move for time requested");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_curr  = a_servo->curr;
   DEBUG_YKINE_SCRP   yLOG_complex ("verb"      , "%s at %d", x_curr->label, x_curr->line);
   /*---(get current values)-------------*/
   xc  = x_curr->x_pos;
   zc  = x_curr->z_pos;
   yc  = x_curr->y_pos;
   sc  = x_curr->secs;
   dc  = x_curr->degs;
   /*---(get previous values)---------*/
   rc = ykine_move_savedprev  (x_curr, &sp, &dp, &xp, &zp, &yp, NULL);
   /*---(calc percent)-------------------*/
   x_range = sc - sp;
   if (x_range == 0.0)  x_pct   = 0.0;
   else                 x_pct   = (a_sec - sp) / x_range;
   DEBUG_YKINE_SCRP   yLOG_complex ("percent"   , "%8.2fb, %8.2fe, %8.2fr, %8.2fp", sp, sc, x_range, x_pct);
   if (strncmp (x_curr->label, "zp_", 3) == 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("polar verb");
      lp = sqrt ((xp * xp) + (zp * zp));
      lc = sqrt ((xc * xc) + (zc * zc));
      l  = ykine__exact_calc (lp, lc, x_pct);
      DEBUG_YKINE_SCRP   yLOG_complex ("dist"      , "%8.2fb, %8.2fe, %8.2fc", lp, lc, l);
      if (dp > dc) {
         dc = 360.0 + dc;
      }
      d  = ykine__exact_calc (dp, dc, x_pct);
      if (d <    0.0)   d  = 360.0 + d;
      if (d >  360.0)   d  = d - 360.0;
      a_servo->deg  = d;
      a_servo->xexp =  l * sin (d * DEG2RAD);
      a_servo->zexp = -l * cos (d * DEG2RAD);
      a_servo->yexp = ykine__exact_calc (yp, yc, x_pct);
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("normal verb");
      d  = ykine__exact_calc (dp, dc, x_pct);
      a_servo->deg  = d;
      a_servo->xexp = ykine__exact_calc (xp, xc, x_pct);
      a_servo->zexp = ykine__exact_calc (zp, zc, x_pct);
      a_servo->yexp = ykine__exact_calc (yp, yc, x_pct);
   }
   DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.2fb, %8.2fe, %8.2fc", dp, dc, d);
   DEBUG_YKINE_SCRP   yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", a_servo->xexp, a_servo->zexp, a_servo->yexp);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> calc current move/deg for all servos --[ ------ [ ------ ]-*/
yKINE_exact_all          (float a_sec)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         i           =    0;
   tSERVO     *x_servo     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   for (i = 0; i < g_nservo; ++i) {
      x_servo   = &(g_servo_info [i]);
      DEBUG_YKINE_SCRP   yLOG_point   ("x_servo"   , x_servo);
      if (x_servo == NULL)   continue;
      rc = ykine__exact_data  (x_servo, a_sec);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> calc the current deg for a servo ------[ ------ [ ------ ]-*/
yKINE_exact              (int a_leg, int a_seg, float *a_deg, float *a_x, float *a_z, float *a_y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;          /* return code for errors         */
   char        rc          =    0;
   int         i           =    0;
   tSERVO     *x_servo     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_deg != NULL)  *a_deg = 0.0;
   if (a_x   != NULL)  *a_x   = 0.0;
   if (a_z   != NULL)  *a_z   = 0.0;
   if (a_y   != NULL)  *a_y   = 0.0;
   /*---(get servo)----------------------*/
   x_servo   = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP   yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(return data)--------------------*/
   if (a_deg != NULL)  *a_deg = x_servo->deg;
   if (a_x   != NULL)  *a_x   = x_servo->xexp;
   if (a_z   != NULL)  *a_z   = x_servo->zexp;
   if (a_y   != NULL)  *a_y   = x_servo->yexp;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     sequential search                        ----===*/
/*====================------------------------------------====================*/
static void      o___SEQUENCIAL______________o (void) {;}

static tSERVO  *s_servo = NULL;
static tMOVE   *s_curr  = NULL;

char
ykine__servo_next       (char a_dir, char a_type, float *a, float *b, float *c, float *d)
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
   /*---(update current)-----------------*/
   DEBUG_YKINE_SCRP   yLOG_schar   (a_dir);
   --rce;  switch (a_dir) {
   case 'H' : s_curr = s_servo->head;  a_dir = 'n';          break;
   case 'n' : if (s_curr != NULL)  s_curr = s_curr->s_next;  break;
   case 'p' : if (s_curr != NULL)  s_curr = s_curr->s_prev;  break;
   case 'T' : s_curr = s_servo->tail;  a_dir = 'p';          break;
   default  : /*---(problem)-------------*/
              DEBUG_YKINE_SCRP   yLOG_snote   ("invalid dir");
              return rce;
              break;
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_curr);
   /*---(check for next)-----------------*/
   while (s_curr != NULL) {
      DEBUG_YKINE_SCRP   yLOG_schar   (s_curr->type);
      if (s_curr->type == YKINE_MOVE_INIT )  break;
      if (s_curr->type == YKINE_MOVE_SERVO)  break;
      if (s_curr->type == YKINE_MOVE_WAIT )  break;
      switch (a_dir) {
      case 'n' : s_curr = s_curr->s_next;  break;
      case 'p' : s_curr = s_curr->s_prev;  break;
      }
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_curr);
   /*---(refuse null)--------------------*/
   --rce;  if (s_curr == NULL) {
      s_curr  = NULL;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no more moves");
      return rce;
   }
   /*---(save position)------------------*/
   DEBUG_YKINE_SCRP   yLOG_svalue  ("found move", s_curr->seq);
   /*---(return values)------------------*/
   DEBUG_YKINE_SCRP   yLOG_schar   (a_type);
   --rce;  switch (a_type) {
   case 'd' :
      if (a  != NULL)  *a  = s_curr->secs;
      if (b  != NULL)  *b  = s_curr->degs;
      break;
   case 'c' :
      if (a  != NULL)  *a  = s_curr->secs;
      if (b  != NULL)  *b  = s_curr->x_pos;
      if (c  != NULL)  *c  = s_curr->z_pos;
      if (d  != NULL)  *d  = s_curr->y_pos;
      break;
   default  :
      DEBUG_YKINE_SCRP   yLOG_snote   ("invalid type");
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_zero_first        (float *a_sec, float *a_x, float *a_z, float *a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -100;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(find servo)---------------------*/
   s_servo   = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_servo);
   --rce;  if (s_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   rc = ykine__servo_next ('H', 'c', a_sec, a_x, a_z, a_y);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the next move ----------------[ ------ [ ------ ]-*/
yKINE_zero_next          (float *a_sec, float *a_x, float *a_z, float *a_y)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(process)------------------------*/
   rc = ykine__servo_next ('n', 'c', a_sec, a_x, a_z, a_y);
   DEBUG_YKINE_SCRP   yLOG_sint    (rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_first         (int a_leg, int a_seg, float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -100;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(find servo)---------------------*/
   s_servo   = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_servo);
   --rce;  if (s_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   rc = ykine__servo_next ('H', 'd', a_sec, a_deg, NULL, NULL);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the next move ----------------[ ------ [ ------ ]-*/
yKINE_move_next          (float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(process)------------------------*/
   rc = ykine__servo_next ('n', 'd', a_sec, a_deg, NULL, NULL);
   DEBUG_YKINE_SCRP   yLOG_sint    (rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the prev move ----------------[ ------ [ ------ ]-*/
yKINE_move_prev          (float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(process)------------------------*/
   rc = ykine__servo_next ('p', 'd', a_sec, a_deg, NULL, NULL);
   DEBUG_YKINE_SCRP   yLOG_sint    (rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_last          (int a_leg, int a_seg, float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -100;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(find servo)---------------------*/
   s_servo   = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_servo);
   --rce;  if (s_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   rc = ykine__servo_next ('T', 'd', a_sec, a_deg, NULL, NULL);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_last_servo    (int a_servo, float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -100;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(find servo)---------------------*/
   s_servo   = ykine_servo_pointer (g_servo_info [a_servo].leg, g_servo_info [a_servo].seg);
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_servo);
   --rce;  if (s_servo == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   rc = ykine__servo_next ('T', 'd', a_sec, a_deg, NULL, NULL);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve data about current -----------[ ------ [ ------ ]-*/
yKINE_move_curdata       (double *a_x, double *a_z, double *a_y)
{
   if (s_curr == NULL)   return -1;
   if (a_x != NULL)  *a_x = s_curr->x_pos;
   if (a_z != NULL)  *a_z = s_curr->z_pos;
   if (a_y != NULL)  *a_y = s_curr->y_pos;
   return 0;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_servo_deg          (int a_leg, int a_seg, float *a_deg)
{
   tSERVO     *x_servo     = NULL;
   double      x_deg       = 0.0;
   /*> if (a_leg < YKINE_BODY || a_leg > YKINE_LR  )  return -1;                      <* 
    *> if (a_seg < YKINE_FEMU || a_seg > YKINE_TIBI)  return -2;                      <*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   if (x_servo->curr == NULL) {
      if (a_seg == YKINE_FEMU)  x_deg  =  0.0;
      if (a_seg == YKINE_PATE)  x_deg  =  0.0;
      if (a_seg == YKINE_TIBI)  x_deg  =  0.0;
      if (a_deg       != NULL)  *a_deg = x_deg;
      return -1;
   }
   if (a_deg       != NULL)  *a_deg = x_servo->deg;
   if (x_servo->exact == 'y')  return 1;
   return 0;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_zero_pos           (float *a_x, float *a_z, float *a_y)
{
   tSERVO     *x_servo     = NULL;
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   if (x_servo == NULL || x_servo->curr == NULL) {
      if (a_x != NULL)  *a_x   = 0.0;
      if (a_z != NULL)  *a_z   = 0.0;
      if (a_y != NULL)  *a_y   = 0.0;
      return -1;
   }
   if (a_x != NULL)  *a_x   = x_servo->xexp;
   if (a_z != NULL)  *a_z   = x_servo->zexp;
   if (a_y != NULL)  *a_y   = x_servo->yexp;
   if (x_servo->exact == 'y')  return 1;
   return 0;
}

char         /*--> get the current details for servo -----[ ------ [ ------ ]-*/
yKINE_servo_move         (int a_leg, int a_seg, char *a_label, float *a_secb, float *a_sece, float *a_dur , float *a_degb, float *a_dege, int *a_seq , int *a_line)
{
   tSERVO     *x_servo     = NULL;
   double      x_deg       = 0.0;
   float       dp, sp;
   if (a_leg < YKINE_RR   || a_leg > YKINE_LR  )  return -1;
   if (a_seg < YKINE_FEMU || a_seg > YKINE_TIBI)  return -2;
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   if (x_servo == NULL)  return -1;
   ykine_move_savedprev  (x_servo->curr, &sp, &dp, NULL, NULL, NULL, NULL);
   if (a_label     != NULL)  strlcpy (a_label, x_servo->curr->label, LEN_LABEL);
   if (a_secb      != NULL)  *a_secb = sp;
   if (a_sece      != NULL)  *a_sece = x_servo->curr->secs;
   if (a_dur       != NULL)  *a_dur  = x_servo->curr->dur;
   if (a_degb      != NULL)  *a_degb = dp;
   if (a_dege      != NULL)  *a_dege = x_servo->curr->degs;
   if (a_seq       != NULL)  *a_seq  = x_servo->curr->seq;
   if (a_line      != NULL)  *a_line = x_servo->curr->line;
   if (x_servo->exact == 'y')  return 1;
   return 0;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_servo_line         (int a_leg, int a_seg, float *a_x1, float *a_z1, float *a_y1, float *a_x2, float *a_z2, float *a_y2)
{
   char        rce         =  -10;
   tSERVO     *x_servo     = 0;
   double      x_deg       = 0.0;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_prev      = NULL;
   x_servo   = ykine_servo_pointer (a_leg, a_seg);
   --rce;  if (x_servo == NULL)    return rce;
   x_curr = x_servo->curr;
   --rce;  if (x_curr  == NULL)    return rce;
   x_prev = x_servo->curr->s_prev;
   while  (x_prev != NULL) {
      if (x_prev->type == YKINE_MOVE_INIT )  break;
      if (x_prev->type == YKINE_MOVE_SERVO)  break;
      if (x_prev->type == YKINE_MOVE_WAIT )  break;
      x_prev = x_prev->s_prev;
   }
   if (x_prev        == NULL)      return -4;
   if (a_x1 != NULL)  *a_x1 = x_prev->x_pos;
   if (a_z1 != NULL)  *a_z1 = x_prev->z_pos;
   if (a_y1 != NULL)  *a_y1 = x_prev->y_pos;
   if (a_x2 != NULL)  *a_x2 = x_curr->x_pos;
   if (a_z2 != NULL)  *a_z2 = x_curr->z_pos;
   if (a_y2 != NULL)  *a_y2 = x_curr->y_pos;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       reporting                              ----===*/
/*====================------------------------------------====================*/
static void      o___REPORTS_________________o (void) {;}

char
yKINE_moves_rpt    (void)
{
   int         i           =    0;
   int         j           =    0;
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_move      = NULL;
   int         x_count     =    0;
   int         x_level     =    0;
   int         x_prefix    [LEN_LABEL];
   int         x_verb      [LEN_LABEL];
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
         if (x_count % 45 == 0)  printf ("\n   seq-  t  ---label-------  line  --dur---  --secs--  --degs--  --xpos--  --zpos--  --ypos--  ---xz---\n");
         if (x_count %  5 == 0)  printf ("\n");
         /*---(indents)------------------*/
         strlcpy (x_prefix, "", LEN_LABEL);
         if (strcmp (x_move->label, "segno") == 0) {
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "  ", LEN_LABEL);
            ++x_level;
         } else if (strncmp (x_move->label, "ripetere ", 9) == 0) {
            --x_level;
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "  ", LEN_LABEL);
            ++x_level;
         } else if (strcmp (x_move->label, "eretepir") == 0) {
            --x_level;
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "  ", LEN_LABEL);
         } else {
            for (j = 0; j < x_level; ++j)   strlcat (x_prefix, "  ", LEN_LABEL);
         }
         sprintf (x_verb, "%s%s", x_prefix, x_move->label);
         /*---(keys)---------------------*/
         printf ("   %4d  %c  %-15.15s  %4d",
               x_move->seq    , x_move->type   , x_verb  , x_move->line);
         /*---(timing)-------------------*/
         if (x_move->dur == 0.0)       printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->dur);
         if (x_move->secs == 0.0)      printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->secs);
         /*---(deg)----------------------*/
         if (x_move->degs    == 0.0)   printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->degs);
         /*---(x,z,y)--------------------*/
         if (x_move->x_pos   == 0.0)   printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->x_pos);
         if (x_move->z_pos   == 0.0)   printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->z_pos);
         if (x_move->y_pos   == 0.0)   printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->y_pos);
         if (x_move->xz_len  == 0.0)   printf ("      -.--");
         else                          printf ("  %8.2lf"  , x_move->xz_len);
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
   int         x_pos       =    0;
   char        x_msg       [LEN_STR  ];
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_move      = NULL;
   tMOVE      *x_next      = NULL;
   int         c           =   -1;
   float       dp, sp;
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "MOVE unit      : question not understood", LEN_STR);
   /*---(supporting data)----------------*/
   if (a_leg >= 0) {
      /*---(get servo)-------------------*/
      x_servo = ykine_servo_pointer (a_leg, a_seg);
      if (x_servo == NULL) {
         sprintf  (ykine__unit_answer, "MOVE unit      : could not locate leg %d/seq %d", a_leg, a_seg);
         return ykine__unit_answer;
      }
      /*---(get move)--------------------*/
      x_move = x_servo->head;
      if (x_move  == NULL) {
         sprintf  (ykine__unit_answer, "MOVE unit      : no moves for this servo");
         return ykine__unit_answer;
      }
      for (i = 0; i < a_cnt; ++i) {
         if (x_move == NULL)  break;
         x_move = x_move->s_next;
      }
      if (x_move == NULL) {
         sprintf  (ykine__unit_answer, "MOVE unit      : past end of moves");
         return ykine__unit_answer;
      }
      /*---(done)------------------------*/
   }
   /*---(answer)-------------------------*/
   if (strcmp (a_question, "header"  ) == 0) {
      ykine_move_savedprev  (x_move, &sp, &dp, NULL, NULL, NULL, NULL);
      sprintf (ykine__unit_answer, "MOVE header    : %2d/%2d %c %-11.11s %8.3lfs %8.3lfe %8.3lfd", x_move->seq, x_servo->count, x_move->type, x_move->label, sp, x_move->secs, x_move->dur);
   }
   else if (strcmp (a_question, "detail"  ) == 0) {
      sprintf (ykine__unit_answer, "MOVE detail    : %8.1lfd %8.1lfx %8.1lfz %8.1lfy %8do", x_move->degs, x_move->x_pos, x_move->z_pos, x_move->y_pos, x_move->other);
   }
   else if (strcmp (a_question, "counts"  ) == 0) {
      strlcpy (ykine__unit_answer, "MOVE counts    : ", LEN_STR);
      for (i = 0; i < g_nservo; ++i) {
         if (i > 0 && i % 3 == 0)  strlcat (ykine__unit_answer, " ", LEN_STR);
         sprintf (x_msg, "%1d", g_servo_info [i].count);
         strlcat (ykine__unit_answer, x_msg, LEN_STR);
      }
   }
   else if (strcmp (a_question, "current" ) == 0) {
      ykine_move_savedprev  (s_curr, &sp, &dp, NULL, NULL, NULL, NULL);
      if (s_curr == NULL)  sprintf (ykine__unit_answer, "MOVE current   : no current move");
      else                 sprintf (ykine__unit_answer, "MOVE current   : %2d/%2d %c %-10.10s %8.3lf %8.3lf %8.3lf", s_curr->seq, s_servo->count, s_curr->type, s_curr->label, sp, s_curr->secs, s_curr->dur);
   }
   else if (strcmp (a_question, "exact"   ) == 0) {
      sprintf (ykine__unit_answer, "MOVE exact     : %c %8.1lfd, %8.1lfx, %8.1lfz, %8.1lfy", x_servo->exact, x_servo->deg, x_servo->xexp, x_servo->zexp, x_servo->yexp);
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
