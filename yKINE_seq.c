/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


tMOVE      *s_gait_begin  [YKINE_MAX_SERVO];



/*> char                                                                                                            <* 
 *> ykine_step_raise       (char a_verb)                                                                            <* 
 *> {                                                                                                               <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                     <* 
 *>    char        rce         =  -10;                                                                              <* 
 *>    char        rc          =    0;                                                                              <* 
 *>    char        x_accel     [LEN_LABEL] = "";                                                                    <* 
 *>    float       xe, ze, ye;                                                                                      <* 
 *>    /+---(header)-------------------------+/                                                                     <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                                              <* 
 *>    /+---(defense)------------------------+/                                                                     <* 
 *>    DEBUG_YKINE_SCRP   yLOG_value   ("b"         , myKINE.b);                                                    <* 
 *>    --rce;  if (myKINE.b >= 0.0)  {                                                                              <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                      <* 
 *>       return rce;                                                                                               <* 
 *>    }                                                                                                            <* 
 *>    --rce;  switch (a_verb) {                                                                                    <* 
 *>    case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI : case YKINE_FK :                              <* 
 *>       DEBUG_YKINE_SCRP   yLOG_note    ("no raise required body/fk moves");                                      <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                      <* 
 *>       return rce;                                                                                               <* 
 *>    }                                                                                                            <* 
 *>    DEBUG_YKINE_SCRP   yLOG_value   ("step_s"    , myKINE.step_s);                                               <* 
 *>    --rce;  if (myKINE.step_s <= 0) {                                                                            <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                      <* 
 *>       return rce;                                                                                               <* 
 *>    }                                                                                                            <* 
 *>    /+---(save endpoint)---------------+/                                                                        <* 
 *>    xe         = myKINE.xe;                                                                                      <* 
 *>    ze         = myKINE.ze;                                                                                      <* 
 *>    s_ye       = myKINE.ye;                                                                                      <* 
 *>    /+---(set for raise)---------------+/                                                                        <* 
 *>    myKINE.xe  = myKINE.xb;                                                                                      <* 
 *>    myKINE.ze  = myKINE.zb;                                                                                      <* 
 *>    /+---(increase height)-------------+/                                                                        <* 
 *>    switch (s_cshape) {                                                                                          <* 
 *>    case YKINE_SSQUARE : case YKINE_RSQUARE :                                                                    <* 
 *>       DEBUG_YKINE_SCRP  yLOG_note    ("square handler");                                                        <* 
 *>       if (myKINE.yb >= myKINE.ye)  myKINE.ye = myKINE.yb + myKINE.step_h;                                       <* 
 *>       else                         myKINE.ye = myKINE.ye + myKINE.step_h;                                       <* 
 *>       ye         = myKINE.ye;                                                                                   <* 
 *>       break;                                                                                                    <* 
 *>    case YKINE_SDIRECT : case YKINE_RDIRECT :                                                                    <* 
 *>       DEBUG_YKINE_SCRP  yLOG_note    ("direct handler");                                                        <* 
 *>       myKINE.ye = myKINE.yb + myKINE.step_h;                                                                    <* 
 *>       ye        = s_ye      + myKINE.step_h;                                                                    <* 
 *>       break;                                                                                                    <* 
 *>       break;                                                                                                    <* 
 *>    }                                                                                                            <* 
 *>    /+---(inverse kinematics)----------+/                                                                        <* 
 *>    rc = yKINE_inverse (myKINE.leg, myKINE.xe, myKINE.ze, myKINE.ye);                                            <* 
 *>    DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);                                                           <* 
 *>    rc = yKINE_angles  (myKINE.leg, YKINE_IK, &myKINE.ce, &myKINE.fe, &myKINE.pe, &myKINE.te);                   <* 
 *>    /+---(create moves)-------------------+/                                                                     <* 
 *>    DEBUG_YKINE_SCRP   yLOG_complex ("beg"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xb, myKINE.zb, myKINE.yb);   <* 
 *>    DEBUG_YKINE_SCRP   yLOG_complex ("end"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xe, myKINE.ze, myKINE.ye);   <* 
 *>    DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);   <* 
 *>    ykine_accel_append (YKINE_IK, 'r', myKINE.a_raise);                                                          <* 
 *>    /+---(put endpoint back)-----------+/                                                                        <* 
 *>    myKINE.xe = xe;                                                                                              <* 
 *>    myKINE.ze = ze;                                                                                              <* 
 *>    myKINE.yb = myKINE.ye;                                                                                       <* 
 *>    myKINE.ye = ye;                                                                                              <* 
 *>    /+---(complete)-----------------------+/                                                                     <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                                              <* 
 *>    return 0;                                                                                                    <* 
 *> }                                                                                                               <*/



