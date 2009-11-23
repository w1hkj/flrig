ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src

if HAVE_FLUID
flgen:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) $@)
endif

if DARWIN
appbundle:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) $@)
endif

if HAVE_NSIS
nsisinst:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) $@)
endif

#EXTRA_DIST = build-aux/config.rpath
