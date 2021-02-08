#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

LDFLAGS="$LDFLAGS -L/lib/arm-linux-gnueabihf/"

AC_PREREQ([2.69])
AC_INIT(ts7800v2-utils, 1.0, support@embeddedarm.com)
AC_CONFIG_SRCDIR([src/ts7800ctl.c])
#AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.00 foreign no-define])

# Checks for programs.
AC_PROG_CC([arm-linux-gnueabihf-gcc])

# Checks for libraries.
AC_CHECK_LIB([gpiod], [gpiod_line_request_input], [], [AC_MSG_ERROR([libgpiod not found])])
AC_CHECK_LIB([procps], [openproc], [], [AC_MSG_ERROR([libprocps not found])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/ioctl.h termios.h unistd.h], [], [AC_MSG_ERROR([Missing required headers/libraries])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([strtoull])

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 ])
AC_OUTPUT

