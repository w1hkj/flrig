# Copyright (c) 2008 Dave Freese, W1HKJ (w1hkj AT w1hkj DOT com)

bin_PROGRAMS = flrig

MINGW32_SRC = \
	flrig-rc.rc \
	include/flrigrc.h \
	support/mingw.c \
	include/compat.h \
	include/compat-mingw.h

# Build the xmlrpcpp source if libflxmlrpc is not found
XMLRPCPP_SRC = \
	xmlrpcpp/XmlRpcBase64.h \
	xmlrpcpp/XmlRpcClient.cpp \
	xmlrpcpp/XmlRpcClient.h \
	xmlrpcpp/XmlRpcDispatch.cpp \
	xmlrpcpp/XmlRpcDispatch.h \
	xmlrpcpp/XmlRpcException.h \
	xmlrpcpp/XmlRpc.h \
	xmlrpcpp/XmlRpcMutex.cpp \
	xmlrpcpp/XmlRpcMutex.h \
	xmlrpcpp/XmlRpcServerConnection.cpp \
	xmlrpcpp/XmlRpcServerConnection.h \
	xmlrpcpp/XmlRpcServer.cpp \
	xmlrpcpp/XmlRpcServer.h \
	xmlrpcpp/XmlRpcServerMethod.cpp \
	xmlrpcpp/XmlRpcServerMethod.h \
	xmlrpcpp/XmlRpcSocket.cpp \
	xmlrpcpp/XmlRpcSocket.h \
	xmlrpcpp/XmlRpcSource.cpp \
	xmlrpcpp/XmlRpcSource.h \
	xmlrpcpp/XmlRpcUtil.cpp \
	xmlrpcpp/XmlRpcUtil.h \
	xmlrpcpp/XmlRpcValue.cpp \
	xmlrpcpp/XmlRpcValue.h

# We distribute these but do not always compile them
EXTRA_flrig_SOURCES = $(FLRIG_WIN32_RES_SRC) $(MINGW32_SRC) $(XMLRPCPP_SRC)

flrig_SOURCES =

if !ENABLE_FLXMLRPC
  flrig_SOURCES += $(XMLRPCPP_SRC)
  flrig_CPPFLAGS = @FLRIG_BUILD_CPPFLAGS@
  flrig_CXXFLAGS = @FLRIG_BUILD_CXXFLAGS@
  flrig_CFLAGS = $(flrig_CXXFLAGS)
  flrig_LDFLAGS = @FLRIG_BUILD_LDFLAGS@
  flrig_LDADD = @FLRIG_BUILD_LDADD@
else
  flrig_CPPFLAGS = @FLRIG_BUILD_CPPFLAGS@ @FLXMLRPC_CFLAGS@
  flrig_CXXFLAGS = @FLRIG_BUILD_CXXFLAGS@ @FLXMLRPC_CFLAGS@
  flrig_CFLAGS = $(flrig_CXXFLAGS)
  flrig_LDFLAGS = @FLRIG_BUILD_LDFLAGS@ @FLXMLRPC_LIBS@
  flrig_LDADD = @FLRIG_BUILD_LDADD@
endif

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

clean-local:
	-rm -rf $(CLEAN_LOCAL)

# Sources that we build. It is OK to have headers here.
flrig_SOURCES += \
	graphics/pixmaps.cxx \
	graphics/icons.cxx \
	graphics/images.cxx \
	main.cxx \
	rigs/rigbase.cxx \
	rigs/rigs.cxx \
	rigs/AOR5K.cxx \
	rigs/FT100D.cxx \
	rigs/FT450.cxx \
	rigs/FT450D.cxx \
	rigs/FT747.cxx \
	rigs/FT767.cxx \
	rigs/FT817.cxx \
	rigs/FT847.cxx \
	rigs/FT857D.cxx \
	rigs/FT890.cxx \
	rigs/FT891.cxx \
	rigs/FT900.cxx \
	rigs/FT920.cxx \
	rigs/FT950.cxx \
	rigs/FT990.cxx \
	rigs/FT990a.cxx \
	rigs/FT991.cxx \
	rigs/FT991A.cxx \
	rigs/FT1000.cxx \
	rigs/FT1000MP.cxx \
	rigs/FT2000.cxx \
	rigs/FTdx101D.cxx \
	rigs/FTdx1200.cxx \
	rigs/FTdx3000.cxx \
	rigs/FT5000.cxx \
	rigs/FTdx9000.cxx \
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
	rigs/IC7100.cxx \
	rigs/IC7200.cxx \
	rigs/IC7300.cxx \
	rigs/IC7410.cxx \
	rigs/IC7600.cxx \
	rigs/IC7610.cxx \
	rigs/IC7800.cxx \
	rigs/IC7851.cxx \
	rigs/IC9100.cxx \
	rigs/IC9700.cxx \
	rigs/IC7700.cxx \
	rigs/ICF8101.cxx \
	rigs/KENWOOD.cxx \
	rigs/K2.cxx \
	rigs/K3.cxx \
	rigs/KX3.cxx \
	rigs/PCR1000.cxx \
	rigs/RAY152.cxx \
	rigs/TS140.cxx \
	rigs/TS450S.cxx \
	rigs/TS480HX.cxx \
	rigs/TS480SAT.cxx \
	rigs/TS570.cxx \
	rigs/TS590S.cxx \
	rigs/TS590SG.cxx \
	rigs/TS790.cxx \
	rigs/TS850.cxx \
	rigs/TS870S.cxx \
	rigs/TS890S.cxx \
	rigs/TS940S.cxx \
	rigs/TS950.cxx \
	rigs/TS990.cxx \
	rigs/TS2000.cxx \
	rigs/TT516.cxx \
	rigs/DELTA-II.cxx \
	rigs/TT538.cxx \
	rigs/TT550.cxx \
	rigs/TT563.cxx \
	rigs/TT566.cxx \
	rigs/TT588.cxx \
	rigs/TT599.cxx \
	support/debug.cxx \
	support/dialogs.cxx \
	support/ptt.cxx \
	support/rig_io.cxx \
	support/serial.cxx \
	support/socket.cxx \
	support/socket_io.cxx \
	support/status.cxx \
	support/support.cxx \
	support/threads.cxx \
	support/timeops.cxx \
	support/trace.cxx \
	support/util.cxx \
	UI/K3_ui.cxx \
	UI/KX3_ui.cxx \
	UI/rigpanel.cxx \
	widgets/combo.cxx \
	widgets/Fl_SigBar.cxx \
	widgets/flinput2.cxx \
	widgets/flslider2.cxx \
	widgets/font_browser.cxx \
	widgets/FreqControl.cxx \
	widgets/pl_tones.cxx \
	widgets/ValueSlider.cxx \
	widgets/hspinner.cxx \
	support/tod_clock.cxx \
	server/xml_server.cxx 

