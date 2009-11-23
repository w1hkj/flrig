AC_DEFUN([AC_FLRIG_SH_DQ], [
  ac_sh_dq="\"`$1 | sed 's/"/\\\\"/g'`\""
])

AC_DEFUN([AC_FLRIG_BUILD_INFO], [
# Define build flags and substitute in Makefile.in
# CPPFLAGS
  FLRIG_BUILD_CPPFLAGS="-I\$(srcdir) -I\$(srcdir)/include"
# CXXFLAGS
  FLRIG_BUILD_CXXFLAGS="$FLTK_CFLAGS -pipe -Wall -fexceptions $OPT_CFLAGS $DEBUG_CFLAGS $XMLRPC_CFLAGS"
# LDFLAGS
  FLRIG_BUILD_LDFLAGS=
# LDADD
  FLRIG_BUILD_LDADD="$FLTK_LIBS $EXTRA_LIBS $XMLRPC_LIBS"

  if test "x$ac_cv_debug" = "xyes"; then
      FLRIG_BUILD_CXXFLAGS="$FLRIG_BUILD_CXXFLAGS -UNDEBUG"
      FLRIG_BUILD_LDFLAGS="$FLRIG_BUILD_LDFLAGS $RDYNAMIC"
  else
      FLRIG_BUILD_CXXFLAGS="$FLRIG_BUILD_CXXFLAGS -DNDEBUG"
  fi

  AC_SUBST([FLRIG_BUILD_CPPFLAGS])
  AC_SUBST([FLRIG_BUILD_CXXFLAGS])
  AC_SUBST([FLRIG_BUILD_LDFLAGS])
  AC_SUBST([FLRIG_BUILD_LDADD])

#define build variables for config.h
  AC_DEFINE_UNQUOTED([BUILD_BUILD_PLATFORM], ["$build"], [Build platform])
  AC_DEFINE_UNQUOTED([BUILD_HOST_PLATFORM], ["$host"], [Host platform])
  AC_DEFINE_UNQUOTED([BUILD_TARGET_PLATFORM], ["$target"], [Target platform])

  test "x$LC_ALL" != "x" && LC_ALL_saved="$LC_ALL"
  LC_ALL=C
  export LC_ALL

  AC_FLRIG_SH_DQ([echo $ac_configure_args])
  AC_DEFINE_UNQUOTED([BUILD_CONFIGURE_ARGS], [$ac_sh_dq], [Configure arguments])

  AC_FLRIG_SH_DQ([date])
  AC_DEFINE_UNQUOTED([BUILD_DATE], [$ac_sh_dq], [Build date])

  AC_FLRIG_SH_DQ([whoami])
  AC_DEFINE_UNQUOTED([BUILD_USER], [$ac_sh_dq], [Build user])

  AC_FLRIG_SH_DQ([hostname])
  AC_DEFINE_UNQUOTED([BUILD_HOST], [$ac_sh_dq], [Build host])

  AC_FLRIG_SH_DQ([$CXX -v 2>&1 | tail -1])
  AC_DEFINE_UNQUOTED([BUILD_COMPILER], [$ac_sh_dq], [Compiler])

  AC_FLRIG_SH_DQ([echo $FLRIG_BUILD_CPPFLAGS $FLRIG_BUILD_CXXFLAGS])
  AC_DEFINE_UNQUOTED([FLRIG_BUILD_CXXFLAGS], [$ac_sh_dq], [FLRIG compiler flags])
  AC_FLRIG_SH_DQ([echo $FLRIG_BUILD_LDFLAGS $FLRIG_BUILD_LDADD])
  AC_DEFINE_UNQUOTED([FLRIG_BUILD_LDFLAGS], [$ac_sh_dq], [FLRIG linker flags])

  if test "x$LC_ALL_saved" != "x"; then
      LC_ALL="$LC_ALL_saved"
      export LC_ALL
  fi
])
