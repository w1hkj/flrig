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
	cwio/cwio.cxx \
	cwio/morse.cxx \
	cwio/cwioUI.cxx \
	cmedia/cmedia.cxx \
	cmedia/hid.cxx \
	cmedia/tmate2.cxx \
	fskio/fsk.cxx \
	fskio/fskioUI.cxx \
	graphics/pixmaps.cxx \
	graphics/icons.cxx \
	graphics/images.cxx \
	main.cxx \
	rigs/rigbase.cxx \
	rigs/rigs.cxx \
	rigs/elad/FDMDUO.cxx \
	rigs/icom/ICbase.cxx \
	rigs/icom/IC703.cxx \
	rigs/icom/IC705.cxx \
	rigs/icom/IC706MKIIG.cxx \
	rigs/icom/IC718.cxx \
	rigs/icom/IC728.cxx \
	rigs/icom/IC735.cxx \
	rigs/icom/IC746.cxx \
	rigs/icom/IC756.cxx \
	rigs/icom/IC756PRO2.cxx \
	rigs/icom/IC756PRO3.cxx \
	rigs/icom/IC910.cxx \
	rigs/icom/IC7000.cxx \
	rigs/icom/IC7100.cxx \
	rigs/icom/IC7200.cxx \
	rigs/icom/IC7300.cxx \
	rigs/icom/IC7410.cxx \
	rigs/icom/IC7600.cxx \
	rigs/icom/IC7610.cxx \
	rigs/icom/IC7800.cxx \
	rigs/icom/IC7851.cxx \
	rigs/icom/IC9100.cxx \
	rigs/icom/IC9700.cxx \
	rigs/icom/IC7700.cxx \
	rigs/icom/ICF8101.cxx \
	rigs/elecraft/K2.cxx \
	rigs/elecraft/K3.cxx \
	rigs/elecraft/KX3.cxx \
	rigs/lab599/TX500.cxx \
	rigs/kenwood/KENWOOD.cxx \
	rigs/kenwood/TS140.cxx \
	rigs/kenwood/TS440.cxx \
	rigs/kenwood/TS450S.cxx \
	rigs/kenwood/TS480HX.cxx \
	rigs/kenwood/TS480SAT.cxx \
	rigs/kenwood/TS570.cxx \
	rigs/kenwood/TS590S.cxx \
	rigs/kenwood/TS590SG.cxx \
	rigs/kenwood/TS790.cxx \
	rigs/kenwood/TS850.cxx \
	rigs/kenwood/TS870S.cxx \
	rigs/kenwood/TS890S.cxx \
	rigs/kenwood/TS940S.cxx \
	rigs/kenwood/TS950.cxx \
	rigs/kenwood/TS990.cxx \
	rigs/kenwood/TS2000.cxx \
	rigs/other/AOR5K.cxx \
	rigs/other/FLEX1500.cxx \
	rigs/other/PCR1000.cxx \
	rigs/other/PowerSDR.cxx \
	rigs/other/RAY152.cxx \
	rigs/other/TMD710.cxx \
	rigs/tentec/TT516.cxx \
	rigs/tentec/DELTA-II.cxx \
	rigs/tentec/TT538.cxx \
	rigs/tentec/TT550.cxx \
	rigs/tentec/TT563.cxx \
	rigs/tentec/TT566.cxx \
	rigs/tentec/TT588.cxx \
	rigs/tentec/TT599.cxx \
	rigs/xiegu/Xiegu-5105.cxx \
	rigs/xiegu/Xiegu-G90.cxx \
	rigs/xiegu/X6100.cxx \
	rigs/yaesu/FT100D.cxx \
	rigs/yaesu/FT450.cxx \
	rigs/yaesu/FT450D.cxx \
	rigs/yaesu/FT736R.cxx \
	rigs/yaesu/FT747.cxx \
	rigs/yaesu/FT757GX2.cxx \
	rigs/yaesu/FT767.cxx \
	rigs/yaesu/FT817.cxx \
	rigs/yaesu/FT817BB.cxx \
	rigs/yaesu/FT818.cxx \
	rigs/yaesu/FT847.cxx \
	rigs/yaesu/FT857D.cxx \
	rigs/yaesu/FT890.cxx \
	rigs/yaesu/FT891.cxx \
	rigs/yaesu/FT900.cxx \
	rigs/yaesu/FT920.cxx \
	rigs/yaesu/FT950.cxx \
	rigs/yaesu/FT990.cxx \
	rigs/yaesu/FT990a.cxx \
	rigs/yaesu/FT991.cxx \
	rigs/yaesu/FT991A.cxx \
	rigs/yaesu/FT1000.cxx \
	rigs/yaesu/FT1000MP.cxx \
	rigs/yaesu/FT1000MP_A.cxx \
	rigs/yaesu/FT2000.cxx \
	rigs/yaesu/FTdx101D.cxx \
	rigs/yaesu/FTdx10.cxx \
	rigs/yaesu/FTdx1200.cxx \
	rigs/yaesu/FTdx3000.cxx \
	rigs/yaesu/FT5000.cxx \
	rigs/yaesu/FTdx9000.cxx \
	support/debug.cxx \
	support/dialogs.cxx \
	support/gpio_ptt.cxx \
	support/ptt.cxx \
	support/rig_io.cxx \
	support/serial.cxx \
	support/socket.cxx \
	support/socket_io.cxx \
	support/status.cxx \
	support/support.cxx \
	support/read_rig.cxx \
	support/restore_rig.cxx \
	support/init_rig.cxx \
	support/TT550_support.cxx \
	support/init_user_interface.cxx \
	support/threads.cxx \
	support/timeops.cxx \
	support/trace.cxx \
	support/util.cxx \
	UI/K3_ui.cxx \
	UI/KX3_ui.cxx \
	UI/rigpanel.cxx \
	UI/meters_dialog.cxx \
	widgets/combo.cxx \
	widgets/Fl_SigBar.cxx \
	widgets/flbrowser2.cxx \
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
	cmedia/hid_lin.cxx \
	cmedia/hid_mac.cxx \
	cmedia/hid_win.cxx \
	UI/ui_bitmaps.cxx \
	UI/gpio.cxx \
	UI/ui_wide.cxx \
	UI/ui_small.cxx \
	UI/ui_touch.cxx \
	UI/ui_setup.cxx \
	UI/ui_memory.cxx \
	UI/xcvr_setup.cxx \
	UI/meters_setup.cxx \
	UI/power_meter_setup.cxx \
	include/cmedia.h \
	include/hid_lin.h \
	include/hid_mac.h \
	include/hid_win.h \
	include/hidapi.h \
	include/tmate2.h \
	include/cwio.h \
	include/cwioUI.h \
	include/fsk.h \
	include/fskioUI.h \
	include/gpio.h \
	include/gpio_ptt.h \
	include/morse.h \
	include/debug.h \
	include/dialogs.h \
	include/flbrowser2.h \
	include/flinput2.h \
	include/flslider2.h \
	include/fileselect.h \
	include/font_browser.h \
	include/flrigrc.h \
	include/icons.h \
	include/kenwood/KENWOOD.h \
	include/pixmaps.h \
	include/Fl_SigBar.h \
	include/FreqControl.h \
	include/hspinner.h \
	include/other/AOR5K.h \
	include/generic.h \
	include/gettext.h \
	include/hidapi.h \
	include/hid_mac.h \
	include/hid_win.h \
	include/elad/FDMDUO.h \
	include/icom/ICbase.h \
	include/icom/IC703.h \
	include/icom/IC705.h \
	include/icom/IC706MKIIG.h \
	include/icom/IC718.h \
	include/icom/IC728.h \
	include/icom/IC735.h \
	include/icom/IC746.h \
	include/icom/IC756.h \
	include/icom/IC756PRO2.h \
	include/icom/IC756PRO3.h \
	include/icom/IC910.h \
	include/icom/IC7000.h \
	include/icom/IC7100.h \
	include/icom/IC7410.h \
	include/icom/IC7200.h \
	include/icom/IC7300.h \
	include/icom/IC7600.h \
	include/icom/IC7610.h \
	include/icom/IC7700.h \
	include/icom/IC7800.h \
	include/icom/IC7851.h \
	include/icom/IC9100.h \
	include/icom/IC9700.h \
	include/icom/ICF8101.h \
	include/images.h \
	include/elecraft/K2.h \
	include/elecraft/K3.h \
	include/K3_ui.h \
	include/elecraft/KX3.h \
	include/KX3_ui.h \
	include/mingw.h \
	include/other/FLEX1500.h \
	include/other/PCR1000.h \
	include/other/RAY152.h \
	include/other/PowerSDR.h \
	include/other/TMD710.h \
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
	include/lab599/TX500.h \
	include/kenwood/KENWOOD.h \
	include/kenwood/TS140.h \
	include/kenwood/TS440.h \
	include/kenwood/TS450S.h \
	include/kenwood/TS480HX.h \
	include/kenwood/TS480SAT.h \
	include/kenwood/TS570.h \
	include/kenwood/TS590S.h \
	include/kenwood/TS590SG.h \
	include/kenwood/TS790.h \
	include/kenwood/TS850.h \
	include/kenwood/TS870S.h \
	include/kenwood/TS890S.h \
	include/kenwood/TS940S.h \
	include/kenwood/TS950.h \
	include/kenwood/TS990.h \
	include/kenwood/TS2000.h \
	include/tentec/DELTA-II.h \
	include/tentec/TT516.h \
	include/tentec/TT538.h \
	include/tentec/TT550.h \
	include/tentec/TT563.h \
	include/tentec/TT566.h \
	include/tentec/TT588.h \
	include/tentec/TT599.h \
	include/timeops.h \
	include/tod_clock.h \
	include/ui.h \
	include/util.h \
	include/ValueSlider.h \
	include/xml_server.h \
	include/xiegu/Xiegu-5105.h \
	include/xiegu/Xiegu-G90.h \
	include/xiegu/X6100.h \
	include/yaesu/FT100D.h \
	include/yaesu/FT450.h \
	include/yaesu/FT450D.h \
	include/yaesu/FT736R.h \
	include/yaesu/FT747.h \
	include/yaesu/FT757GX2.h \
	include/yaesu/FT767.h \
	include/yaesu/FT817.h \
	include/yaesu/FT817BB.h \
	include/yaesu/FT818.h \
	include/yaesu/FT847.h \
	include/yaesu/FT857D.h \
	include/yaesu/FT920.h \
	include/yaesu/FT890.h \
	include/yaesu/FT891.h \
	include/yaesu/FT900.h \
	include/yaesu/FT950.h \
	include/yaesu/FT990.h \
	include/yaesu/FT990a.h \
	include/yaesu/FT991.h \
	include/yaesu/FT991A.h \
	include/yaesu/FT1000.h \
	include/yaesu/FT1000MP.h \
	include/yaesu/FT1000MP_A.h \
	include/yaesu/FT2000.h \
	include/yaesu/FTdx10.h \
	include/yaesu/FTdx101D.h \
	include/yaesu/FTdx1200.h \
	include/yaesu/FTdx3000.h \
	include/yaesu/FT5000.h \
	include/yaesu/FTdx9000.h \
	include/combo.h \
	images/alc.xbm \
	images/P5.xbm \
	images/P10.xbm \
	images/P15.xbm \
	images/P20.xbm \
	images/P25.xbm \
	images/P50.xbm \
	images/P100.xbm \
	images/P200.xbm \
	images/P200log.xbm \
	images/S60.xbm \
	images/SWR.xbm \
	images/volts.xbm \
	cwio/cwioUI.fl \
	fskio/fskioUI.fl

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
