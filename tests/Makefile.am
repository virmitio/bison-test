## Process this file with automake to create Makefile.in.

## Makefile for Bison testsuite.

## Copyright (C) 2000-2012 Free Software Foundation, Inc.

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

EXTRA_DIST = $(TESTSUITE_AT) testsuite package.m4

DISTCLEANFILES       = atconfig $(check_SCRIPTS)
MAINTAINERCLEANFILES = Makefile.in $(TESTSUITE)

## ------------ ##
## package.m4.  ##
## ------------ ##

$(srcdir)/package.m4: $(top_srcdir)/configure
	$(AM_V_GEN) { \
	  echo '# Signature of the current package.'; \
	  echo 'm4_define([AT_PACKAGE_NAME],      [$(PACKAGE_NAME)])'; \
	  echo 'm4_define([AT_PACKAGE_TARNAME],   [$(PACKAGE_TARNAME)])'; \
	  echo 'm4_define([AT_PACKAGE_VERSION],   [$(PACKAGE_VERSION)])'; \
	  echo 'm4_define([AT_PACKAGE_STRING],    [$(PACKAGE_STRING)])'; \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT], [$(PACKAGE_BUGREPORT)])'; \
	} >$@.tmp
	$(AM_V_at) mv $@.tmp $@

## ------------ ##
## Test suite.  ##
## ------------ ##

TESTSUITE_AT = \
	local.at \
	testsuite.at \
	input.at named-refs.at \
	output.at skeletons.at sets.at reduce.at \
	synclines.at headers.at actions.at conflicts.at \
	calc.at \
        torture.at existing.at regression.at \
        c++.at \
        java.at \
	cxx-type.at glr-regression.at \
	push.at

TESTSUITE = $(srcdir)/testsuite

AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): $(srcdir)/package.m4 $(TESTSUITE_AT)
	$(AM_V_GEN) $(AUTOTEST) -I $(srcdir) testsuite.at -o $@.tmp
	$(AM_V_at) mv $@.tmp $@

atconfig: $(top_builddir)/config.status
	cd $(top_builddir) && ./config.status tests/$@

clean-local:
	test ! -f $(TESTSUITE) || $(SHELL) $(TESTSUITE) --clean

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) $(TESTSUITE) $(TESTSUITEFLAGS)

check_SCRIPTS = bison

# Run the test suite on the *installed* tree.
installcheck-local:
	$(TESTSUITE) AUTOTEST_PATH="$(bindir)" $(TESTSUITEFLAGS)

# Be real mean with it.
.PHONY: maintainer-check-g++
maintainer-check-g++: $(TESTSUITE)
	$(TESTSUITE) $(TESTSUITEFLAGS) --compile-c-with-cxx

.PHONY: maintainer-check-posix
maintainer-check-posix: $(TESTSUITE)
	$(TESTSUITE) $(TESTSUITEFLAGS) POSIXLY_CORRECT=1 _POSIX2_VERSION=200112

.PHONY: maintainer-check-valgrind
maintainer-check-valgrind: $(TESTSUITE)
	test -z '$(VALGRIND)' ||					\
	  $(TESTSUITE) $(TESTSUITEFLAGS)				\
	  PREBISON='$(VALGRIND) -q' PREPARSER='$(VALGRIND) -q'		\
	  VALGRIND_OPTS='--leak-check=full --show-reachable=yes'

.PHONY: maintainer-check
maintainer-check: maintainer-check-posix maintainer-check-valgrind maintainer-check-g++

.PHONY: maintainer-push-check
maintainer-push-check:
	BISON_USE_PUSH_FOR_PULL=1 $(MAKE) $(AM_MAKEFLAGS) maintainer-check

.PHONY: maintainer-xml-check
maintainer-xml-check:
	BISON_TEST_XML=1 $(MAKE) $(AM_MAKEFLAGS) maintainer-check

.PHONY: maintainer-release-check
maintainer-release-check: maintainer-check maintainer-push-check maintainer-xml-check
