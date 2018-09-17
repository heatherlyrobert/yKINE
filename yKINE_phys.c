/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


char
yKINE_phys_flat    (char a_meth, double a_sec, double *a_lowest, int *a_count)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_legnum    = 0;
   int         x_count     = 0;
   double      x_lowest    = 0.0;
   double      x_forgive   = 2.0;
   tSEG       *x_leg       = NULL;
   /*---(defense)------------------------*/
   if (a_sec < 0.0     )            return -1;
   if (a_sec > myKINE.scrp_len)  return -2;
   /*---(head through legs)--------------*/
   for (i = 0; i < YKINE_MAX_LEGS; ++i) {
      /*---(test and set)----------------*/
      if (a_meth == YKINE_FK)  x_leg = ((tSEG *) fk) + (i * YKINE_MAX_SEGS);
      if (a_meth == YKINE_IK)  x_leg = ((tSEG *) ik) + (i * YKINE_MAX_SEGS);
      if (a_meth == YKINE_GK)  x_leg = ((tSEG *) gk) + (i * YKINE_MAX_SEGS);
      /*---(establish position)----------*/
      x_legnum = i * 3;
      if (x_legnum + 2 >= g_nservo) break;
      /*> printf ("   leg = %d, servo = %d\n", i, x_legnum);                          <*/
      yKINE_exact_leg   (a_sec, i);
      yKINE_forward     (i, g_servo_info [x_legnum + 0].deg, g_servo_info [x_legnum + 1].deg, g_servo_info [x_legnum + 2].deg);
      /*---(check for new lowest)--------*/
      /*> printf ("   TIBI.y  = %8.1lf, x_lowest = %8.1lf, x_count = %2d, x_forgive = %8.1lf\n", x_leg [YKINE_TIBI].y, x_lowest, x_count, x_forgive);   <*/
      if (x_leg [YKINE_TIBI].cy  < x_lowest - x_forgive) {
         x_lowest = x_leg [YKINE_TIBI].cy;
         x_count  = 1;
         continue;
      }
      /*---(check for above lowest)------*/
      if (x_leg [YKINE_TIBI].cy  > x_lowest + x_forgive) {
         continue;
      }
      /*---(mark another lowest)---------*/
      ++x_count;
   }
   /*> printf ("   done with loop\n");                                                <*/
   /*> printf ("   x_lowest = %8.1lf, x_count = %2d, x_forgive = %8.1lf\n", x_lowest, x_count, x_forgive);   <*/
   /*---(update lowest)------------------*/
   if (a_lowest != NULL)  *a_lowest = x_lowest;
   if (a_count  != NULL)  *a_count  = x_count;
   /*---(complete)-----------------------*/
   return 0;
}

/*============================----end-of-source---============================*/