# Additional source files that are distributed
EXTRA_DIST = \
	config.h \
	flrig_icon.cxx \
	UI/ui_bitmaps.cxx \
	UI/ui_wide.cxx \
	UI/ui_small.cxx \
	UI/ui_touch.cxx \
	UI/ui_setup.cxx \
	UI/ui_memory.cxx \
	UI/xcvr_setup.cxx \
	UI/meters_setup.cxx \
	UI/power_meter_setup.cxx \
	include/combo.h \
	include/debug.h \
	include/dialogs.h \
	include/fileselect.h \
	include/font_browser.h \
	include/flrigrc.h \
	include/flinput2.h \
	include/icons.h \
	include/KENWOOD.h \
	include/pixmaps.h \
	include/flslider2.h \
	include/Fl_SigBar.h \
	include/FreqControl.h \
	include/hspinner.h \
	include/AOR5K.h \
	include/FT100D.h \
	include/FT450.h \
	include/FT450D.h \
	include/FT747.h \
	include/FT767.h \
	include/FT817.h \
	include/FT847.h \
	include/FT857D.h \
	include/FT920.h \
	include/FT890.h \
	include/FT891.h \
	include/FT900.h \
	include/FT950.h \
	include/FT990.h \
	include/FT990a.h \
	include/FT991.h \
	include/FT991A.h \
	include/FT1000.h \
	include/FT1000MP.h \
	include/FT2000.h \
	include/FTdx101D.h \
	include/FTdx1200.h \
	include/FTdx3000.h \
	include/FT5000.h \
	include/FTdx9000.h \
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
	include/IC7100.h \
	include/IC7410.h \
	include/IC7200.h \
	include/IC7300.h \
	include/IC7600.h \
	include/IC7610.h \
	include/IC7700.h \
	include/IC7800.h \
	include/IC7851.h \
	include/IC9100.h \
	include/IC9700.h \
	include/ICF8101.h \
	include/images.h \
	include/K2.h \
	include/K3.h \
	include/K3_ui.h \
	include/KX3.h \
	include/KX3_ui.h \
	include/PCR1000.h \
	include/RAY152.h \
	include/mingw.h \
	include/pl_tones.h \
	include/ptt.h \
	include/rig.h \
	include/rigs.h \
	include/rigbase.h \
	include/rig_io.h \
	include/rigpanel.h \
	include/serial.h \
	include/socket.h \
	include/socket_io.h \
	include/status.h \
	include/support.h \
	include/threads.h \
	include/trace.h \
	include/TS140.h \
	include/TS450S.h \
	include/TS480HX.h \
	include/TS480SAT.h \
	include/TS570.h \
	include/TS590S.h \
	include/TS590SG.h \
	include/TS790.h \
	include/TS850.h \
	include/TS870S.h \
	include/TS890S.h \
	include/TS940S.h \
	include/TS950.h \
	include/TS990.h \
	include/TS2000.h \
	include/TT516.h \
	include/DELTA-II.h \
	include/TT538.h \
	include/TT550.h \
	include/TT563.h \
	include/TT566.h \
	include/TT588.h \
	include/TT599.h \
	include/timeops.h \
	include/tod_clock.h \
	include/ui.h \
	include/util.h \
	include/ValueSlider.h \
	include/xml_server.h \
	images/alc.xbm \
	images/P5.xbm \
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
	$(srcdir)/../scripts/buildmxe.sh \
	$(srcdir)/../scripts/builddist.sh \
	$(srcdir)/../data/mac/Info.plist.in \
	$(srcdir)/../data/mac/flrig.icns \
	$(srcdir)/../scripts/mkappbundle.sh \
	$(FLRIG_WIN32_SRC) \
	$(FLRIG_FL_SRC)
