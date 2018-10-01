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
   char       *q           = "(,";
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
   x_len = strllen   (q, LEN_RECD);
   for (i = c = 0; i < x_len; ++i)   c += strldcnt (a_recd, q [i], LEN_RECD);
   DEBUG_YKINE_SCRP  yLOG_value   ("fields"    , c);
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
ykine_queue_popable     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   rc = ykine__queue_popcheck ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 1;
}

char
ykine_queue_popskip     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   rc = ykine__queue_popcheck ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rc;
   }
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
ykine_queue_adjval      (const float a_old, const char *a_entry, float *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_rel       =  '-';
   char        x_entry     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_entry);
   --rce;  if (a_entry == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_new);
   --rce;  if (a_new   == NULL) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   *a_new = 0.0;
   strlcpy (x_entry, a_entry, LEN_LABEL);
   x_len  = strlen (x_entry);
   /*---(check empty)--------------------*/
   --rce;  if (x_len == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("empty");
      *a_new   = 0.0;
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rc);
      return rce;
   }
   /*---(check zero markers)-------------*/
   if (x_len == 4 && strcmp (x_entry, "-.--") == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("zero marker");
      *a_new = 0.0;
   }
   if (x_len == 3 && strcmp (x_entry, "-.-") == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("zero marker");
      *a_new = 0.0;
   }
   else if (x_len == 1 && x_entry[0] == '-') {
      DEBUG_YKINE_SCRP   yLOG_snote   ("zero marker");
      *a_new = 0.0;
   }
   /*---(check min/one marker)-----------*/
   else if (x_len == 1 && x_entry[0] == '.') {
      DEBUG_YKINE_SCRP   yLOG_snote   ("min/one marker");
      *a_new = 1.0;
   }
   /*---(check unchanged marker)---------*/
   else if (x_len == 1 && x_entry[0] == '=') {
      DEBUG_YKINE_SCRP   yLOG_snote   ("unchanged marker");
      *a_new   = a_old;
   }
   /*---(check normal value)-------------*/
   else {
      DEBUG_YKINE_SCRP   yLOG_snote   ("normal");
      if (x_len > 1 && x_entry[x_len - 1] == 'r') {
         x_rel   = 'y';
         x_entry[--x_len] = 0;
      }
      *a_new = atof (x_entry);
      if (x_rel == 'y')   *a_new += a_old;
   }
   /*---(output)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_sint    (*a_new);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_queue_adjfrom     (const float a_old, const char *a_entry, float *a_new)
{
   char        rc          =    0;
   float       a_temp      =  0.0;
   rc = ykine_queue_adjval (0.0, a_entry, &a_temp);
   if (rc < 0)  return rc;
   *a_new = a_old + a_temp;
   return 0;
}

char
ykine_queue_popval      (const float a_old, float *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_rel       =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(check)--------------------------*/
   rc = ykine__queue_popcheck ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(run)----------------------------*/
   rc = ykine_queue_adjval (a_old, s_queue [s_cqueue], a_new);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(prepare for next)---------------*/
   ykine_queue_wipe (s_cqueue);
   ++s_cqueue;
   DEBUG_YKINE_SCRP   yLOG_value   ("s_cqueue"  , s_cqueue);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_queue_popfrom     (const float a_old, float *a_new)
{
   char        rc          =    0;
   float       a_temp      =  0.0;
   rc = ykine_queue_popval (0.0, &a_temp);
   if (rc < 0)  return rc;
   *a_new = a_old + a_temp;
   return 0;
}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_queue_popverb     (void)
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
   rc = ykine_queue_popstr (x_verb);
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
ykine_queue_popservo    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_servo     [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(pop)----------------------------*/
   rc = ykine_queue_popstr (x_servo);
   if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(prepare)------------------------*/
   x_len = strlen (x_servo);
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



