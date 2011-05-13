# Copyright (c) 2008 Dave Freese, W1HKJ (w1hkj AT w1hkj DOT com)

bin_PROGRAMS = flrig

flrig_CPPFLAGS = @FLRIG_BUILD_CPPFLAGS@
flrig_CXXFLAGS = @FLRIG_BUILD_CXXFLAGS@
flrig_CFLAGS = $(flrig_CXXFLAGS)
flrig_LDFLAGS = @FLRIG_BUILD_LDFLAGS@
flrig_LDADD = @FLRIG_BUILD_LDADD@

MINGW32_SRC = \
	flrig-rc.rc \
	include/flrigrc.h \
	mingw.c \
	include/compat.h \
	include/compat-mingw.h


# We distribute these but do not always compile them
EXTRA_flrig_SOURCES = $(FLRIG_WIN32_RES_SRC) $(MINGW32_SRC)

flrig_SOURCES =

if MINGW32
if HAVE_WINDRES
  flrig_SOURCES += $(MINGW32_SRC)
endif
endif

########################################################################


FLRIG_VERSION_MAJOR = @FLRIG_VERSION_MAJOR@
FLRIG_VERSION_MINOR = @FLRIG_VERSION_MINOR@
FLRIG_VERSION_PATCH = @FLRIG_VERSION_PATCH@
FLRIG_VERSION = @FLRIG_VERSION@

.EXPORT_ALL_VARIABLES: nsisinst appbundle

# Sources that are generated,
BUILT_SOURCES =
# not distributed,
nodist_flrig_SOURCES = $(BUILT_SOURCES)
# and deleted by the clean targets
CLEANFILES = $(BUILT_SOURCES)
CLEAN_LOCAL =

if MINGW32
if HAVE_WINDRES
.rc.o:
	$(WINDRES) -I$(srcdir)/include -I$(srcdir)/../data/win32 $< -O coff $@
endif
endif

install-data-local:
	if test -f $(srcdir)/../data/flrig.xpm; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/pixmaps; \
	    $(INSTALL_DATA) $(srcdir)/../data/flrig.xpm $(DESTDIR)/$(datadir)/pixmaps; \
	fi
	if test -f $(srcdir)/../data/flrig.desktop; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/applications; \
	    $(INSTALL_DATA) $(srcdir)/../data/flrig.desktop $(DESTDIR)/$(datadir)/applications; \
	fi

uninstall-local:
	rm -f $(DESTDIR)/$(datadir)/pixmaps/flrig.xpm
	rm -f $(DESTDIR)/$(datadir)/applications/flrig.desktop

FLRIG_FL_SRC = rigpanel.fl

