/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



static tSERVO  *s_servo   = NULL;
static char     s_accel     [LEN_LABEL] = "";
static double   b, s;               /* input and adjusted durations           */
static float    yb, pb, rb, db, tb; /* begin  yaw, pitch, roll, dir, and tilt */
static double   ye, pe, re, de, te; /* end    yaw, pitch, roll, dir, and tilt */



/*====================------------------------------------====================*/
/*===----                     zero-point movements                     ----===*/
/*====================------------------------------------====================*/
static void      o___ZERO____________________o (void) {;}

char
ykine_body_xz2zp        (float a_xpos, float a_zpos, float *a_deg, float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   float       r           = 0.0;
   float       d           = 0.0;
   float       l           = 0.0;
   /*---(fix degree)---------------------*/
   r = atan2 (-a_zpos, a_xpos);
   d = r * RAD2DEG;
   d = round (d * 10.0) / 10.0;
   while (d <    0.0)   d += 360.0;
   while (d >  360.0)   d -= 360.0;
   /*---(fix length)---------------------*/
   l = sqrt ((a_xpos * a_xpos) + (a_zpos * a_zpos));
   l = round (l * 10.0) / 10.0;
   if (l < 0.1 && l > -0.1)   d = l = 0.0;
   /*---(save results)-------------------*/
   if (a_deg  != NULL)  *a_deg  = d;
   if (a_len  != NULL)  *a_len  = l;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_body_zp2xz        (float a_deg, float a_len, float *a_xpos, float *a_zpos)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x           = 0.0;
   float       z           = 0.0;
   /*---(fix degree)---------------------*/
   while (a_deg <    0.0)   a_deg += 360.0;
   while (a_deg >  360.0)   a_deg -= 360.0;
   /*---(set coordinates)----------------*/
   x =  a_len * cos (a_deg * DEG2RAD);
   z = -a_len * sin (a_deg * DEG2RAD);
   x = round (x * 10.0) / 10.0;
   z = round (z * 10.0) / 10.0;
   if (x < 0.1 && x > -0.1)   x = 0.0;
   if (z < 0.1 && z > -0.1)   z = 0.0;
   /*---(save results)-------------------*/
   if (a_xpos != NULL)  *a_xpos = x;
   if (a_zpos != NULL)  *a_zpos = z;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> identify the servos effected ----------[ ------ [ ------ ]-*/
ykine__body_zero_prep   (char *a_one, char *a_two, char *a_thr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get servo)-------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(old values)---------------------*/
   ykine_move_savedloc  (x_servo, NULL, NULL, &myKINE.xb, &myKINE.zb, &myKINE.yb, NULL);
   /*---(check accel)-----------------*/
   yPARSE_top      (s_accel);
   DEBUG_YKINE_SCRP  yLOG_info    ("s_accel"   , s_accel);
   rc  = ykine_accel_dur (s_accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);
   /*---(check normal)----------------*/
   yPARSE_popval (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   if (rc < 0)  strlcpy (s_accel, "", LEN_LABEL);
   else         b = -1.0;
   /*---(get positions)---------------*/
   rc = yPARSE_popstr    (a_one);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_one"     , "%3d, %s", rc, a_one);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yPARSE_popstr    (a_two);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_two"     , "%3d, %s", rc, a_two);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yPARSE_popstr    (a_thr);
   DEBUG_YKINE_SCRP  yLOG_complex ("a_thr"     , "%3d, %s", rc, a_thr);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__body_ze_getter   (char *x_str, char *z_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      xe, ze, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.xb, x_str, &xe);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.zb, z_str, &ze);
   rc = ykine_stance_scale   (myKINE.s_from        , myKINE.yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xe, ze, ye);
   /*---(save)---------------------------*/
   if (x != NULL)  *x = xe;
   if (z != NULL)  *z = ze;
   if (y != NULL)  *y = ye;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ck based move -----------[ ------ [ ------ ]-*/
ykine__body_zp_getter   (char *d_str, char *o_str, char *y_str, float *x, float *z, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       db, ob, yb;                  /* previous/begin values          */
   double      de, oe, ye;                  /* updated/ending values          */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(convert to ck)---------------*/
   ykine_body_xz2zp (myKINE.xb, myKINE.zb, &db, &ob);
   yb      = myKINE.yb;
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.3fd, %8.2fo, %8.2fy", db, ob, myKINE.yb);
   /*---(calculate)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   rc = ykine_stance_scale   (myKINE.s_from, db, d_str, &de);
   rc = ykine_stance_scale   (myKINE.s_from, ob, o_str, &oe);
   rc = ykine_stance_scale   (myKINE.s_from, yb, y_str, &ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("adjust"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.3fd, %8.2fo, %8.2fy", de, oe, ye);
   /*---(convert from cx)-------------*/
   ykine_body_zp2xz (de, oe, x, z);
   if (y != NULL)  *y = ye;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine__body_zero_driver (char a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   float       d, s        =  0.0;
   float       l           =  0.0;
   double      xe, ze, ye;
   float       xb, zb, yb;                       /* previous coordinates      */
   char       *x_one       [LEN_LABEL];
   char       *x_two       [LEN_LABEL];
   char       *x_thr       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(mark servers)----------------*/
   rc = ykine__body_zero_prep (x_one, x_two, x_thr);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get positions)---------------*/
   switch (a_verb) {
   case YKINE_ZE :  rc = ykine__body_ze_getter (x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
   case YKINE_ZP :  rc = ykine__body_ze_getter (x_one, x_two, x_thr, &myKINE.xe, &myKINE.ze, &myKINE.ye);  break;
   default       :
                    DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, verb type not known");
                    DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
                    return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("getter"    , rc);
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("FAILED, using previous");
      myKINE.xe = myKINE.xb;
      myKINE.ze = myKINE.zb;
      myKINE.ye = myKINE.yb;
   }
   DEBUG_YKINE_SCRP   yLOG_complex ("coords"    , "%8.3lfx , %8.3lfz , %8.3lfy", myKINE.xe, myKINE.ze, myKINE.ye);
   /*---(get distance)----------------*/
   switch (a_verb) {
   case YKINE_ZE : ykine_exact_dist_xzy   (); break;
   case YKINE_ZP : ykine_exact_dist_doy   (); break;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("distance"  , myKINE.le);
   /*---(process moves)---------------*/
   if (b < 0)  {
      ykine_accel_zero      (s_accel, xb, zb, yb, xe, ze, ye);
   } else {
      /*---(caclulate polar)-------------*/
      rc = ykine_body_xz2zp      (xe, ze, &d, &l);
      DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);
      /*---(process moves)---------------*/
      s = b * myKINE.s_pace;
      DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);
      if (myKINE.s_hidden != 'y')  rc = ykine_move_create (YKINE_MOVE_SERVO, YKINE_NONE, x_servo, myKINE.s_verb, myKINE.s_cline, d, s);
      DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);
      if (rc == 0)  rc = ykine_move_addloc (x_servo, xe, ze, ye);
      DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);
      --rce;  if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xe, ze, ye);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_body_zero         (void)
{
   return ykine__body_zero_driver (YKINE_ZE);
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_body_zpolar       (void)
{
   return ykine__body_zero_driver (YKINE_ZP);
}

/*> char         /+--> handle body centering -----------------[ ------ [ ------ ]-+/                                                          <* 
 *> ykine_body_zero         (void)                                                                                                            <* 
 *> {                                                                                                                                         <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                               <* 
 *>    char        rce         =  -10;               /+ return code for errors    +/                                                          <* 
 *>    char        rc          =    0;                                                                                                        <* 
 *>    tSERVO     *x_servo     =    0;                                                                                                        <* 
 *>    float       d, s        =  0.0;                                                                                                        <* 
 *>    float       l           =  0.0;                                                                                                        <* 
 *>    double      xe, ze, ye;                                                                                                                <* 
 *>    float       xb, zb, yb;                       /+ previous coordinates      +/                                                          <* 
 *>    /+---(header)-------------------------+/                                                                                               <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                                                                        <* 
 *>    /+---(servo)--------------------------+/                                                                                               <* 
 *>    x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);                                                                                <* 
 *>    DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);                                                                                <* 
 *>    --rce;  if (x_servo ==  NULL) {                                                                                                        <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                                <* 
 *>       return rce;                                                                                                                         <* 
 *>    }                                                                                                                                      <* 
 *>    /+---(check accel)-----------------+/                                                                                                  <* 
 *>    yPARSE_top      (s_accel);                                                                                                             <* 
 *>    DEBUG_YKINE_SCRP  yLOG_info    ("s_accel"   , s_accel);                                                                                <* 
 *>    rc  = ykine_accel_dur (s_accel);                                                                                                       <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);                                                                                     <* 
 *>    /+---(check normal)----------------+/                                                                                                  <* 
 *>    yPARSE_popval (0.0, &b);                                                                                                               <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);                                                                                      <* 
 *>    if (rc < 0)  strlcpy (s_accel, "", LEN_LABEL);                                                                                         <* 
 *>    else         b = -1.0;                                                                                                                 <* 
 *>    /+---(det coordinates)-------------+/                                                                                                  <* 
 *>    rc = ykine_move_savedloc  (x_servo, NULL, NULL, &xb, &zb, &yb, NULL);                                                                  <* 
 *>    DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);                                                                          <* 
 *>    if (myKINE.s_from == YKINE_PURE) {                                                                                                     <* 
 *>       if (rc == 0)  rc  = yPARSE_popval   (xb, &xe);                                                                                      <* 
 *>       if (rc == 0)  rc  = yPARSE_popval   (zb, &ze);                                                                                      <* 
 *>       if (rc == 0)  rc  = yPARSE_popval   (yb, &ye);                                                                                      <* 
 *>    } else {                                                                                                                               <* 
 *>       if (rc == 0)  rc  = yPARSE_popfrom  (xb, &xe);                                                                                      <* 
 *>       if (rc == 0)  rc  = yPARSE_popfrom  (zb, &ze);                                                                                      <* 
 *>       if (rc == 0)  rc  = yPARSE_popfrom  (yb, &ye);                                                                                      <* 
 *>    }                                                                                                                                      <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);                                                                                     <* 
 *>    --rce;  if (rc <  0) {                                                                                                                 <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                                <* 
 *>       return rce;                                                                                                                         <* 
 *>    }                                                                                                                                      <* 
 *>    DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xe, ze, ye);                                                   <* 
 *>    /+---(process moves)---------------+/                                                                                                  <* 
 *>    if (b < 0)  {                                                                                                                          <* 
 *>       ykine_accel_zero      (s_accel, xb, zb, yb, xe, ze, ye);                                                                            <* 
 *>    } else {                                                                                                                               <* 
 *>       /+---(caclulate polar)-------------+/                                                                                               <* 
 *>       rc = ykine_bogy_xz2zp      (xe, ze, &d, &l);                                                                                        <* 
 *>       DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", d, l);                                                              <* 
 *>       /+---(process moves)---------------+/                                                                                               <* 
 *>       s = b * myKINE.s_pace;                                                                                                              <* 
 *>       DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);                                                                                   <* 
 *>       if (myKINE.s_hidden != 'y')  rc = ykine_move_create (YKINE_MOVE_SERVO, YKINE_NONE, x_servo, myKINE.s_verb, myKINE.s_cline, d, s);   <* 
 *>       DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);                                                                                  <* 
 *>       if (rc == 0)  rc = ykine_move_addloc (x_servo, xe, ze, ye);                                                                         <* 
 *>       DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);                                                                                  <* 
 *>       --rce;  if (rc <  0) {                                                                                                              <* 
 *>          DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                             <* 
 *>          return rce;                                                                                                                      <* 
 *>       }                                                                                                                                   <* 
 *>    }                                                                                                                                      <* 
 *>    /+---(complete)-----------------------+/                                                                                               <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                                                                        <* 
 *>    return 0;                                                                                                                              <* 
 *> }                                                                                                                                         <*/

/*> char         /+--> handle body centering -----------------[ ------ [ ------ ]-+/                                                           <* 
 *> ykine_body_zpolar       (void)                                                                                                             <* 
 *> {                                                                                                                                          <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                                <* 
 *>    char        rce         =  -10;               /+ return code for errors    +/                                                           <* 
 *>    char        rc          =    0;                                                                                                         <* 
 *>    tSERVO     *x_servo     =    0;                                                                                                         <* 
 *>    float       s           =  0.0;                                                                                                         <* 
 *>    float       db, lb, yb;                       /+ previous coordinates      +/                                                           <* 
 *>    double      de, le, ye;                       /+ current coordinates       +/                                                           <* 
 *>    float       xe, ze;                           /+ current coordinates       +/                                                           <* 
 *>    /+---(header)-------------------------+/                                                                                                <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                                                                         <* 
 *>    /+---(servo)--------------------------+/                                                                                                <* 
 *>    x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);                                                                                 <* 
 *>    DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);                                                                                 <* 
 *>    --rce;  if (x_servo ==  NULL) {                                                                                                         <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                                 <* 
 *>       return rce;                                                                                                                          <* 
 *>    }                                                                                                                                       <* 
 *>    /+---(check accel)-----------------+/                                                                                                   <* 
 *>    yPARSE_top      (s_accel);                                                                                                              <* 
 *>    DEBUG_YKINE_SCRP  yLOG_info    ("s_accel"   , s_accel);                                                                                 <* 
 *>    rc  = ykine_accel_dur (s_accel);                                                                                                        <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);                                                                                      <* 
 *>    /+---(check normal)----------------+/                                                                                                   <* 
 *>    yPARSE_popval (0.0, &b);                                                                                                                <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);                                                                                       <* 
 *>    if (rc < 0)  strlcpy (s_accel, "", LEN_LABEL);                                                                                          <* 
 *>    else         b = -1.0;                                                                                                                  <* 
 *>    /+---(det coordinates)-------------+/                                                                                                   <* 
 *>    rc = ykine_move_savedloc  (x_servo, NULL, &db, NULL, NULL, &yb, &lb);                                                                   <* 
 *>    DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);                                                                           <* 
 *>    if (myKINE.s_from == YKINE_PURE) {                                                                                                      <* 
 *>       if (rc == 0)  rc  = yPARSE_popval   (db, &de);                                                                                       <* 
 *>       if (rc == 0)  rc  = yPARSE_popval   (lb, &le);                                                                                       <* 
 *>       if (rc == 0)  rc  = yPARSE_popval   (yb, &ye);                                                                                       <* 
 *>    } else {                                                                                                                                <* 
 *>       if (rc == 0)  rc  = yPARSE_popfrom  (db, &de);                                                                                       <* 
 *>       if (rc == 0)  rc  = yPARSE_popfrom  (lb, &le);                                                                                       <* 
 *>       if (rc == 0)  rc  = yPARSE_popfrom  (yb, &ye);                                                                                       <* 
 *>    }                                                                                                                                       <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("queue"     , rc);                                                                                      <* 
 *>    --rce;  if (rc <  0) {                                                                                                                  <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                                 <* 
 *>       return rce;                                                                                                                          <* 
 *>    }                                                                                                                                       <* 
 *>    if (de <    0.0)   de  = 360.0 + de;                                                                                                    <* 
 *>    if (de >  360.0)   de  = de - 360.0;                                                                                                    <* 
 *>    DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", de, le);                                                                <* 
 *>    /+---(caclulate end)---------------+/                                                                                                   <* 
 *>    rc = ykine_body_zp2xz      (de, le, &xe, &ze);                                                                                          <* 
 *>    DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xe, ze, ye);                                                    <* 
 *>    /+---(process moves)---------------+/                                                                                                   <* 
 *>    if (b < 0)  {                                                                                                                           <* 
 *>       ykine_accel_zpolar    (s_accel, db, lb, yb, de, le, ye);                                                                             <* 
 *>    } else {                                                                                                                                <* 
 *>       /+---(caclulate polar)-------------+/                                                                                                <* 
 *>       rc = ykine_bogy_xz2zp      (xe, ze, &de, &le);                                                                                       <* 
 *>       DEBUG_YKINE_SCRP  yLOG_complex ("deg/len"   , "%8.2fd, %8.2fl", de, le);                                                             <* 
 *>       /+---(process moves)---------------+/                                                                                                <* 
 *>       s = b * myKINE.s_pace;                                                                                                               <* 
 *>       DEBUG_YKINE_SCRP  yLOG_value   ("s"         , s);                                                                                    <* 
 *>       if (myKINE.s_hidden != 'y')  rc = ykine_move_create (YKINE_MOVE_SERVO, YKINE_NONE, x_servo, myKINE.s_verb, myKINE.s_cline, de, s);   <* 
 *>       DEBUG_YKINE_SCRP  yLOG_value   ("create"    , rc);                                                                                   <* 
 *>       if (rc == 0)  rc = ykine_move_addloc (x_servo, xe, ze, ye);                                                                          <* 
 *>       DEBUG_YKINE_SCRP  yLOG_value   ("addloc"    , rc);                                                                                   <* 
 *>       --rce;  if (rc <  0) {                                                                                                               <* 
 *>          DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                              <* 
 *>          return rce;                                                                                                                       <* 
 *>       }                                                                                                                                    <* 
 *>    }                                                                                                                                       <* 
 *>    /+---(complete)-----------------------+/                                                                                                <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                                                                         <* 
 *>    return 0;                                                                                                                               <* 
 *> }                                                                                                                                          <*/




/*====================------------------------------------====================*/
/*===----                     orientation movements                    ----===*/
/*====================------------------------------------====================*/
static void      o___ORIENT__________________o (void) {;}

char         /*--> check orientation limits --------------[ ------ [ ------ ]-*/
ykine_body_orient_valid (float y, float p, float r)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(test yaw)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sdouble (y);
   --rce;  if (y > 40.0 || y < -40.0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("yaw out of range");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(test pitch)---------------------*/
   DEBUG_YKINE_SCRP   yLOG_sdouble (p);
   --rce;  if (p > 30.0 || p < -30.0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("pitch out of range");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(test roll)----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sdouble (r);
   --rce;  if (r > 30.0 || r < -30.0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("roll out of range");
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
ykine_body_orient_prep  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(check accel)-----------------*/
   yPARSE_top      (s_accel);
   DEBUG_YKINE_SCRP  yLOG_info    ("s_accel"   , s_accel);
   rc  = ykine_accel_dur (s_accel);
   DEBUG_YKINE_SCRP  yLOG_value   ("accel_dur" , rc);
   /*---(check normal)----------------*/
   yPARSE_popval (0.0, &b);
   DEBUG_YKINE_SCRP  yLOG_value   ("b"         , b);
   if (rc < 0) {
      strlcpy (s_accel, "", LEN_LABEL);
      s = b * myKINE.s_pace;
   }
   else         s = b = -1.0;
   DEBUG_YKINE_SCRP  yLOG_complex ("timing"    , "%-6.6s, %8.2fb, %8.2fs", s_accel, b, s);
   rc = 0;
   /*---(get existing)-------------------*/
   s_servo = ykine_servo_pointer (YKINE_BODY, YKINE_YAW  );
   if (rc == 0)  rc = ykine_move_savedloc  (s_servo + 0, NULL, &yb, NULL, NULL, NULL, NULL);
   if (rc == 0)  rc = ykine_move_savedloc  (s_servo + 1, NULL, &pb, NULL, NULL, NULL, NULL);
   if (rc == 0)  rc = ykine_move_savedloc  (s_servo + 2, NULL, &rb, NULL, NULL, NULL, NULL);
   /*---(reporting)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("before"    , "%8.2fy, %8.2fp, %8.2fr", yb, pb, rb);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_body_orient_wrap  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   float       x, z, y, l;
   /*---(reporting)-------------------*/
   DEBUG_YKINE_SCRP  yLOG_complex ("after"     , "%8.2fy, %8.2fp, %8.2fr", ye, pe, re);
   /*---(check limits)----------------*/
   rc = ykine_body_orient_valid (ye, pe, re);
   DEBUG_YKINE_SCRP  yLOG_value   ("limits"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process normal)--------------*/
   if (b < 0)  {
      ykine_accel_orient    (s_accel, yb, pb, rb, ye, pe, re);
   }
   /*---(process accelerated)---------*/
   else {
      ykine_move_create (YKINE_MOVE_SERVO, YKINE_NONE, s_servo + 0, myKINE.s_verb, myKINE.s_cline, ye, s);
      ykine_move_create (YKINE_MOVE_SERVO, YKINE_NONE, s_servo + 1, myKINE.s_verb, myKINE.s_cline, pe, s);
      ykine_move_create (YKINE_MOVE_SERVO, YKINE_NONE, s_servo + 2, myKINE.s_verb, myKINE.s_cline, re, s);
      ykine_body_orient2xyz (ye, pe, re, &x, &z, &y, &l);
      ykine_move_addloc (s_servo + 2, x, z, y);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> convert orientation to dir/tilt -------[ ------ [ ------ ]-*/
ykine_body_pr2dt        (float p, float r, float *d, float *t)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_len       =    0;
   float       x_dir       =    0;
   /*---(calculate)----------------------*/
   x_len = sqrt ((p * p) + (r * r));
   x_dir = RAD2DEG * atan2 (r, p);
   /*---(fix and round)------------------*/
   x_len = round (x_len * 10.0) / 10.0;
   x_dir = round (x_dir * 10.0) / 10.0;
   while (x_dir <= -180.0)   x_dir += 360.0;
   while (x_dir >   180.0)   x_dir -= 360.0;
   /*---(save returns)-------------------*/
   if (d != NULL)  *d = x_dir;
   if (t != NULL)  *t = x_len;
   /*---(complete)-----------------------*/
   return 0;
}

float
ykine_round_val     (float a)
{
   a = round (a * 10.0) / 10.0;
   if (a > -0.1 && a < 0.1)  a = 0.0;
   return a;
}

float
ykine_round_deg     (float a)
{
   a = round (a * 10.0) / 10.0;
   if (a > -0.1 && a < 0.1)  a = 0.0;
   while (a <= -180.0)   a += 360.0;
   while (a >   180.0)   a -= 360.0;
   return a;
}

char         /*--> convert dir/tilt to orientation -------[ ------ [ ------ ]-*/
ykine_body_dt2pr        (float d, float t, float *p, float *r)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_pitch     =    0;
   float       x_roll      =    0;
   /*---(calculate)----------------------*/
   x_pitch =  t * cos (d * DEG2RAD);
   x_roll  =  t * sin (d * DEG2RAD);
   /*---(fix and round)------------------*/
   x_pitch = round (x_pitch * 10.0) / 10.0;
   x_roll  = round (x_roll  * 10.0) / 10.0;
   if (x_pitch > -0.1 && x_pitch < 0.1)  x_pitch = 0.0;
   if (x_roll  > -0.1 && x_roll  < 0.1)  x_roll  = 0.0;
   while (x_pitch <= -180.0)   x_pitch += 360.0;
   while (x_pitch >   180.0)   x_pitch -= 360.0;
   while (x_roll  <= -180.0)   x_roll  += 360.0;
   while (x_roll  >   180.0)   x_roll  -= 360.0;
   /*---(save returns)-------------------*/
   if (p != NULL)  *p = x_pitch;
   if (r != NULL)  *r = x_roll;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> convert orientation into position -----[ ------ [ ------ ]-*/
ykine_body_orient2xyz   (float a_yaw, float a_pitch, float a_roll, float *a_x, float *a_z, float *a_y, float *a_l)
{  /*---(design notes)-------------------*/
   /*  returns highest point on body, so y is always returned positive.
    *  yaw, pitch, and roll ranges must be checked *before* this function.
    */
   /*---(locals)-----------+-----+-----+-*/
   float       h, a, d, da, t, x, z, y, l;
   /*---(convert)------------------------*/
   ykine_body_pr2dt       (a_pitch, a_roll, &d, &t);
   da  = 90.0 - (d + a_yaw);
   /*> printf ("%8.2fy, %8.2fp, %8.2fr, %8.2fd, %8.2ft, %8.2fda\n", a_yaw, a_pitch, a_roll, d, t, da);   <*/
   /*---(vertical)-----------------------*/
   h   = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   y   = ykine_round_val ( h * sin (t  * DEG2RAD));
   /*> printf ("           %8.2fh, %8.2fy\n", h, y);                                  <*/
   /*---(horizontal)---------------------*/
   h   = h * cos (t * DEG2RAD);
   x   = ykine_round_val ( h * cos (da * DEG2RAD));
   z   = ykine_round_val (-h * sin (da * DEG2RAD));
   /*---(length as check)----------------*/
   l   = ykine_round_val (sqrt ((x * x) + (z * z) + (y * y)));
   /*> printf ("           %8.2fh, %8.2fx, %8.2fz, %8.2fl\n", h, x, z, l);            <*/
   /*---(save returns)-------------------*/
   if (a_x != NULL)  *a_x  = x;
   if (a_z != NULL)  *a_z  = z;
   if (a_y != NULL)  *a_y  = y;
   if (a_l != NULL)  *a_l  = l;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_body_orient       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   rc = ykine_body_orient_prep ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update values)------------------*/
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_popval   (yb, &ye);
      if (rc == 0)  rc  = yPARSE_popval   (pb, &pe);
      if (rc == 0)  rc  = yPARSE_popval   (rb, &re);
   } else {
      if (rc == 0)  rc  = yPARSE_popfrom  (yb, &ye);
      if (rc == 0)  rc  = yPARSE_popfrom  (pb, &pe);
      if (rc == 0)  rc  = yPARSE_popfrom  (rb, &re);
   }
   /*---(process moves)---------------*/
   rc = ykine_body_orient_wrap ();
   DEBUG_YKINE_SCRP  yLOG_value   ("wrap"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_body_opolar       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   double      x_dir, x_tilt;
   float       x_pitch, x_roll;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   rc = ykine_body_orient_prep ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to dir/tilt)------------*/
   rc = ykine_body_pr2dt       (pb, rb, &db, &tb);
   DEBUG_YKINE_SCRP  yLOG_complex ("converted" , "%8.2fd, %8.2ft", db, tb);
   /*---(update values)------------------*/
   if (myKINE.s_from == YKINE_PURE) {
      if (rc == 0)  rc  = yPARSE_popval   (yb, &ye);
      if (rc == 0)  rc  = yPARSE_popval   (db, &x_dir);
      if (rc == 0)  rc  = yPARSE_popval   (tb, &x_tilt);
   } else {
      if (rc == 0)  rc  = yPARSE_popfrom  (yb, &ye);
      if (rc == 0)  rc  = yPARSE_popfrom  (db, &x_dir);
      if (rc == 0)  rc  = yPARSE_popfrom  (tb, &x_tilt);
   }
   de = x_dir;
   te = x_tilt;
   DEBUG_YKINE_SCRP  yLOG_complex ("updated"   , "%8.2fy, %8.2fd, %8.2ft", ye, de, te);
   /*---(convert back to pitch/roll)-----*/
   rc = ykine_body_dt2pr        (de, te, &x_pitch, &x_roll);
   pe = x_pitch;
   re = x_roll;
   DEBUG_YKINE_SCRP  yLOG_complex ("converted" , "%8.2fp, %8.2fr", pe, re);
   /*---(process moves)---------------*/
   rc = ykine_body_orient_wrap ();
   DEBUG_YKINE_SCRP  yLOG_value   ("wrap"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



