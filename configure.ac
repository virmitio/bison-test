# Configure template for GNU Bison.                   -*-Autoconf-*-
#
# Copyright (C) 2001-2012 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# In order for some versions of Sun Studio to compile our C++ test cases
# correctly, we need Autoconf 2.64 or better to handle the restrict
# keyword in at least string.h from gnulib.  We need Autoconf 2.68 or
# better to avoid a typo in the `configure --help' entry for the YACC
# environment variable.
AC_PREREQ([2.68])
m4_pattern_forbid([^gl_[A-Z]])

AC_INIT([GNU Bison],
        m4_esyscmd([build-aux/git-version-gen .tarball-version]),
        [bug-bison@gnu.org])
AC_SUBST([PACKAGE_COPYRIGHT_YEAR], [2012])
AC_DEFINE_UNQUOTED([PACKAGE_COPYRIGHT_YEAR], [$PACKAGE_COPYRIGHT_YEAR],
                   [The copyright year for this package])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# Automake 1.10.3 and 1.11.1 fix a security flaw discussed here:
#
#   http://thread.gmane.org/gmane.comp.sysutils.autotools.announce/131
#
# To avoid 1.11, we make 1.11.1 the minimum version.
#
# We want gnits strictness only when rolling a stable release.  For
# release candidates, we use version strings like 2.4.3_rc1, but gnits
# doesn't like that, so we let the underscore disable gnits.  Between
# releases, we want to be able run make dist without being required to
# add a bogus NEWS entry.  In that case, the version string
# automatically contains a dash, which we also let disable gnits.
AM_INIT_AUTOMAKE([1.11.1 dist-xz silent-rules]
                 m4_bmatch(m4_defn([AC_PACKAGE_VERSION]), [[-_]],
                           [gnu], [gnits]))
AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([lib/config.h:lib/config.in.h])

# Checks for the compiler.
AC_PROG_CC_STDC
AC_PROG_CXX

# Gnulib (early checks).
gl_EARLY

AC_ARG_ENABLE([gcc-warnings],
[  --enable-gcc-warnings   turn on lots of GCC warnings (not recommended)],
[case $enable_gcc_warnings in
   yes|no) ;;
   *)  AC_MSG_ERROR([invalid value for --gcc-warnings: $enable_gcc_warnings]);;
 esac],
              [enable_gcc_warnings=no])
if test "$enable_gcc_warnings" = yes; then
  warn_common='-Wall -Wextra -Wno-sign-compare -Wcast-align
    -Wcast-qual -Wformat -Wpointer-arith -Wwrite-strings'
  warn_c='-Wbad-function-cast -Wmissing-declarations -Wmissing-prototypes
    -Wshadow -Wstrict-prototypes'
  warn_cxx='-Wnoexcept'
  AC_LANG_PUSH([C])
  for i in $warn_common $warn_c;
  do
    gl_WARN_ADD([$i], [WARN_CFLAGS])
  done
  gl_WARN_ADD([-Werror], [WERROR_CFLAGS])
  # Warnings for the test suite only.
  gl_WARN_ADD([-Wundef], [WARN_CFLAGS_TEST])
  gl_WARN_ADD([-pedantic], [WARN_CFLAGS_TEST])
  AC_LANG_POP([C])

  AC_LANG_PUSH([C++])
  for i in $warn_common $warn_cxx;
  do
    gl_WARN_ADD([$i], [WARN_CXXFLAGS])
  done
  gl_WARN_ADD([ -Wzero-as-null-pointer-constant], [WARN_CXXFLAGS],
              [AC_LANG_PROGRAM([], [nullptr])])
  gl_WARN_ADD([-Werror], [WERROR_CXXFLAGS])
  # Warnings for the test suite only.
  gl_WARN_ADD([-Wundef], [WARN_CXXFLAGS_TEST])
  gl_WARN_ADD([-pedantic], [WARN_CXXFLAGS_TEST])
  AC_LANG_POP([C++])
fi

BISON_TEST_FOR_WORKING_C_COMPILER
BISON_TEST_FOR_WORKING_CXX_COMPILER

AC_ARG_ENABLE([yacc],
  [AC_HELP_STRING([--disable-yacc],
     [do not build a yacc command or an -ly library])],
  , [enable_yacc=yes])
case $enable_yacc in
yes)
  YACC_SCRIPT=yacc
  YACC_LIBRARY=liby.a;;
*)
  YACC_SCRIPT=
  YACC_LIBRARY=;;
esac
AC_SUBST([YACC_SCRIPT])
AC_SUBST([YACC_LIBRARY])

# Checks for programs.
AC_PROG_LEX
$LEX_IS_FLEX || AC_MSG_ERROR([Flex is required])
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_GNU_M4
AC_DEFINE_UNQUOTED([M4], ["$M4"], [Define to the GNU M4 executable name.])
AC_DEFINE_UNQUOTED([M4_GNU_OPTION], ["$M4_GNU"], [Define to "-g" if GNU M4
supports -g, otherwise to "".])
AM_MISSING_PROG([HELP2MAN], [help2man])
AC_PATH_PROG([XSLTPROC], [xsltproc])
AC_SUBST([XSLTPROC])

# Checks for header files.
AC_CHECK_HEADERS_ONCE([locale.h])

# Checks for compiler characteristics.
AC_C_INLINE

# Gnulib (later checks).  Putting them here rather than right after
# gl_EARLY avoids some redundant checks.
gl_INIT

# Checks for library functions.
AC_CHECK_FUNCS_ONCE([setlocale])
AM_WITH_DMALLOC
BISON_PREREQ_TIMEVAR

# Gettext.
# We use gnulib, which is only guaranteed to work properly with the
# latest Gettext.
AM_GNU_GETTEXT([external], [need-ngettext])
AM_GNU_GETTEXT_VERSION([0.18])
BISON_I18N

# Internationalized parsers.
AC_CONFIG_FILES([runtime-po/Makefile.in])
# Autoconf macros for packages using internationalized parsers.
aclocaldir='${datadir}/aclocal'
AC_SUBST([aclocaldir])

# Create the benchmark script.
AC_CONFIG_FILES([etc/bench.pl], [chmod +x etc/bench.pl])

# Initialize the test suite.
AC_CONFIG_TESTDIR(tests)
AC_CONFIG_FILES([tests/Makefile tests/atlocal])
AC_CONFIG_FILES([tests/bison], [chmod +x tests/bison])
AC_CHECK_PROGS([VALGRIND], [valgrind])
AM_MISSING_PROG([AUTOM4TE], [autom4te])
# Needed by tests/atlocal.in.
AC_SUBST([GCC])

gt_JAVACOMP([1.3])
gt_JAVAEXEC

AC_CONFIG_FILES([Makefile
		 build-aux/Makefile
		 po/Makefile.in
		 data/Makefile
		 etc/Makefile
		 examples/Makefile
		    examples/calc++/Makefile
		 lib/Makefile src/Makefile
		 doc/Makefile
		   doc/yacc.1])
AC_OUTPUT
