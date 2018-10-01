/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*====================------------------------------------====================*/
/*===----                        shared functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_set_servo   (char a_meth, int a_leg, int a_seg, float a_deg, float a_beat)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       s           =  0.0;
   float       x, y, z;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   s = a_beat * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_secs"    , s);
   if (rc == 0)  rc = ykine_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, a_deg, s);
   if (rc == 0)  rc = yKINE_endpoint    (a_leg, a_seg, a_meth, NULL, NULL, &x, &z, &y);
   if (rc == 0)  rc = ykine_move_addloc (x_servo, x, z, y);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         fk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___FK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_fk_getter       (int a_leg, int a_seg, char *a_entry, float *a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d           =  0.0;
   float       dp          =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get values)------------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, &dp, NULL, NULL, NULL, NULL);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_adjval   (dp, a_entry, &d);
   } else {
      if (rc == 0)  rc  = ykine_queue_adjfrom  (dp, a_entry, &d);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("deg"       , d);
   /*---(return)-------------------------*/
   *a_deg = d;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_fk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   float       b           =  0.0;
   int         x_leg       = 0.0;
   float       f, p, t;
   char       *x_femu      [LEN_LABEL];
   char       *x_pate      [LEN_LABEL];
   char       *x_tibi      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_queue_popservo ();
   DEBUG_YKINE_SCRP  yLOG_value   ("servo"     , rc);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = ykine_queue_popstr    (x_femu);
   if (rc >= 0)  rc = ykine_queue_popstr    (x_pate);
   if (rc >= 0)  rc = ykine_queue_popstr    (x_tibi);
   /*---(handle trouble)--------------*/
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      if (ykine_servo_unfocused (x_leg, YKINE_FEMU))  continue;
      if (rc >= 0)  rc = ykine__scrp_fk_getter (x_leg, YKINE_FEMU, x_femu, &f);
      if (rc >= 0)  rc = ykine__scrp_fk_getter (x_leg, YKINE_PATE, x_pate, &p);
      if (rc >= 0)  rc = ykine__scrp_fk_getter (x_leg, YKINE_TIBI, x_tibi, &t);
      /*---(calculate endpoint)----------*/
      if (rc >= 0)  rc = yKINE_forward     (x_leg, f, p, t);
      DEBUG_YKINE_SCRP  yLOG_value   ("forward"   , rc);
      /*---(process moves)---------------*/
      if (rc >= 0)  rc = ykine__scrp_set_servo (YKINE_FK, x_leg, YKINE_FEMU, f, b);
      if (rc >= 0)  rc = ykine__scrp_set_servo (YKINE_FK, x_leg, YKINE_PATE, p, b);
      if (rc >= 0)  rc = ykine__scrp_set_servo (YKINE_FK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___IK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_ik_getter   (int a_leg, char *x_str, float *x, char *z_str, float *z, char *y_str, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       xp, zp, yp;                       /* previous coordinates      */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(det coordinates)-------------*/
   rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xp, &zp, &yp, NULL);
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = ykine_queue_adjval   (xp, x_str, x);
      if (rc == 0)  rc  = ykine_queue_adjval   (zp, z_str, z);
      if (rc == 0)  rc  = ykine_queue_adjval   (yp, y_str, y);
   } else {
      if (rc == 0)  rc  = ykine_queue_adjfrom  (xp, x_str, x);
      if (rc == 0)  rc  = ykine_queue_adjfrom  (zp, z_str, z);
      if (rc == 0)  rc  = ykine_queue_adjfrom  (yp, y_str, y);
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", *x, *z, *y);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle inverse verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_ik           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   float       d           =  0.0;
   float       l           =  0.0;
   float       b, s        =  0.0;
   float       x, z, y;
   float       f, p, t;
   char       *x_str       [LEN_LABEL];
   char       *z_str       [LEN_LABEL];
   char       *y_str       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   if (rc >= 0)  rc  = ykine_queue_popservo ();
   DEBUG_YKINE_SCRP  yLOG_value   ("servo"     , rc);
   /*---(get duration)----------------*/
   if (rc >= 0)  rc  = ykine_queue_popval   (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   /*---(get positions)---------------*/
   if (rc >= 0)  rc = ykine_queue_popstr    (x_str);
   if (rc >= 0)  rc = ykine_queue_popstr    (z_str);
   if (rc >= 0)  rc = ykine_queue_popstr    (y_str);
   /*---(handle trouble)--------------*/
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      if (ykine_servo_unfocused (x_leg, YKINE_TIBI))  continue;
      /*---(get positions)---------------*/
      if (rc >= 0)  rc = ykine__scrp_ik_getter (x_leg, x_str, &x, z_str, &z, y_str, &y);
      /*---(inverse kinematics)----------*/
      if (rc >= 0)  rc = yKINE_inverse (x_leg, x, z, y);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(retrieve angles)-------------*/
      if (rc >= 0)  rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &f, &p, &t);
      DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , f);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , p);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , t);
      /*---(process moves)---------------*/
      if (rc >= 0)  rc = ykine__scrp_set_servo (YKINE_IK, x_leg, YKINE_FEMU, f, b);
      if (rc >= 0)  rc = ykine__scrp_set_servo (YKINE_IK, x_leg, YKINE_PATE, p, b);
      if (rc >= 0)  rc = ykine__scrp_set_servo (YKINE_IK, x_leg, YKINE_TIBI, t, b);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save abolute ik based move ------------[ ------ [ ------ ]-*/
ykine_scrp_ik_pure      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   double      x_xsave     =  0.0;
   double      x_zsave     =  0.0;
   double      x_ysave     =  0.0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      if (ykine_servo_unfocused (x_leg, YKINE_FEMU))  continue;
      /*---(clean values)----------------*/
      /*> myKINE.s_xpos = x_xsave;                                                    <* 
       *> myKINE.s_zpos = x_zsave;                                                    <* 
       *> myKINE.s_ypos = x_ysave;                                                    <*/
      /*---(adjust)----------------------*/
      /*> rc = ykine__parse_adjust ();                                                <*/
      /*---(calc angles)-----------------*/
      rc = yKINE_inverse  (x_leg, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse rc", rc);
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &myKINE.s_femu, &myKINE.s_pate, &myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , myKINE.s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , myKINE.s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , myKINE.s_tibi);
      /*---(add moves)-------------------*/
      /*> rc = ykine_move_create (MOVE_SERVO, g_servo_info + i + 0, myKINE.s_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);   <* 
       *> rc = ykine_move_create (MOVE_SERVO, g_servo_info + i + 1, myKINE.s_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);   <* 
       *> rc = ykine_move_create (MOVE_SERVO, g_servo_info + i + 2, myKINE.s_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);   <* 
       *> rc = ykine_move_addloc (g_servo_info + i + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);                               <*/
      /*---(add femur)-------------------*/
      x_servo = ykine_servo_pointer (x_leg, YKINE_FEMU);
      if (x_servo ==  NULL)  continue;
      rc = ykine_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      /*---(add patella)-----------------*/
      x_servo = ykine_servo_pointer (x_leg, YKINE_PATE);
      if (x_servo ==  NULL)  continue;
      rc = ykine_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      /*---(add tibia)-------------------*/
      x_servo = ykine_servo_pointer (x_leg, YKINE_TIBI);
      if (x_servo ==  NULL)  continue;
      rc = ykine_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      /*---(add endpoint)----------------*/
      rc = ykine_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_scrp_ik_from      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         j           = 0;
   int         x_leg       = 0.0;
   double      x_xsave     = 0.0;
   double      x_zsave     = 0.0;
   double      x_ysave     = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(parse)--------------------------*/
   /*> rc = ykine_parse_fields_OLD   ();                                              <*/
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   /*> rc = ykine_parse_check   ();                                                   <*/
   /*> if (rc < 0) {                                                                  <* 
    *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                              <* 
    *>    return rc;                                                                  <* 
    *> }                                                                              <*/
   /*---(process)------------------------*/
   x_xsave = myKINE.s_xpos;
   x_zsave = myKINE.s_zpos;
   x_ysave = myKINE.s_ypos;
   for (j = 0; j < g_nservo; ++j) {
      if (g_servo_info [j].scrp != 'y') continue;
      /*---(identify len)----------------*/
      x_leg = j / 3.0;
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(clean values)----------------*/
      /*> myKINE.s_xpos = x_xsave;                                                    <* 
       *> myKINE.s_zpos = x_zsave;                                                    <* 
       *> myKINE.s_ypos = x_ysave;                                                    <*/
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_xpos"    , myKINE.s_xpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_zpos"    , myKINE.s_zpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_ypos"    , myKINE.s_ypos);
      /*---(calc new)--------------------*/
      /*> DEBUG_YKINE_SCRP  yLOG_double  ("xsave"     , g_servo_info [j + 2].xsave );   <*/
      /*> DEBUG_YKINE_SCRP  yLOG_double  ("zsave"     , g_servo_info [j + 2].zsave );   <*/
      /*> DEBUG_YKINE_SCRP  yLOG_double  ("ysave"     , g_servo_info [j + 2].ysave );   <*/
      /*> myKINE.s_xpos += g_servo_info [j + 2].xsave;                                <* 
       *> myKINE.s_zpos += g_servo_info [j + 2].zsave;                                <* 
       *> myKINE.s_ypos += g_servo_info [j + 2].ysave;                                <*/
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_xpos new", myKINE.s_xpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_zpos new", myKINE.s_zpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_ypos new", myKINE.s_ypos);
      /*---(adjust)----------------------*/
      /*> rc = ykine__parse_adjust ();                                                <*/
      /*---(get angles)------------------*/
      rc = yKINE_inverse  (x_leg, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &myKINE.s_femu, &myKINE.s_pate, &myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , myKINE.s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , myKINE.s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , myKINE.s_tibi);
      rc = ykine_move_create (MOVE_SERVO, g_servo_info + j + 0, myKINE.s_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      rc = ykine_move_create (MOVE_SERVO, g_servo_info + j + 1, myKINE.s_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      rc = ykine_move_create (MOVE_SERVO, g_servo_info + j + 2, myKINE.s_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      rc = ykine_move_addloc (g_servo_info + j + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}