/*> char         /+--> prepare to read a gait ----------------[ ------ [ ------ ]-+/                                                              <* 
 *> ykine_seq_begin_STUFF   (char *a_list)                                                                                                        <* 
 *> {                                                                                                                                             <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                                   <* 
 *>    char        rce         =  -10;                                                                                                            <* 
 *>    char        rc          =    0;                                                                                                            <* 
 *>    int         i, j        = 0;                                                                                                               <* 
 *>    char       *p           = NULL;                                                                                                            <* 
 *>    char        x_leg       = 0;                                                                                                               <* 
 *>                                                                                                                                               <* 
 *>    int         x_servo     = 0;                                                                                                               <* 
 *>    double      x_deg       = 0.0;                                                                                                             <* 
 *>    double      x_xpos      = 0.0;                                                                                                             <* 
 *>    double      x_zpos      = 0.0;                                                                                                             <* 
 *>    double      x_ypos      = 0.0;                                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                                   <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                                                                            <* 
 *>    /+---(mark servers)----------------+/                                                                                                      <* 
 *>    DEBUG_YKINE_SCRP   yLOG_point   ("a_list"    , a_list);                                                                                    <* 
 *>    if (a_list == NULL) {                                                                                                                      <* 
 *>       DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                                                                                    <* 
 *>       return rce;                                                                                                                             <* 
 *>    }                                                                                                                                          <* 
 *>    DEBUG_YKINE_SCRP   yLOG_info    ("a_list"    , a_list);                                                                                    <* 
 *>    /+---(process)------------------------+/                                                                                                   <* 
 *>    for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {                                                                                         <* 
 *>       /+---(filter)----------------------+/                                                                                                   <* 
 *>       rc = yKINE_servo_index (x_leg, YKINE_TIBI);                                                                                             <* 
 *>       if (rc < 0)                     continue;                                                                                               <* 
 *>       if (myKINE.servos [rc] == '_')  continue;                                                                                               <* 
 *>                                                                                                                                               <* 
 *>                                                                                                                                               <* 
 *>                                                                                                                                               <* 
 *>       for (j = 0; j <= 2; ++j) {                                                                                                              <* 
 *>          /+---(filter uninvolved servos)----+/                                                                                                <* 
 *>          x_servo = (i - 2) + j;                                                                                                               <* 
 *>          s_gait_begin [x_servo] = NULL;                                                                                                       <* 
 *>          /+---(get last position)-----------+/                                                                                                <* 
 *>          DEBUG_YKINE_SCRP   yLOG_value    ("servo"     , x_servo);                                                                            <* 
 *>          DEBUG_YKINE_SCRP   yLOG_info     ("name"      , g_servo_info [x_servo].label);                                                       <* 
 *>          yKINE_move_tail     (x_servo, NULL, &x_deg);                                                                                         <* 
 *>          ykine_move_curdata  (&x_xpos, &x_zpos, &x_ypos);                                                                                     <* 
 *>          DEBUG_YKINE_SCRP   yLOG_double   ("last deg"  , x_deg);                                                                              <* 
 *>          /+---(write header note)-----------+/                                                                                                <* 
 *>          ykine_move_create (g_servo_info + x_servo, YKINE_NOTE , YKINE_NONE, myKINE.s_tline, "incipio"      , '-', YKINE_NONE, 0.0  , 0.0);   <* 
 *>          /+---(write header placeholders)---+/                                                                                                <* 
 *>          ykine_move_add_pure (g_servo_info + x_servo, x_xpos, x_zpos, x_ypos);                                                                  <* 
 *>          ykine_move_create (g_servo_info + x_servo, YKINE_WAIT , YKINE_NONE, myKINE.s_tline, "wait for turn", '-', YKINE_NONE, x_deg, 3.0);   <* 
 *>          ykine_move_add_pure (g_servo_info + x_servo, x_xpos, x_zpos, x_ypos);                                                                  <* 
 *>          ykine_move_create (g_servo_info + x_servo, YKINE_SERVO, YKINE_NONE, myKINE.s_tline, "up to neutral", '-', YKINE_NONE, 0.0  , 0.5);   <* 
 *>          ykine_move_create (g_servo_info + x_servo, YKINE_SERVO, YKINE_NONE, myKINE.s_tline, "step to first", '-', YKINE_NONE, 0.0  , 0.5);   <* 
 *>          ykine_move_create (g_servo_info + x_servo, YKINE_WAIT , YKINE_NONE, myKINE.s_tline, "wait for legs", '-', YKINE_NONE, 0.0  , 3.0);   <* 
 *>          /+---(save end of header)----------+/                                                                                                <* 
 *>          DEBUG_YKINE_SCRP   yLOG_point    ("saved"     , g_servo_info [x_servo].tail);                                                        <* 
 *>          s_gait_begin [x_servo] = g_servo_info [x_servo].tail;                                                                                <* 
 *>          /+---(done)------------------------+/                                                                                                <* 
 *>       }                                                                                                                                       <* 
 *>    }                                                                                                                                          <* 
 *>    /+---(complete)-----------------------+/                                                                                                   <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                                                                            <* 
 *>    return 0;                                                                                                                                  <* 
 *> }                                                                                                                                             <*/
