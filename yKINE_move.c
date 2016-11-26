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

tMOVE*       /*--> create a new, blank move --------------[ leaf   [ ------ ]-*/
MOVE__new          (void)
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

tMOVE*       /*--> destroy a single move -----------------[ leaf   [ ------ ]-*/
MOVE__free         (
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
MOVE_create        (
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
   x_move          = MOVE__new ();
   --rce;  if (x_move == NULL)   {
      DEBUG_YKINE_SCRP yLOG_exit      (__FUNCTION__);
      return rce; 
   }
   /*---(assign basics)---------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("assign basic values");
   x_move->type     = a_type;
   x_move->servo    = a_servo;
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
   /*---(display stats)------------------*/
   DEBUG_YKINE_SCRP   yLOG_value   ("count"     , a_servo->count);
   DEBUG_YKINE_SCRP   yLOG_value   ("seq"       , x_move->seq);
   DEBUG_YKINE_SCRP   yLOG_value   ("sec_beg"   , x_move->sec_beg);
   DEBUG_YKINE_SCRP   yLOG_value   ("sec_end"   , x_move->sec_end);
   DEBUG_YKINE_SCRP   yLOG_value   ("deg_beg"   , x_move->deg_beg);
   /*---(update globals)-----------------*/
   DEBUG_YKINE_SCRP   yLOG_double  ("scrp_len"  , yKINE_its.scrp_len);
   if (x_move->sec_end > yKINE_its.scrp_len) {
      yKINE_its.scrp_len = x_move->sec_end;
      DEBUG_YKINE_SCRP   yLOG_note    ("end time greater than current length");
      DEBUG_YKINE_SCRP   yLOG_double  ("my.p_len"  , yKINE_its.scrp_len);
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
MOVE_addloc        (
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
MOVE_repeat        (
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
         MOVE_create (YKINE_MOVE_SERVO, a_servo, x_label, x_moves [j]->line, x_moves [j]->deg_end, x_moves [j]->sec_dur);
         MOVE_addloc (a_servo, x_moves [j]->x_pos, x_moves [j]->z_pos, x_moves [j]->y_pos);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> repeat from the last segno ------------[ ------ [ ------ ]-*/
MOVE_dalsegno      (
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
         MOVE_create (YKINE_MOVE_SERVO, a_servo, x_label, x_curr->line, x_curr->deg_end, x_curr->sec_dur);
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



/*====================------------------------------------====================*/
/*===----                      script position                         ----===*/
/*====================------------------------------------====================*/
static void      o___POSITION________________o (void) {;}

char         /*--> set the current move for a servo ------[ ------ [ ------ ]-*/
MOVE_curset        (int a_servo, float a_time)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   x_next = g_servos [a_servo].head;
   if (x_next == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("no moves for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(walk thru moves)----------------*/
   while (x_next != NULL) {
      DEBUG_YKINE_SCRP   yLOG_sint    (x_next->seq);
      if (x_next->sec_beg >  a_time) {
         DEBUG_YKINE_SCRP   yLOG_snote   ("too far");
         DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
         return -11;
      }
      if (x_next->sec_end <  a_time) {
         x_next = x_next->s_next;
         continue;
      }
      DEBUG_YKINE_SCRP   yLOG_snote   ("found it");
      g_servos [a_servo].curr = x_next;
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("failed");
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return -12;
}

char         /*--> calc the current deg for a servo ------[ ------ [ ------ ]-*/
MOVE_curone        (int a_servo, double a_time)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* generic return code            */
   tMOVE      *x_curr      = NULL;
   float       x_beg       = 0.0;
   float       x_end       = 0.0;
   float       x_pct       = 0.0;
   float       x_range     = 0.0;
   float       x_pos       = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_info    ("label"     , g_servos [a_servo].label);
   /*---(check for correct current)------*/
   if        (g_servos [a_servo].curr == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("servo current is not set");
      rc = MOVE_curset (a_servo, a_time);
      DEBUG_YKINE_SCRP   yLOG_value   ("rc"        , rc);
   }
   /*---(check return code)--------------*/
   if      (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check for right times)----------*/
   x_curr  = g_servos [a_servo].curr;
   if        (a_time < x_curr->sec_beg) {
      DEBUG_YKINE_SCRP   yLOG_note    ("time is before current move beg");
      rc = MOVE_curset (a_servo, a_time);
      DEBUG_YKINE_SCRP   yLOG_value   ("rc"        , rc);
   } else if (a_time > x_curr->sec_end) {
      DEBUG_YKINE_SCRP   yLOG_note    ("time is after current move end");
      rc = MOVE_curset (a_servo, a_time);
      DEBUG_YKINE_SCRP   yLOG_value   ("rc"        , rc);
   }
   /*---(check return code)--------------*/
   if      (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(calc degrees)-------------------*/
   x_curr  = g_servos [a_servo].curr;
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
MOVE_curleg        (double a_secs, int a_leg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;
   /*---(header)-------------------------*/
   rc = MOVE_curone  (a_leg * 3 + 0, a_secs);
   rc = MOVE_curone  (a_leg * 3 + 1, a_secs);
   rc = MOVE_curone  (a_leg * 3 + 2, a_secs);
   return 0;
}

char         /*--> calc current move/deg for all servos --[ ------ [ ------ ]-*/
MOVE_curall        (double a_time)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (strcmp (g_servos [i].label, "end-of-list") == 0)  break;
      rc = MOVE_curone  (i, a_time);
   }
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static tMOVE   *s_curr = NULL;

char         /*--> retrieve the first move ---------------[ ------ [ ------ ]-*/
MOVE_first         (int a_servo, float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(get current)--------------------*/
   x_next = g_servos [a_servo].head;
   if (x_next == NULL) {
      s_curr  = NULL;
      *a_sec  = 0.0;
      *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no moves for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(return values)------------------*/
   s_curr  = x_next;
   *a_sec  = s_curr->sec_end;
   *a_deg  = s_curr->deg_end;
   DEBUG_YKINE_SCRP   yLOG_snote   ("failed");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve the next move ----------------[ ------ [ ------ ]-*/
MOVE_next          (float *a_sec, float *a_deg)
{
   /*---(locals)-----------+-----------+-*/
   tMOVE      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(get current)--------------------*/
   x_next = s_curr;
   if (x_next == NULL) {
      s_curr  = NULL;
      *a_sec  = 0.0;
      *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no current move for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -10;
   }
   /*---(get next)-----------------------*/
   x_next = x_next->s_next;
   if (x_next == NULL) {
      s_curr  = NULL;
      *a_sec  = 0.0;
      *a_deg  = 0.0;
      DEBUG_YKINE_SCRP   yLOG_snote   ("no next move for servo");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return -11;
   }
   /*---(return values)------------------*/
   s_curr  = x_next;
   *a_sec  = x_next->sec_end;
   *a_deg  = x_next->deg_end;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("failed");
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> retrieve an exact place in time -------[ ------ [ ------ ]-*/
MOVE_exact         (double a_sec, int a_leg, double *a_diffx, double *a_diffz, double *a_diffy, double *a_y)
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
   if (a_sec < 0.0     )  return -1;
   if (a_sec > yKINE_its.scrp_len)  return -2;
   x_leg = a_leg * 3;
   rc1 = MOVE_curleg    (a_sec, a_leg);
   rc2 = yKINE_forward  (a_leg, g_servos [x_leg + 0].deg, g_servos [x_leg + 1].deg, g_servos [x_leg + 2].deg);
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
   return 0;
}




/*============================----end-of-source---============================*/
