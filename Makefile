#================================[[ beg-code ]]================================#



#===(manditory)============================================#
# basename of executable, header, and all code files
NAME_BASE  = yKINE
# additional standard and outside libraries
LIB_STD    = -lm
# all heatherly libraries used, debug versions whenever available
LIB_MINE   = -lySTR_debug -lyPARSE_debug
# directory for production code, no trailing slash
INST_DIR   = 



#===(optional)=============================================#
# extra include directories required
INC_MINE   = 
# utilities generated, separate from main program
NAME_UTIL  = 
# libraries only for the utilities
LIB_UTIL   = 



#===(post-processing)======================================#
# create a rule for...
#install_prep       :
#remove_prep        :



#===(master template)======================================#
include /usr/local/sbin/make_program



#===(post-processing)======================================#
# create a rule for...
#install_post       :
#remove_post        :



#================================[[ end-code ]]================================#


#*============================----beg-of-source---============================*#

##===(current variables)=================================================================================================================================================#
#BASE    = yKINE
#DEBUG   = ${BASE}_debug
#UNIT    = ${BASE}_unit
#HDIR    = /home/system/yKINE.hexapod_kinematics
#IDIR    = 
#MNUM    = 3
#MDIR    = /usr/share/man/man${MNUM}
#MBASE   = ${BASE}.${MNUM}
#
##===(compilier variables)===============================================================================================================================================#
## must have "-x c" on gcc line so stripped files work with alternate extensions
#COMP    = gcc -c -std=gnu89 -x c -g -pg -Wall -Wextra
#INCS    = -I/usr/local/include 
#
##===(linker options)====================================================================================================================================================================================================================================================================================#
##------   (0)-------------- (1)-------------- (2)-------------- (3)-------------- (4)-------------- (5)-------------- (6)-------------- (7)-------------- (8)-------------- (9)-------------- (A)-------------- (B)-------------- (C)-------------- (D)-------------- (E)-------------- (F)--------------
#LINK    = gcc
#LIBDIR  = -L/usr/local/lib
#LIBS    = ${LIBDIR}         -lm               -lySTR            -lyURG
#LIBD    = ${LIBDIR}         -lm               -lySTR_debug      -lyURG_debug      -lyLOG
#LIBU    = ${LIBDIR}         -lm               -lySTR_debug      -lyURG_debug      -lyLOG            -lyUNIT           -lyVAR
#
##===(file lists)============================================================================================================================================================================#
##------   (0)-------------- (1)-------------- (2)-------------- (3)-------------- (4)-------------- (5)-------------- (6)-------------- (7)-------------- (8)-------------- (9)-------------- (A)-------------- (B)-------------- (C)-------------- (D)-------------- (5)--------------
#HEADS   = ${BASE}.h         ${BASE}_priv.h
#OBJS    = ${BASE}_base.os   ${BASE}_data.os   ${BASE}_calc.os   ${BASE}_scrp.os   ${BASE}_move.os   ${BASE}_phys.os
#OBJD    = ${BASE}_base.o    ${BASE}_data.o    ${BASE}_calc.o    ${BASE}_scrp.o    ${BASE}_move.o    ${BASE}_phys.o
#OBJU    = ${OBJD}.o         ${UNIT}.o
#
##*---(make variables)-----------------*#
#COPY    = cp -f
#CLEAN   = rm -f
#PRINT   = @printf
#STRIP   = @grep -v -e " DEBUG_" -e " yLOG_" 
#
#
#
##*---(executables)--------------------*#
#all                : ${DEBUG} ${BASE} ${UNIT}
#
#${BASE}            : ${OBJD}
#	${LINK}  -shared -Wl,-soname,lib${BASE}.so.1   ${LIBS}  -o lib${BASE}.so.1.0   ${OBJS}
#	ar       rcs  lib${BASE}.a   ${OBJS}
#
#${DEBUG}           : ${OBJD}
#	${LINK}  -shared -Wl,-soname,lib${DEBUG}.so.1  ${LIBD}  -o lib${DEBUG}.so.1.0  ${OBJD}
#	ar       rcs  lib${DEBUG}.a  ${OBJD}
#
#${UNIT}            : ${OBJU}
#	${LINK}  -o ${UNIT}        ${OBJU}   ${LIBU}
#
#
#
##*---(components)---------------------*#
#${BASE}_base.o     : ${HEADS}       ${BASE}_base.c
#	${COMP}  -fPIC  ${BASE}_base.c                           ${INC}
#	${STRIP}        ${BASE}_base.c      > ${BASE}_base.cs
#	${COMP}  -fPIC  ${BASE}_base.cs    -o ${BASE}_base.os    ${INC}
#
#${BASE}_data.o     : ${HEADS}       ${BASE}_data.c
#	${COMP}  -fPIC  ${BASE}_data.c                           ${INC}
#	${STRIP}        ${BASE}_data.c      > ${BASE}_data.cs
#	${COMP}  -fPIC  ${BASE}_data.cs    -o ${BASE}_data.os    ${INC}
#
#${BASE}_calc.o     : ${HEADS}       ${BASE}_calc.c
#	${COMP}  -fPIC  ${BASE}_calc.c                           ${INC}
#	${STRIP}        ${BASE}_calc.c      > ${BASE}_calc.cs
#	${COMP}  -fPIC  ${BASE}_calc.cs    -o ${BASE}_calc.os    ${INC}
#
#${BASE}_scrp.o     : ${HEADS}       ${BASE}_scrp.c
#	${COMP}  -fPIC  ${BASE}_scrp.c                           ${INC}
#	${STRIP}        ${BASE}_scrp.c      > ${BASE}_scrp.cs
#	${COMP}  -fPIC  ${BASE}_scrp.cs    -o ${BASE}_scrp.os    ${INC}
#
#${BASE}_move.o     : ${HEADS}       ${BASE}_move.c
#	${COMP}  -fPIC  ${BASE}_move.c                           ${INC}
#	${STRIP}        ${BASE}_move.c      > ${BASE}_move.cs
#	${COMP}  -fPIC  ${BASE}_move.cs    -o ${BASE}_move.os    ${INC}
#
#${BASE}_phys.o     : ${HEADS}       ${BASE}_phys.c
#	${COMP}  -fPIC  ${BASE}_phys.c                           ${INC}
#	${STRIP}        ${BASE}_phys.c      > ${BASE}_phys.cs
#	${COMP}  -fPIC  ${BASE}_phys.cs    -o ${BASE}_phys.os    ${INC}
#
#${UNIT}.o          : ${HEADS} ${BASE}.unit
#	koios    ${BASE}
#	${COMP}  ${UNIT}.c  ${INCS}
#
#
#
##*---(housecleaning)------------------*#
#clean              :
#	${PRINT}  "\n--------------------------------------\n"
#	${PRINT}  "cleaning out local object, backup, and temp files\n"
#	${CLEAN} lib${BASE}.so.1
#	${CLEAN} lib${BASE}.so.1.0
#	${CLEAN} lib${DEBUG}.so.1
#	${CLEAN} lib${DEBUG}.so.1.0
#	${CLEAN} *.o
#	${CLEAN} *.cs
#	${CLEAN} *.os
#	${CLEAN} *~
#	${CLEAN} temp*
#	${CLEAN} ${BASE}_unit.c
#
#bigclean           :
#	${PRINT}  "\n--------------------------------------\n"
#	${PRINT}  "clean out local swap files\n"
#	${CLEAN}  '.'*.sw?
#
#remove             :
#	${PRINT}  "\n--------------------------------------\n"
#	${PRINT}  "remove ${BASE} from production\n"
#	_lib      -d  ${BASE}
#	_lib      -d  ${DEBUG}
#	ldconfig
#
#install            :
#	#---(production version)--------------#
#	sha1sum   lib${BASE}.so.1.0
#	_lib      -s ${BASE}
#	ldconfig
#	sha1sum   lib${BASE}.a
#	_lib      -a ${BASE}
#	ldconfig
#	#---(debug version)-------------------#
#	sha1sum   lib${DEBUG}.so.1.0
#	_lib      -S ${DEBUG}
#	ldconfig
#	sha1sum   lib${DEBUG}.a
#	_lib      -A ${DEBUG}
#	ldconfig
#	#---(documentation)-------------------#
#	rm -f     ${MDIR}/${MBASE}.bz2
#	cp -f     ${BASE}.3    ${MDIR}
#	bzip2     ${MDIR}/${MBASE}
#	chmod     0644  ${MDIR}/${MBASE}.bz2
