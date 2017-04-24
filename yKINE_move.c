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
yKINE__move_free   (
      /*----------+-----------+-----------------------------------------------*/
      tMOVE      *a_move)
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
   DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , yKINE_its.scrp_len);
   if (x_move->sec_end > yKINE_its.scrp_len) {
      yKINE_its.scrp_len = x_move->sec_end;
      DEBUG_YKINE_SCRP   yLOG_note    ("end time greater than current length");
      DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , yKINE_its.scrp_len);
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
yKINE_move_delete  (tMOVE *a_move)
{
   tMOVE      *x_next      = NULL;
   /*---(defense)------------------------*/
   if (a_move        == NULL)  return -1;
   /*---(update timings)-----------------*/
   x_next = a_move->s_next;
   while (x_next != NULL) {
      x_next->sec_beg -= a_move->sec_dur;
      x_next->sec_end -= a_move->sec_dur;
      x_next = x_next->s_next;
   }
   /*---(update next deg_beg)------------*/
   if (a_move->s_prev != NULL && a_move->s_next != NULL) {
      a_move->s_next->deg_beg = a_move->s_prev->deg_end;
   }
   /*---(mark deleted)-------------------*/
   a_move->type     = YKINE_MOVE_DEL;
   a_move->sec_dur  = 0.0;
   a_move->sec_beg  = 0.0;
   a_move->sec_end  = 0.0;
   a_move->deg_beg  = a_move->s_prev->deg_end;
   a_move->deg_end  = a_move->s_prev->deg_end;
   /*> /+---(remove from servo list)---------+/                                       <* 
    *> DEBUG_YKINE_SCRP   yLOG_note    ("remove from backwards/prev direction");      <* 
    *> if (a_move->s_next != NULL) a_move->s_next->s_prev = a_move->s_prev;           <* 
    *> else                        a_move->servo->tail    = a_move->s_prev;           <* 
    *> DEBUG_YKINE_SCRP   yLOG_note    ("remove from forewards/next direction");      <* 
    *> if (a_move->s_prev != NULL) a_move->s_prev->s_next = a_move->s_next;           <* 
    *> else                        a_move->servo->head    = a_move->s_next;           <* 
    *> DEBUG_YKINE_SCRP   yLOG_note    ("decrement count");                           <* 
    *> --a_move->servo->count;                                                        <* 
    *> yKINE__move_free (a_move);                                                     <*/
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      current position                        ----===*/
/*====================------------------------------------====================*/
static void      o___CURRENT_________________o (void) {;}

char         /*--> set the current move for a servo ------[ ------ [ ------ ]-*/
yKINE_move_curset        (int a_servo, double a_time)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_next      = NULL;
   int         x_curlong   = 0;
   int         x_beglong   = 0;
   int         x_endlong   = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   x_next = g_servos [a_servo].head;
   g_servos [a_servo].exact = '-';
   if (x_next == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no moves for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   x_curlong = round (a_time          * 1000.0);
   /*---(walk thru moves)----------------*/
   while (x_next != NULL) {
      x_beglong = round (x_next->sec_beg * 1000.0);
      x_endlong = round (x_next->sec_end * 1000.0);
      DEBUG_YKINE_SCRP   yLOG_sint    (x_next->seq);
      if (x_curlong < x_beglong) {
         DEBUG_YKINE_SCRP   yLOG_snote   ("too far");
         DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
         return -11;
      }
      if (x_curlong >= x_endlong) {
         x_next = x_next->s_next;
         continue;
      }
      DEBUG_YKINE_SCRP   yLOG_snote   ("found it");
      g_servos [a_servo].curr = x_next;
      g_servos [a_servo].exact = 'y';
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("failed");
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return -12;
}

char         /*--> calc the current deg for a servo ------[ ------ [ ------ ]-*/
yKINE_move_curone        (int a_servo, double a_time)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* generic return code            */
   tMOVE      *x_curr      = NULL;
   double      x_beg       = 0.0;
   double      x_end       = 0.0;
   double      x_pct       = 0.0;
   double      x_range     = 0.0;
   double      x_pos       = 0.0;
   int         x_curlong   = 0;
   int         x_beglong   = 0;
   int         x_endlong   = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_info    ("label"     , g_servos [a_servo].label);
   g_servos [a_servo].exact = '-';
   /*---(check for correct current)------*/
   if        (g_servos [a_servo].curr == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("servo current is not set");
      rc = yKINE_move_curset (a_servo, a_time);
      DEBUG_YKINE_SCRP   yLOG_value   ("rc"        , rc);
   }
   /*---(check return code)--------------*/
   if      (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check for right times)----------*/
   x_curr  = g_servos [a_servo].curr;
   x_curlong = round (a_time          * 1000.0);
   x_beglong = round (x_curr->sec_beg * 1000.0);
   x_endlong = round (x_curr->sec_end * 1000.0);
   /*> printf ("\na_time = %8.5lf, sec_end = %8.5lf, sec_end = %8.5lf\n", a_time, x_curr->sec_beg, x_curr->sec_end);   <* 
    *> printf (  "a_time = %8d, sec_beg %8d, sec_end %8d\n", x_curlong, x_beglong, x_endlong);                         <*/
   if        (x_curlong < x_beglong) {
      DEBUG_YKINE_SCRP   yLOG_note    ("time is before current move beg");
      rc = yKINE_move_curset (a_servo, a_time);
      DEBUG_YKINE_SCRP   yLOG_value   ("rc"        , rc);
      /*> printf ("before\n");                                                        <*/
   } else if (x_curlong >= x_endlong) {
      DEBUG_YKINE_SCRP   yLOG_note    ("time is on or after current move end");
      rc = yKINE_move_curset (a_servo, a_time);
      DEBUG_YKINE_SCRP   yLOG_value   ("rc"        , rc);
      /*> printf ("on-or-after\n");                                                   <*/
   }
   /*---(check return code)--------------*/
   if      (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   x_curr  = g_servos [a_servo].curr;
   x_curlong = round (a_time          * 1000.0);
   x_beglong = round (x_curr->sec_beg * 1000.0);
   x_endlong = round (x_curr->sec_end * 1000.0);
   /*> printf ("\na_time = %8.5lf, sec_end = %8.5lf, sec_end = %8.5lf\n", a_time, x_curr->sec_beg, x_curr->sec_end);   <* 
    *> printf (  "a_time = %8d, sec_beg %8d, sec_end %8d\n", x_curlong, x_beglong, x_endlong);                         <*/
   /*---(calc degrees)-------------------*/
   DEBUG_YKINE_SCRP   yLOG_double  ("sec_beg"   , x_curr->sec_beg);
   DEBUG_YKINE_SCRP   yLOG_double  ("sec_end"   , x_curr->sec_end);
   x_beg   = x_curr->sec_beg;
   x_end   = x_curr->sec_end;
   DEBUG_YKINE_SCRP   yLOG_double  ("deg_beg"   , x_curr->deg_beg);
   DEBUG_YKINE_SCRP   yLOG_double  ("deg_end"   , x_curr->deg_end);
   x_pct   = (a_time - x_beg) / (x_end - x_beg);
   if (x_end == 0.0)  x_pct = 0.0;
   DEBUG_YKINE_SCRP   yLOG_double  ("x_pct"     , x_pct);
   x_range = x_curr->deg_end - x_curr->deg_beg;
   DEBUG_YKINE_SCRP   yLOG_double  ("x_range"   , x_range);
   x_pos   = x_curr->deg_beg + (x_range * x_pct);
   DEBUG_YKINE_SCRP   yLOG_double  ("x_pos"     , x_pos);
   g_servos [a_servo].deg = x_pos;
   /*---(calc position)------------------*/
   if (x_curr->s_prev == NULL) {
      g_servos [a_servo].xexp  = x_curr->x_pos;
      g_servos [a_servo].zexp  = x_curr->z_pos;
      g_servos [a_servo].yexp  = x_curr->y_pos;
   } else {
      x_range = x_curr->x_pos - x_curr->s_prev->x_pos;
      g_servos [a_servo].xexp = x_curr->s_prev->x_pos + (x_range * x_pct);
      x_range = x_curr->z_pos - x_curr->s_prev->z_pos;
      g_servos [a_servo].zexp = x_curr->s_prev->z_pos + (x_range * x_pct);
      x_range = x_curr->y_pos - x_curr->s_prev->y_pos;
      g_servos [a_servo].yexp = x_curr->s_prev->y_pos + (x_range * x_pct);
   }
   /*> printf ("%8.3lf  %2d  %-10s  %8.2lf  %8.2lf  %8.2lf\n", a_time, a_servo,         <* 
    *>       x_curr->label,                                                             <* 
    *>    g_servos [a_servo].xexp, g_servos [a_servo].zexp, g_servos [a_servo].yexp);   <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> calc current move/deg for a leg -------[ ------ [ ------ ]-*/
yKINE_move_curleg        (double a_secs, int a_leg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;
   /*---(header)-------------------------*/
   rc = yKINE_move_curone  (a_leg * 3 + 0, a_secs);
   rc = yKINE_move_curone  (a_leg * 3 + 1, a_secs);
   rc = yKINE_move_curone  (a_leg * 3 + 2, a_secs);
   return 0;
}

char         /*--> calc current move/deg for all servos --[ ------ [ ------ ]-*/
yKINE_move_curall        (double a_time)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (strcmp (g_servos [i].label, "end-of-list") == 0)  break;
      rc = yKINE_move_curone  (i, a_time);
   }
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     sequential search                        ----===*/
/*====================------------------------------------====================*/
static void      o___SEQUENCIAL______________o (void) {;}

static tMOVE   *s_curr = NULL;

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_first         (int a_servo, double *a_sec, double *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(skip non-moves)-----------------*/
   x_next = g_servos [a_servo].head;
   while (x_next != NULL) {
      if (x_next->type == YKINE_MOVE_INIT )  break;
      if (x_next->type == YKINE_MOVE_SERVO)  break;
      if (x_next->type == YKINE_MOVE_WAIT )  break;
      x_next = x_next->s_next;
   }
   /*---(refuse null)--------------------*/
   if (x_next == NULL) {
      s_curr  = NULL;
      if (a_sec != NULL)  *a_sec  = 0.0;
      if (a_sec != NULL)  *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no moves for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(return values)------------------*/
   DEBUG_YKINE_SCRP   yLOG_svalue  ("found move", x_next->seq);
   s_curr  = x_next;
   if (a_sec != NULL)  *a_sec  = s_curr->sec_end;
   if (a_deg != NULL)  *a_deg  = s_curr->deg_end;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the next move ----------------[ ------ [ ------ ]-*/
yKINE_move_next          (double *a_sec, double *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(get current)--------------------*/
   x_next = s_curr;
   if (x_next == NULL) {
      s_curr  = NULL;
      if (a_sec != NULL)  *a_sec  = 0.0;
      if (a_deg != NULL)  *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no current move for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(skip non-moves)-----------------*/
   x_next = x_next->s_next;
   while (x_next != NULL) {
      if (x_next->type == YKINE_MOVE_INIT )  break;
      if (x_next->type == YKINE_MOVE_SERVO)  break;
      if (x_next->type == YKINE_MOVE_WAIT )  break;
      x_next = x_next->s_next;
   }
   /*---(get next)-----------------------*/
   if (x_next == NULL) {
      s_curr  = NULL;
      if (a_sec != NULL)  *a_sec  = 0.0;
      if (a_deg != NULL)  *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no next move for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -11;
   }
   /*---(return values)------------------*/
   DEBUG_YKINE_SCRP   yLOG_svalue  ("found move", x_next->seq);
   s_curr  = x_next;
   if (a_sec != NULL)  *a_sec  = x_next->sec_end;
   if (a_deg != NULL)  *a_deg  = x_next->deg_end;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the prev move ----------------[ ------ [ ------ ]-*/
yKINE_move_prev          (double *a_sec, double *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_prev      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(get current)--------------------*/
   x_prev = s_curr;
   if (x_prev == NULL) {
      s_curr  = NULL;
      if (a_sec != NULL)  *a_sec  = 0.0;
      if (a_deg != NULL)  *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no current move for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(skip non-moves)-----------------*/
   x_prev = x_prev->s_prev;
   while (x_prev != NULL) {
      if (x_prev->type == YKINE_MOVE_INIT )  break;
      if (x_prev->type == YKINE_MOVE_SERVO)  break;
      if (x_prev->type == YKINE_MOVE_WAIT )  break;
      x_prev = x_prev->s_prev;
   }
   /*---(get next)-----------------------*/
   if (x_prev == NULL) {
      s_curr  = NULL;
      if (a_sec != NULL)  *a_sec  = 0.0;
      if (a_deg != NULL)  *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no prev move for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -11;
   }
   /*---(return values)------------------*/
   DEBUG_YKINE_SCRP   yLOG_svalue  ("found move", x_prev->seq);
   s_curr  = x_prev;
   if (a_sec != NULL)  *a_sec  = x_prev->sec_end;
   if (a_deg != NULL)  *a_deg  = x_prev->deg_end;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("failed");
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
yKINE_move_last          (int a_servo, double *a_sec, double *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_prev      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(skip non-moves)-----------------*/
   x_prev = g_servos [a_servo].tail;
   while (x_prev != NULL) {
       DEBUG_YKINE_SCRP   yLOG_sint    (x_prev->seq);
      if (x_prev->type == YKINE_MOVE_INIT )  break;
      if (x_prev->type == YKINE_MOVE_SERVO)  break;
      if (x_prev->type == YKINE_MOVE_WAIT )  break;
      if (x_prev->type == YKINE_MOVE_NOTE )  break;
      x_prev = x_prev->s_prev;
   }
   /*---(refuse null)--------------------*/
   if (x_prev == NULL) {
      s_curr  = NULL;
      if (a_sec != NULL)  *a_sec  = 0.0;
      if (a_deg != NULL)  *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no moves for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(return values)------------------*/
   DEBUG_YKINE_SCRP   yLOG_svalue  ("found move", x_prev->seq);
   s_curr  = x_prev;
   if (a_sec != NULL)  *a_sec  = s_curr->sec_end;
   if (a_deg != NULL)  *a_deg  = s_curr->deg_end;
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
   if (a_sec > yKINE_its.scrp_len) {
      DEBUG_YKINE_SCRP   yLOG_warn    ("a_sec"     , "greater than yKINE_its.scrp_len");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return -2;
   }
   x_leg = a_leg * 3;
   DEBUG_YKINE_SCRP   yLOG_value   ("x_leg"     , x_leg);
   rc1 = yKINE_move_curleg    (a_sec, a_leg);
   rc2 = yKINE_forward  (a_leg, g_servos [x_leg + 0].deg, g_servos [x_leg + 1].deg, g_servos [x_leg + 2].deg);
   DEBUG_YKINE_SCRP   yLOG_value   ("rc2"       , rc2);
   rc3 = yKINE_endpoint (a_leg, YKINE_TIBI, YKINE_FK, NULL, NULL, &x_xpos, &x_zpos, &x_ypos);
   if (a_diffx != NULL)  *a_diffx = g_servos [x_leg + 2].xexp - x_xpos;
   if (a_diffz != NULL)  *a_diffz = g_servos [x_leg + 2].zexp - x_zpos;
   if (a_diffy != NULL)  *a_diffy = g_servos [x_leg + 2].yexp - x_ypos;
   if (a_y     != NULL)  *a_y     = x_ypos;
   x_xdif = g_servos [x_leg + 2].xexp - x_xpos;
   x_zdif = g_servos [x_leg + 2].zexp - x_zpos;
   x_ydif = g_servos [x_leg + 2].yexp - x_ypos;
   /*> if (a_sec < 40.0) {                                                                                                                                                  <* 
    *>    if (a_leg == 0)  printf ("--secs--  leg  rc1  rc2  rc3  --xpos--  --xexp--  --xdif--  --zpos--  --zexp--  --zdif--  --ypos--  --yexp--  --ydif--  --full--\n");   <* 
    *>    printf ("%8.3f   %d   %3d  %3d  %3d  ", a_sec, a_leg, rc1, rc2, rc3);                                                                                             <* 
    *>    printf ("%8.1lf  %8.1lf  %8.1lf  ", x_xpos, g_servos [x_leg + 2].xexp, x_xdif);                                                                                   <* 
    *>    printf ("%8.1lf  %8.1lf  %8.1lf  ", x_zpos, g_servos [x_leg + 2].zexp, x_zdif);                                                                                   <* 
    *>    printf ("%8.1lf  %8.1lf  %8.1lf  ", x_ypos, g_servos [x_leg + 2].yexp, x_ydif);                                                                                   <* 
    *>    printf ("%8.1lf", sqrt ((x_xdif * x_xdif) + (x_zdif * x_zdif) + (x_ydif * x_ydif)));                                                                              <* 
    *>    printf ("\n");                                                                                                                                                    <* 
    *> }                                                                                                                                                                    <*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc2;
}

char         /*--> get the current deg for servo ---------[ ------ [ ------ ]-*/
yKINE_servo_deg          (int a_leg, int a_seg, double *a_deg)
{
   int         x_servo     = 0;
   double      x_deg       = 0.0;
   if (a_leg < YKINE_RR   || a_leg > YKINE_LR  )  return -1;
   if (a_seg < YKINE_FEMU || a_seg > YKINE_TIBI)  return -2;
   x_servo = (a_leg * 3) + (a_seg - YKINE_FEMU);
   if (g_servos [x_servo].curr == NULL) {
      if (a_seg == YKINE_FEMU)  x_deg  =  0.0;
      if (a_seg == YKINE_PATE)  x_deg  =  0.0;
      if (a_seg == YKINE_TIBI)  x_deg  = 90.0;
      if (a_deg       != NULL)  *a_deg = x_deg;
      return -1;
   }
   if (a_deg       != NULL)  *a_deg = g_servos [x_servo].deg;
   if (g_servos [x_servo].exact == 'y')  return 1;
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
   if (g_servos [x_servo].curr == NULL)  return -1;
   if (a_label     != NULL)  strlcpy (a_label, g_servos [x_servo].curr->label, LEN_LABEL);
   if (a_secb      != NULL)  *a_secb = g_servos [x_servo].curr->sec_beg;
   if (a_sece      != NULL)  *a_sece = g_servos [x_servo].curr->sec_end;
   if (a_dur       != NULL)  *a_dur  = g_servos [x_servo].curr->sec_dur;
   if (a_degb      != NULL)  *a_degb = g_servos [x_servo].curr->deg_beg;
   if (a_dege      != NULL)  *a_dege = g_servos [x_servo].curr->deg_end;
   if (a_seq       != NULL)  *a_seq  = g_servos [x_servo].curr->seq;
   if (a_line      != NULL)  *a_line = g_servos [x_servo].curr->line;
   if (g_servos [x_servo].exact == 'y')  return 1;
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
   x_curr = g_servos [x_servo].curr;
   if (x_curr        == NULL)      return -3;
   x_prev = g_servos [x_servo].curr->s_prev;
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
      x_servo = g_servos + i;
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



/*============================----end-of-source---============================*/
