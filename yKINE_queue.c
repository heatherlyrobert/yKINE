/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*---1----- -----2----- -----3----- -----4----- ----------comments------------*/
#define     MAX_QUEUE   10
static      char       *s_queue     [MAX_QUEUE];
static      int         s_nqueue    =    0;
static      int         s_cqueue    =    0;


char
ykine_queue_wipe        (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_index < 0)            return rce;
   --rce;  if (a_index >= MAX_QUEUE)   return rce;
   /*---(clear)--------------------------*/
   if (s_queue [a_index] != NULL)  free (s_queue [a_index]);
   s_queue [a_index] = NULL;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_queue_purge       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*---(clear)--------------------------*/
   for (i = 0; i < MAX_QUEUE; ++i) {
      ykine_queue_wipe (i);
   }
   /*---(reset)--------------------------*/
   s_nqueue  = 0;
   s_cqueue  = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_queue_push        (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_string == NULL)       return rce;
   --rce;  if (s_nqueue >= MAX_QUEUE)  return rce;
   /*---(enqueue)------------------------*/
   s_queue [s_nqueue] = strdup (a_string);
   ++s_nqueue;
   /*---(complete)-----------------------*/
   return 0;
}


char         /*--> parse incomming record ----------------[ ------ [ ------ ]-*/
ykine_queue_recd            (char *a_recd)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          =   0;                /* generic return code       */
   int         x_len       = 0;
   int         c           =    0;
   char       *p           = NULL;
   char       *q           = "";
   char       *r           = NULL;
   int         i           =    0;
   float       x_temp      =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_YKINE_SCRP  yLOG_point   ("a_recd"    , a_recd);
   --rce;  if (a_recd == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("a_recd"    , a_recd);
   x_len = strllen   (a_recd, LEN_RECD);
   DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);
   c     = strldcnt (a_recd, q [0], LEN_RECD);
   DEBUG_YKINE_SCRP  yLOG_value   ("c"         , c);
   /*---(get first)----------------------*/
   p     = strtok_r (a_recd, q, &r);
   DEBUG_YKINE_SCRP  yLOG_point   ("p"         , p);
   c     = 0;
   /*---(read fields)--------------------*/
   while (p != NULL) {
      /*---(clean it up)-----------------*/
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strllen (p, LEN_RECD);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);
      /*---(enqueue it)------------------*/
      rc = ykine_queue_push (p);
      DEBUG_YKINE_SCRP  yLOG_value   ("enqueue"   , rc);
      ++c;
      /*---(get next)--------------------*/
      p = strtok_r (NULL  , q, &r);
      DEBUG_YKINE_SCRP  yLOG_point   ("p"         , p);
      /*---(done)------------------------*/
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("fields"    , c);
   /*---(done)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine__queue_popcheck   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense on empty)---------------*/
   DEBUG_YKINE_SCRP   yLOG_sint    (s_nqueue);
   --rce;  if (s_nqueue <= 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("none to pop");
      return rce;
   }
   /*---(defense on all used)------------*/
   DEBUG_YKINE_SCRP   yLOG_sint    (MAX_QUEUE);
   DEBUG_YKINE_SCRP   yLOG_sint    (s_cqueue);
   --rce;  if (s_cqueue >= MAX_QUEUE) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("all popped");
      return rce;
   }
   /*---(defense on null)----------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (s_queue [s_cqueue]);
   --rce;  if (s_queue [s_cqueue] == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("null entry");
      return rce;
   }
   /*---(done)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("checked");
   return 0;
}

char
ykine_queue_popstr      (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_string);
   --rce;  if (a_string == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (a_string, "", LEN_STR);
   /*---(check)--------------------------*/
   rc = ykine__queue_popcheck ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(enqueue)------------------------*/
   strlcpy (a_string, s_queue [s_cqueue], LEN_STR);
   DEBUG_YKINE_SCRP   yLOG_snote   (a_string);
   /*---(prepare for next)---------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("wiping");
   ykine_queue_wipe (s_cqueue);
   ++s_cqueue;
   DEBUG_YKINE_SCRP   yLOG_sint    (s_cqueue);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_queue_popval      (float *a_value, char *a_rel)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_value);
   --rce;  if (a_value == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_rel);
   --rce;  if (a_rel == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   *a_rel   = 0;
   *a_value = 0.0;
   /*---(check)--------------------------*/
   rc = ykine__queue_popcheck ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
   /*---(prepare)------------------------*/
   *a_rel = 0;
   x_len = strlen (s_queue [s_cqueue]);
   /*---(check empty)--------------------*/
   --rce;  if (x_len == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("empty");
      *a_rel   = 0;
      *a_value = 0.0;
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rce;
   }
   /*---(check zero marker)--------------*/
   if (x_len == 3 && strcmp (s_queue [s_cqueue], "-.-") == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("zero marker");
      *a_rel   = 0;
      *a_value = 0.0;
   }
   /*---(check one marker)---------------*/
   else if (x_len == 1 && s_queue [s_cqueue][0] == '-') {
      DEBUG_YKINE_SCRP   yLOG_snote   ("one marker");
      *a_rel   = 0;
      *a_value = 1.0;
   }
   /*---(check unchanged marker)---------*/
   else if (x_len == 1 && s_queue [s_cqueue][0] == '"') {
      DEBUG_YKINE_SCRP   yLOG_snote   ("unchanged marker");
      *a_rel   = 1;
      *a_value = 0.0;
   }
   /*---(check normal value)-------------*/
   else {
      DEBUG_YKINE_SCRP   yLOG_snote   ("normal");
      if (x_len > 1 && s_queue [s_cqueue][x_len - 1] == 'r') {
         *a_rel   = 1;
         s_queue [s_cqueue][--x_len] = 0;
      }
      *a_value = atof (s_queue [s_cqueue]);
   }
   /*---(output)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_sint    (*a_rel);
   DEBUG_YKINE_SCRP   yLOG_sint    (*a_value);
   /*---(prepare for next)---------------*/
   DEBUG_YKINE_SCRP   yLOG_snote   ("wiping");
   ykine_queue_wipe (s_cqueue);
   ++s_cqueue;
   DEBUG_YKINE_SCRP   yLOG_sint    (s_cqueue);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_queue_popverb     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   int         i           =    0;
   char       *x_verb      = NULL;
   int         x_len       =    0;
   int         x_index     =   -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(pop)----------------------------*/
   rc = ykine_queue_popstr (x_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
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
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_queue_servos      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP  yLOG_value   ("s_nqueue"  , s_nqueue);
   --rce;  if (s_nqueue <= 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("MAX_QUEUE" , MAX_QUEUE);
   DEBUG_YKINE_SCRP  yLOG_value   ("s_cqueue"  , s_cqueue);
   --rce;  if (s_cqueue >= MAX_QUEUE) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_len = strlen (s_queue [s_cqueue]);
   /*---(check empty)--------------------*/
   --rce;  if (x_len == 0) {
      return rce;
   }
   --rce;  if (x_len <  2) {
      return rce;
   }
   --rce;  if (x_len >  2) {
      return rce;
   }
   /*---(identify servos)----------------*/
   p = s_queue [s_cqueue];
   myKINE.s_count = ykine_servos (p);
   DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);
   --rce;  if (myKINE.s_count < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare for next)---------------*/
   ykine_queue_wipe (s_cqueue);
   ++s_cqueue;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
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
ykine__unit_queue       (char *a_question, int a_num)
{
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "QUEUE unit       : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if      (strcmp (a_question, "stats"    ) == 0) {
      sprintf (ykine__unit_answer, "QUEUE stats    : total %2d, curr %2d", s_nqueue, s_cqueue);
   }
   else if (strcmp (a_question, "entry"    ) == 0) {
      if (a_num < 0 || a_num >= MAX_QUEUE) {
         sprintf (ykine__unit_answer, "QUEUE entry  - : not a valid entry");
      } else if (s_queue [a_num] == NULL) {
         sprintf (ykine__unit_answer, "QUEUE entry  %d : null", a_num);
      } else {
         sprintf (ykine__unit_answer, "QUEUE entry  %d : %2d[%s]", a_num, strlen (s_queue [a_num]), s_queue [a_num]);
      }
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



