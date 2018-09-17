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
   DEBUG_YKINE_SCRP   yLOG_value   ("x_new"     , x_new);
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
   x_new->deg_beg =  0.0f;
   x_new->deg_end =  0.0f;
   /*---(timing)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default timings");
   x_new->sec_dur =  0.0f;
   x_new->sec_beg =  0.0f;
   x_new->sec_end =  0.0f;
   /*---(location)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default locations");
   x_new->x_pos   =  0.0f;
   x_new->y_pos   =  0.0f;
   x_new->z_pos   =  0.0f;
   /*---(status)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("default locations");
   x_new->status  =   -1;
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
yKINE_move_create  (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type      ,   /* type of move (pause, servo, ...)         */
      tSERVO     *a_servo     ,   /* servo                                    */
      char       *a_label     ,   /* step label                               */
      int         a_line      ,   /* source line                              */
      double      a_deg       ,   /* end position                             */
      double      a_sec       )   /* duration                                 */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   tMOVE      *x_move      = NULL;          /* new requires entry             */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
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
   x_move->deg_end  = a_deg;
   x_move->sec_dur  = a_sec;
   /*---(hook it up to servo)---------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("hook up move into servo doubly-linked list");
   if (a_servo->tail == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("first entry in list");
      a_servo->head         = x_move;
      a_servo->tail         = x_move;
      a_servo->count        = 1;
      DEBUG_YKINE_SCRP   yLOG_note    ("update sec/deg based on being first move");
      x_move->seq           = 1;
      x_move->sec_beg       = 0.0f;
      x_move->sec_end       = a_sec;
      x_move->deg_beg       = a_deg;
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("add to tail");
      x_move->s_prev        = a_servo->tail;
      a_servo->tail->s_next = x_move;
      a_servo->tail         = x_move;
      ++(a_servo->count);
      DEBUG_YKINE_SCRP   yLOG_note    ("update sec/deg based on previous move");
      x_move->seq           = x_move->s_prev->seq + 1;
      x_move->sec_beg       = x_move->s_prev->sec_end;
      x_move->sec_end       = x_move->sec_beg + a_sec;
      x_move->deg_beg       = x_move->s_prev->deg_end;
   }
   if (a_type == YKINE_MOVE_NOTE)  x_move->deg_end  = x_move->deg_beg;
   /*---(display stats)------------------*/
   DEBUG_YKINE_SCRP   yLOG_value   ("count"     , a_servo->count);
   DEBUG_YKINE_SCRP   yLOG_value   ("seq"       , x_move->seq);
   DEBUG_YKINE_SCRP   yLOG_double  ("sec_beg"   , x_move->sec_beg);
   DEBUG_YKINE_SCRP   yLOG_double  ("sec_end"   , x_move->sec_end);
   DEBUG_YKINE_SCRP   yLOG_double  ("deg_beg"   , x_move->deg_beg);
   /*---(update globals)-----------------*/
   DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , myKINE.scrp_len);
   if (x_move->sec_end > myKINE.scrp_len) {
      myKINE.scrp_len = x_move->sec_end;
      DEBUG_YKINE_SCRP   yLOG_note    ("end time greater than current length");
      DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , myKINE.scrp_len);
   }
   /*---(check segno)--------------------*/
   if (a_servo->segno_flag == 'y') {
      if (a_servo->segno == NULL) {
         a_servo->segno = x_move;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> add a location to a move object -------[ ------ [ ------ ]-*/
yKINE_move_addloc        (
      /*----------+-----------+-----------------------------------------------*/
      tSERVO     *a_servo     ,   /* servo                                    */
      double      a_xpos      ,   /* end location                             */
      double      a_zpos      ,   /* end location                             */
      double      a_ypos      )   /* end location                             */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_xpos"    , a_xpos);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_zpos"    , a_zpos);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_ypos"    , a_ypos);
   /*---(defenses)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_servo"   , a_servo);
   --rce;  if (a_servo     == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not add location to a null servo");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_info    ("name"      , a_servo->label);
   /*---(assign basics)---------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("assign location values");
   a_servo->tail->x_pos    = a_xpos;
   a_servo->tail->z_pos    = a_zpos;
   a_servo->tail->y_pos    = a_ypos;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> repeat the last moves -----------------[ ------ [ ------ ]-*/
yKINE_move_repeat        (
      /*----------+-----------+-----------------------------------------------*/
      tSERVO     *a_servo     ,   /* servo                                    */
      int         a_count     ,   /* how many steps to copy (off end)         */
      int         a_times     )   /* how many times to copy                   */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        x_label     [LEN_LABEL];
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_moves     [100];
   int         i           = 0;
   int         j           = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_point   ("a_servo"   , a_servo);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_count"   , a_count);
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
   DEBUG_YKINE_SCRP   yLOG_value   ("count"     , a_servo->count);
   --rce;  if (a_servo->count <  a_count) {
      DEBUG_YKINE_SCRP   yLOG_note    ("not enough moves on servo to repeat count");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(build queue)--------------------*/
   for (i = 0; i < 100; ++i) {
      x_moves [i] = NULL;
   }
   x_curr = a_servo->tail;
   for (i = 1; i < a_count; ++i) {
      x_curr = x_curr->s_prev;
   }
   for (i = 0; i < a_count; ++i) {
      x_moves [i] = x_curr;
      x_curr      = x_curr->s_next;
   }
   /*---(add)----------------------------*/
   for (i = 0; i < a_times; ++i) {
      for (j = 0; j < a_count; ++j) {
         sprintf (x_label, "repeat %d.%d", i,j);
         yKINE_move_create (YKINE_MOVE_SERVO, a_servo, x_label, x_moves [j]->line, x_moves [j]->deg_end, x_moves [j]->sec_dur);
         yKINE_move_addloc (a_servo, x_moves [j]->x_pos, x_moves [j]->z_pos, x_moves [j]->y_pos);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> repeat from the last segno ------------[ ------ [ ------ ]-*/
yKINE_move_dsegno      (
      /*----------+-----------+-----------------------------------------------*/
      tSERVO     *a_servo     ,   /* servo                                    */
      int         a_times     )   /* how many times to copy                   */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        x_label     [LEN_LABEL];
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_end       = NULL;
   int         i           = 0;
   int         j           = 0;
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
   DEBUG_YKINE_SCRP   yLOG_char    ("segno_flag", a_servo->segno_flag);
   --rce;  if (a_servo->segno_flag != 'y') {
      DEBUG_YKINE_SCRP   yLOG_note    ("a segno is not ready for use");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(add)----------------------------*/
   x_end  = a_servo->tail;
   for (i = 0; i < a_times; ++i) {
      x_curr = a_servo->segno;
      j = 0;
      while (x_curr != NULL) {
         sprintf (x_label, "segno %d.%d", i,j);
         yKINE_move_create (YKINE_MOVE_SERVO, a_servo, x_label, x_curr->line, x_curr->deg_end, x_curr->sec_dur);
         if (x_curr == x_end)  break;
         x_curr = x_curr->s_next;
         ++j;
      }
   }
   /*---(clear)--------------------------*/
   a_servo->segno      = NULL;
   a_servo->segno_flag = '-';
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
      DEBUG_YKINE_SCRP   yLOG_value   ("sec less"  , a_move->sec_dur);
      x_next->sec_beg -= a_move->sec_dur;
      x_next->sec_end -= a_move->sec_dur;
      x_next = x_next->s_next;
      DEBUG_YKINE_SCRP   yLOG_point   ("x_next"    , x_next);
   }
   /*---(update next deg_beg)------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("s_next"    , a_move->s_next);
   DEBUG_YKINE_SCRP   yLOG_point   ("s_prev"    , a_move->s_prev);
   if (a_move->s_prev != NULL && a_move->s_next != NULL) {
      a_move->s_next->deg_beg = a_move->s_prev->deg_end;
   }
   /*---(mark deleted)-------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("wipe move header values");
   a_move->type     = YKINE_MOVE_DEL;
   a_move->sec_beg  = 0.0;
   a_move->sec_end  = 0.0;
   a_move->sec_dur  = 0.0;
   if (a_move->s_prev != NULL) {
      a_move->deg_beg  = a_move->s_prev->deg_end;
      a_move->deg_end  = a_move->s_prev->deg_end;
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



/*====================------------------------------------====================*/
/*===----                      current position                        ----===*/
/*====================------------------------------------====================*/
static void      o___CURRENT_________________o (void) {;}

char         /*--> identify the current move -------------[ ------ [ ------ ]-*/
ykine__exact_check       (tMOVE *a_curr, double a_sec)
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
   x_beg = round (a_curr->sec_beg * 1000.0);
   x_end = round (a_curr->sec_end * 1000.0);
   DEBUG_YKINE_SCRP   yLOG_sint    (a_curr->seq);
   /*---(start time)---------------------*/
   if (x_cur == x_end && x_end == 0.00) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("start");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return  2;
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
ykine__exact_find       (tSERVO *a_servo, double a_sec)
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

double
ykine__exact_calc        (double a_beg, double a_end, double a_pct)
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
   DEBUG_YKINE_SCRP   yLOG_double  ("a_beg"     , a_beg);
   DEBUG_YKINE_SCRP   yLOG_double  ("a_end"     , a_end);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_range"   , x_range);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_inc"     , x_inc);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_pos"     , x_pos);
   return x_pos;
}

char         /*--> calc the current deg for a servo ------[ ------ [ ------ ]-*/
ykine__exact_data        (tSERVO *a_servo, double a_sec)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* generic return code            */
   tMOVE      *x_curr      = NULL;
   double      x_beg       = 0.0;
   double      x_end       = 0.0;
   double      x_pct       = 0.0;
   double      x_range     = 0.0;
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
   /*---(calc percent)-------------------*/
   x_beg   = x_curr->sec_beg;
   x_end   = x_curr->sec_end;
   x_range = x_end - x_beg;
   if (x_range == 0.0)  x_pct   = 0.0;
   else                 x_pct   = (a_sec - x_beg) / x_range;
   DEBUG_YKINE_SCRP   yLOG_double  ("x_beg"     , x_beg);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_end"     , x_end);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_range"   , x_range);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_pct"     , x_pct);
   /*---(calc degrees)-------------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("calculate degrees");
   a_servo->deg = ykine__exact_calc (x_curr->deg_beg, x_curr->deg_end, x_pct);
   /*---(calc position)------------------*/
   if (x_curr->s_prev == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("exact start match, just copy coordinates");
      a_servo->xexp  = x_curr->x_pos;
      a_servo->zexp  = x_curr->z_pos;
      a_servo->yexp  = x_curr->y_pos;
   } else {
      DEBUG_YKINE_SCRP   yLOG_note    ("calculate x-pos");
      a_servo->xexp = ykine__exact_calc (x_curr->s_prev->x_pos, x_curr->x_pos, x_pct);
      /*> x_range = x_curr->x_pos - x_curr->s_prev->x_pos;                            <* 
       *> a_servo->xexp = x_curr->s_prev->x_pos + (x_range * x_pct);                  <*/
      DEBUG_YKINE_SCRP   yLOG_note    ("calculate z-pos");
      a_servo->zexp = ykine__exact_calc (x_curr->s_prev->z_pos, x_curr->z_pos, x_pct);
      /*> x_range = x_curr->z_pos - x_curr->s_prev->z_pos;                            <* 
       *> a_servo->zexp = x_curr->s_prev->z_pos + (x_range * x_pct);                  <*/
      DEBUG_YKINE_SCRP   yLOG_note    ("calculate y-pos");
      a_servo->yexp = ykine__exact_calc (x_curr->s_prev->y_pos, x_curr->y_pos, x_pct);
      /*> x_range = x_curr->y_pos - x_curr->s_prev->y_pos;                            <* 
       *> a_servo->yexp = x_curr->s_prev->y_pos + (x_range * x_pct);                  <*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> calc current move/deg for a leg -------[ ------ [ ------ ]-*/
yKINE_exact_leg          (double a_sec, int a_leg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;          /* return code for errors         */
   char        rc          =    0;
   int         i           =    0;
   tSERVO     *x_servo     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(walk all segs)------------------*/
   for (i = YKINE_FEMU; i <= YKINE_TIBI; ++i) {
      x_servo   = ykine_servo_pointer (a_leg, i);
      DEBUG_YKINE_SCRP   yLOG_point   ("x_servo"   , x_servo);
      if (x_servo == NULL)   continue;
      rc = ykine__exact_data  (x_servo, a_sec);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> calc current move/deg for all servos --[ ------ [ ------ ]-*/
yKINE_exact_all          (double a_sec)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =   0;
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   for (i = 0; i < g_nservo; ++i) {
      rc = yKINE_exact_leg    (i, a_sec);
      if (rc < 0)  continue;
      rc = ykine__exact_data  (i, a_sec);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> calc the current deg for a servo ------[ ------ [ ------ ]-*/
yKINE_exact              (int a_leg, int a_seg, double a_sec, double *a_deg, double *a_x, double *a_z, double *a_y)
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
   /*---(get data)-----------------------*/
   rc = ykine__exact_data  (x_servo, a_sec);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
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
ykine__servo_next       (char a_dir, char a_type, double *a, double *b, double *c, double *d)
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
      if (a  != NULL)  *a  = s_curr->sec_end;
      if (b  != NULL)  *b  = s_curr->deg_end;
      break;
   case 'c' :
      if (a  != NULL)  *a  = s_curr->sec_end;
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
yKINE_zero_first        (double *a_sec, double *a_x, double *a_z, double *a_y)
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
yKINE_zero_next          (double *a_sec, double *a_x, double *a_z, double *a_y)
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
yKINE_move_first         (int a_leg, int a_seg, double *a_sec, double *a_deg)
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
yKINE_move_next          (double *a_sec, double *a_deg)
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
yKINE_move_prev          (double *a_sec, double *a_deg)
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
yKINE_move_last          (int a_leg, int a_seg, double *a_sec, double *a_deg)
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
yKINE_move_last_servo    (int a_servo, double *a_sec, double *a_deg)
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

char         /*--> retrieve an exact place in time -------[ ------ [ ------ ]-*/
yKINE_move_exact         (double a_sec, int a_leg, double *a_diffx, double *a_diffz, double *a_diffy, double *a_y)
{
   char        rc1         = 0;
   char        rc2         = 0;
   char        rc3         = 0;
   double      x_xpos      = 0.0;
   double      x_zpos      = 0.0;
   double      x_ypos      = 0.0;
   double      x_xdif      = 0.0;
   double      x_zdif      = 0.0;
   double      x_ydif      = 0.0;
   int         x_leg       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_double  ("a_sec"     , a_sec);
   if (a_sec < 0.0     )  {
      DEBUG_YKINE_SCRP   yLOG_warn    ("a_sec"     , "less than zero");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
   }
   if (a_sec > myKINE.scrp_len) {
      DEBUG_YKINE_SCRP   yLOG_warn    ("a_sec"     , "greater than myKINE.scrp_len");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return -2;
   }
   x_leg = a_leg * 3;
   DEBUG_YKINE_SCRP   yLOG_value   ("x_leg"     , x_leg);
   rc1 = yKINE_exact_leg      (a_sec, a_leg);
   rc2 = yKINE_forward  (a_leg, g_servo_info [x_leg + 0].deg, g_servo_info [x_leg + 1].deg, g_servo_info [x_leg + 2].deg);
   DEBUG_YKINE_SCRP   yLOG_value   ("rc2"       , rc2);
   rc3 = yKINE_endpoint (a_leg, YKINE_TIBI, YKINE_FK, NULL, NULL, &x_xpos, &x_zpos, &x_ypos);
   if (a_diffx != NULL)  *a_diffx = g_servo_info [x_leg + 2].xexp - x_xpos;
   if (a_diffz != NULL)  *a_diffz = g_servo_info [x_leg + 2].zexp - x_zpos;
   if (a_diffy != NULL)  *a_diffy = g_servo_info [x_leg + 2].yexp - x_ypos;
   if (a_y     != NULL)  *a_y     = x_ypos;
   x_xdif = g_servo_info [x_leg + 2].xexp - x_xpos;
   x_zdif = g_servo_info [x_leg + 2].zexp - x_zpos;
   x_ydif = g_servo_info [x_leg + 2].yexp - x_ypos;
   /*> if (a_sec < 40.0) {                                                                                                                                                  <* 
    *>    if (a_leg == 0)  printf ("--secs--  leg  rc1  rc2  rc3  --xpos--  --xexp--  --xdif--  --zpos--  --zexp--  --zdif--  --ypos--  --yexp--  --ydif--  --full--\n");   <* 
    *>    printf ("%8.3f   %d   %3d  %3d  %3d  ", a_sec, a_leg, rc1, rc2, rc3);                                                                                             <* 
    *>    printf ("%8.1lf  %8.1lf  %8.1lf  ", x_xpos, g_servo_info [x_leg + 2].xexp, x_xdif);                                                                                   <* 
    *>    printf ("%8.1lf  %8.1lf  %8.1lf  ", x_zpos, g_servo_info [x_leg + 2].zexp, x_zdif);                                                                                   <* 
    *>    printf ("%8.1lf  %8.1lf  %8.1lf  ", x_ypos, g_servo_info [x_leg + 2].yexp, x_ydif);                                                                                   <* 
    *>    printf ("%8.1lf", sqrt ((x_xdif * x_xdif) + (x_zdif * x_zdif) + (x_ydif * x_ydif)));                                                                              <* 
    *>    printf ("\n");                                                                                                                                                    <* 
    *> }                                                                                                                                                                    <*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc2;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_servo_deg          (int a_leg, int a_seg, double *a_deg)
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
yKINE_zero_pos           (double *a_x, double *a_z, double *a_y)
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
yKINE_servo_move         (int a_leg, int a_seg, char *a_label, double *a_secb, double *a_sece, double *a_dur , double *a_degb, double *a_dege, int *a_seq , int *a_line)
{
   int         x_servo     = 0;
   double      x_deg       = 0.0;
   if (a_leg < YKINE_RR   || a_leg > YKINE_LR  )  return -1;
   if (a_seg < YKINE_FEMU || a_seg > YKINE_TIBI)  return -2;
   x_servo = (a_leg * 3) + (a_seg - YKINE_FEMU);
   if (g_servo_info [x_servo].curr == NULL)  return -1;
   if (a_label     != NULL)  strlcpy (a_label, g_servo_info [x_servo].curr->label, LEN_LABEL);
   if (a_secb      != NULL)  *a_secb = g_servo_info [x_servo].curr->sec_beg;
   if (a_sece      != NULL)  *a_sece = g_servo_info [x_servo].curr->sec_end;
   if (a_dur       != NULL)  *a_dur  = g_servo_info [x_servo].curr->sec_dur;
   if (a_degb      != NULL)  *a_degb = g_servo_info [x_servo].curr->deg_beg;
   if (a_dege      != NULL)  *a_dege = g_servo_info [x_servo].curr->deg_end;
   if (a_seq       != NULL)  *a_seq  = g_servo_info [x_servo].curr->seq;
   if (a_line      != NULL)  *a_line = g_servo_info [x_servo].curr->line;
   if (g_servo_info [x_servo].exact == 'y')  return 1;
   return 0;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_servo_line         (int a_leg, int a_seg, double *a_x1, double *a_z1, double *a_y1, double *a_x2, double *a_z2, double *a_y2)
{
   int         x_servo     = 0;
   double      x_deg       = 0.0;
   tMOVE      *x_curr      = NULL;
   tMOVE      *x_prev      = NULL;
   if (a_leg <  YKINE_RR   || a_leg > YKINE_LR  )    return -1;
   if (a_seg != YKINE_TIBI)                          return -2;
   x_servo = (a_leg * 3) + (a_seg - YKINE_FEMU);
   x_curr = g_servo_info [x_servo].curr;
   if (x_curr        == NULL)      return -3;
   x_prev = g_servo_info [x_servo].curr->s_prev;
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
   int         i           = 0;
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_move      = NULL;
   int         x_count     = 0;
   printf ("yKINE scripting report of all servo moves\n");
   printf ("\n");
   for (i = 0; i < g_nservo; ++i) {
      x_servo = g_servo_info + i;
      printf ("%2d) %s\n", i, x_servo->label);
      x_move  = x_servo->head;
      x_count = 0;
      while (x_move != NULL) {
         if (x_count % 45 == 0)  printf ("\n   seq-  t  ---label-------  line  --secb--  --sece--  --secs--  --degb--  --dege--  --xpos--  --zpos--  --ypos--\n");
         if (x_count %  5 == 0)  printf ("\n");
         printf ("   %4d  %c  %-15.15s  %4d  ",
               x_move->seq    , x_move->type   , x_move->label  , x_move->line);
         if (x_move->sec_dur == 0.0)  printf ("     - -       - -       - -  ");
         else                         printf ("%8.3lf  %8.3lf  %8.3lf  ", x_move->sec_beg, x_move->sec_end, x_move->sec_dur);
         printf ("%8.1lf  %8.1lf  ", x_move->deg_beg, x_move->deg_end);
         if (x_move->x_pos + x_move->z_pos + x_move->y_pos == 0.0) {
            printf ("     - -       - -       - -\n");
         } else {
            printf ("%8.2lf  %8.2lf  %8.2lf\n", x_move->x_pos  , x_move->z_pos  , x_move->y_pos  );
         }
         x_move  = x_move->s_next;
         ++x_count;
      }
      printf ("\n");
   }
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
      x_next  = x_servo->head;
      if (x_next  == NULL) {
         sprintf  (ykine__unit_answer, "MOVE unit      : no moves for this servo");
         return ykine__unit_answer;
      }
      while (x_next != NULL) {
         x_move = x_next;
         if (c >= a_cnt)  break;
         x_next = x_next->s_next;
         ++c;
      }
      if (c < a_cnt) {
         sprintf  (ykine__unit_answer, "MOVE unit      : past end of moves");
         return ykine__unit_answer;
      }
      /*---(done)------------------------*/
   }
   /*---(answer)-------------------------*/
   if (strcmp (a_question, "header"  ) == 0) {
      sprintf (ykine__unit_answer, "MOVE header    : %2d/%2d %c %-10.10s %8.3lf %8.3lf %8.3lf", x_move->seq, x_servo->count, x_move->type, x_move->label, x_move->sec_beg, x_move->sec_end, x_move->sec_dur);
   }
   else if (strcmp (a_question, "detail"  ) == 0) {
      sprintf (ykine__unit_answer, "MOVE detail    : %8.1lfb %8.1lfe %8.1lfx %8.1lfz %8.1lfy", x_move->deg_beg, x_move->deg_end, x_move->x_pos, x_move->z_pos, x_move->y_pos);
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
      if (s_curr == NULL)  sprintf (ykine__unit_answer, "MOVE current   : no current move");
      else                 sprintf (ykine__unit_answer, "MOVE current   : %2d/%2d %c %-10.10s %8.3lf %8.3lf %8.3lf", s_curr->seq, s_servo->count, s_curr->type, s_curr->label, s_curr->sec_beg, s_curr->sec_end, s_curr->sec_dur);
   }
   else if (strcmp (a_question, "exact"   ) == 0) {
      sprintf (ykine__unit_answer, "MOVE exact     : %c %8.1lfd, %8.1lfx, %8.1lfz, %8.1lfy", x_servo->exact, x_servo->deg, x_servo->xexp, x_servo->zexp, x_servo->yexp);
   }
   /*---(complete)-----------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
