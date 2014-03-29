AC_PREREQ([2.65])
LT_PREREQ([2.4.2])

AC_INIT([lll], [1.0.1], [jajm@jajm.org])
LT_INIT()
AM_INIT_AUTOMAKE([-Wall -Werror silent-rules subdir-objects])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Libtool version info
AC_SUBST(LIBTOOL_CUR, 0)
AC_SUBST(LIBTOOL_REV, 0)
AC_SUBST(LIBTOOL_AGE, 0)

# Check for programs
AC_PROG_AWK
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AM_PROG_AR
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_CHECK_HEADERS([stddef.h stdio.h stdlib.h string.h])

AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove])

AC_CONFIG_FILES([
	lll.pc
	Makefile
	src/Makefile
	include/Makefile
	t/Makefile
])

AM_SILENT_RULES

AC_OUTPUT