if HAVE_FLUID
flgen: $(FLRIG_FL_SRC)
	(cd $(srcdir)/include; \
	for f in $(FLRIG_FL_SRC); do \
	    c=$${f%.fl}.cxx; \
		h=$${f%.fl}.h; \
		h=$${h##*/}; \
	    $(FLUID) -c -o ../$$c -h $$h ../$$f; \
	done)
endif


INSTALLER_FILE = flrig-$(FLRIG_VERSION)_setup.exe
APPBUNDLE=flrig-$(FLRIG_VERSION)
APPBUNDLE_NOLIBS=$(APPBUNDLE)-nolibs

if HAVE_NSIS
nsisinst: $(bin_PROGRAMS)
	sh $(srcdir)/../scripts/mknsisinst.sh "$(srcdir)/../data" .
    CLEANFILES += $(INSTALLER_FILE)
endif

if DARWIN
appbundle: $(bin_PROGRAMS)
	sh $(srcdir)/../scripts/mkappbundle.sh "$(srcdir)/../data" .
    CLEAN_LOCAL += $(APPBUNDLE_NOLIBS) $(APPBUNDLE) $(APPBUNDLE)*.dmg
endif

#if HAVE_ASCIIDOC
#$(builddir)/../doc/guide.html: $(builddir)/../doc/guide.txt
#	$(MAKE) -C $(builddir)/../doc $(AM_MAKEFLAGS) guide.html

#$(builddir)/dialogs/guide.cxx: $(builddir)/../doc/guide.html
#	mkdir -p $(builddir)/dialogs
#	sed 's/"/\\"/g; s/$$/\\n\\/g; 1 s/.*/const char* szBeginner = "&/; $$ s/.*/&n";/' $< > $@
#dialogs/htmlstrings.cxx: $(builddir)/dialogs/guide.cxx

#distclean-local:
#	@if test "$(builddir)" != "$(srcdir)"; then \
#	    rm -f $(builddir)/dialogs/guide.cxx; \
#        fi
#endif

clean-local:
	-rm -rf $(CLEAN_LOCAL)

# Sources that we build. It is OK to have headers here.
flrig_SOURCES += \
	combo.cxx \
	debug.cxx \
	dialogs.cxx \
	Fl_SigBar.cxx \
	flinput2.cxx \
	pixmaps.cxx \
	icons.cxx \
	flslider2.cxx \
	font_browser.cxx \
	FreqControl.cxx \
	images.cxx \
	K3_ui.cxx \
	ptt.cxx \
	rig.cxx \
	rig_io.cxx \
	rigpanel.cxx \
	rigbase.cxx \
	rigs/FT100D.cxx \
	rigs/FT450.cxx \
	rigs/FT767.cxx \
	rigs/FT817.cxx \
	rigs/FT847.cxx \
	rigs/FT857D.cxx \
	rigs/FT890.cxx \
	rigs/FT920.cxx \
	rigs/FT950.cxx \
	rigs/FT1000.cxx \
	rigs/FT1000MP.cxx \
	rigs/FT2000.cxx \
	rigs/ICbase.cxx \
	rigs/IC703.cxx \
	rigs/IC706MKIIG.cxx \
	rigs/IC718.cxx \
	rigs/IC728.cxx \
	rigs/IC735.cxx \
	rigs/IC746.cxx \
	rigs/IC756.cxx \
	rigs/IC756PRO2.cxx \
	rigs/IC756PRO3.cxx \
	rigs/IC910.cxx \
	rigs/IC7000.cxx \
	rigs/IC7200.cxx \
	rigs/IC7600.cxx \
	rigs/IC7700.cxx \
	rigs/K2.cxx \
	rigs/K3.cxx \
	rigs/RAY152.cxx \
	rigs/TS140.cxx \
	rigs/TS450S.cxx \
	rigs/TS480HX.cxx \
	rigs/TS480SAT.cxx \
	rigs/TS570.cxx \
	rigs/TS590S.cxx \
	rigs/TS2000.cxx \
	rigs/TT516.cxx \
	rigs/DELTA-II.cxx \
	rigs/TT538.cxx \
	rigs/TT550.cxx \
	rigs/TT563.cxx \
	rigs/TT566.cxx \
	rigs/TT588.cxx \
	rigs/TT599.cxx \
	rigs.cxx \
	serial.cxx \
	status.cxx \
	support.cxx \
	util.cxx \
	ValueSlider.cxx \
	xml_io.cxx \
	xmlrpcpp/XmlRpc.h \
	xmlrpcpp/XmlRpcClient.cpp \
	xmlrpcpp/XmlRpcClient.h \
	xmlrpcpp/XmlRpcDispatch.cpp \
	xmlrpcpp/XmlRpcDispatch.h \
	xmlrpcpp/XmlRpcException.h \
	xmlrpcpp/XmlRpcMutex.cpp \
	xmlrpcpp/XmlRpcMutex.h \
	xmlrpcpp/XmlRpcServer.cpp \
	xmlrpcpp/XmlRpcServer.h \
	xmlrpcpp/XmlRpcServerConnection.cpp \
	xmlrpcpp/XmlRpcServerConnection.h \
	xmlrpcpp/XmlRpcServerMethod.cpp \
	xmlrpcpp/XmlRpcServerMethod.h \
	xmlrpcpp/XmlRpcSocket.cpp \
	xmlrpcpp/XmlRpcSocket.h \
	xmlrpcpp/XmlRpcSource.cpp \
	xmlrpcpp/XmlRpcSource.h \
	xmlrpcpp/XmlRpcThread.cpp \
	xmlrpcpp/XmlRpcThread.h \
	xmlrpcpp/XmlRpcThreadedServer.cpp \
	xmlrpcpp/XmlRpcThreadedServer.h \
	xmlrpcpp/XmlRpcUtil.cpp \
	xmlrpcpp/XmlRpcUtil.h \
	xmlrpcpp/XmlRpcValue.cpp \
	xmlrpcpp/XmlRpcValue.h \
	xmlrpcpp/base64.h


# Additional source files that are distributed
EXTRA_DIST = \
	config.h \
	flrig_icon.cxx \
	rigpanel.fl \
	include/combo.h \
	include/debug.h \
	include/dialogs.h \
	include/fileselect.h \
	include/font_browser.h \
	include/flrigrc.h \
	include/flinput2.h \
	include/icons.h \
	include/pixmaps.h \
	include/flslider2.h \
	include/Fl_SigBar.h \
	include/FreqControl.h \
	include/FT100D.h \
	include/FT450.h \
	include/FT767.h \
	include/FT817.h \
	include/FT847.h \
	include/FT857D.h \
	include/FT920.h \
	include/FT890.h \
	include/FT950.h \
	include/FT1000.h \
	include/FT1000MP.h \
	include/FT2000.h \
	include/generic.h \
	include/gettext.h \
	include/ICbase.h \
	include/IC703.h \
	include/IC706MKIIG.h \
	include/IC718.h \
	include/IC728.h \
	include/IC735.h \
	include/IC746.h \
	include/IC756.h \
	include/IC756PRO2.h \
	include/IC756PRO3.h \
	include/IC910.h \
	include/IC7000.h \
	include/IC7200.h \
	include/IC7600.h \
	include/IC7700.h \
	include/images.h \
	include/K2.h \
	include/K3.h \
	include/K3_ui.h \
	include/RAY152.h \
	include/mingw.h \
	include/ptt.h \
	include/rig.h \
	include/rigs.h \
	include/rigbase.h \
	include/rig_io.h \
	include/rigpanel.h \
	include/serial.h \
	include/xml_io.h \
	include/status.h \
	include/support.h \
	include/threads.h \
	include/TS140.h \
	include/TS450S.h \
	include/TS480HX.h \
	include/TS480SAT.h \
	include/TS570.h \
	include/TS590S.h \
	include/TS2000.h \
	include/TT516.h \
	include/DELTA-II.h \
	include/TT538.h \
	include/TT550.h \
	include/TT563.h \
	include/TT566.h \
	include/TT588.h \
	include/TT599.h \
	include/util.h \
	include/ValueSlider.h \
	images/alc.xbm \
	images/P25.xbm \
	images/P50.xbm \
	images/P100.xbm \
	images/P200.xbm \
	images/P200log.xbm \
	images/S60.xbm \
	images/SWR.xbm

# Additional non-source files that are distributed
# Additional source files that support non-Linux cross compilation
EXTRA_DIST += \
	$(srcdir)/../data/flrig.desktop \
	$(srcdir)/../data/flrig.xpm \
	$(srcdir)/../data/win32/fl_app.nsi \
	$(srcdir)/../data/win32/flrig.ico \
	$(srcdir)/../scripts/mknsisinst.sh \
	$(srcdir)/../scripts/build4mingw.sh \
	$(srcdir)/../data/mac/Info.plist.in \
	$(srcdir)/../data/mac/flrig.icns \
	$(srcdir)/../scripts/mkappbundle.sh \
	$(FLRIG_WIN32_SRC) \
	$(FLRIG_FL_SRC)
