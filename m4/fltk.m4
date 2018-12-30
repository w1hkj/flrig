AC_DEFUN([AC_CHECK_FLTK], [
  AC_ARG_VAR([FLTK_CONFIG], [Path to fltk-config utility])
  AC_ARG_VAR([FLTK_CFLAGS], [C compiler flags for FLTK, overriding fltk-config])
  AC_ARG_VAR([FLTK_LIBS], [linker flags for FLTK, overriding fltk-config])

  if test "x$FLTK_CFLAGS" = "x" && test "x$FLTK_LIBS" = "x"; then
      if test "x$FLTK_CONFIG" = "x"; then
          AC_PATH_PROG([FLTK_CONFIG], [fltk-config], [no])
      else
          AC_MSG_CHECKING([for fltk-config])
          AC_MSG_RESULT([$FLTK_CONFIG])
      fi
      if test "$FLTK_CONFIG" = "no"; then
          AC_MSG_ERROR([
  *** The fltk-config script could not be found. Please install the development
  *** headers and libraries for FLTK 1.1.x, or set PATH to the directory that
  *** contains fltk-config.
          ])
      fi
      HAVE_FLTK_API_VERSION=no
      FLTK_API_VERSION="`$FLTK_CONFIG --api-version`"
      if test $? -ne 0; then
          AC_MSG_ERROR([$FLTK_CONFIG failed])
      fi
      if test "x$FLTK_API_VERSION" = "x1.3" ||
         test "x$FLTK_API_VERSION" = "x1.4"; then
          HAVE_FLTK_API_VERSION=yes
      fi
      FLRIG_FLTK_API_MAJOR=${FLTK_API_VERSION%%.*}
      FLRIG_FLTK_API_MINOR=${FLTK_API_VERSION#*.}; FLRIG_FLTK_API_MINOR=${FLRIG_FLTK_API_MINOR%%.*}
      if test "${HAVE_FLTK_API_VERSION}" = "no"; then
          AC_MSG_ERROR([
  *** The version of FLTK found on your system provides API version $FLTK_API_VERSION.
  *** To build $PACKAGE you need a FLTK version that provides API 1.3 or 1.4.
          ])
      fi
      FLTK_CFLAGS=`$FLTK_CONFIG --cxxflags`
      if test "x$ac_cv_static" != "xyes"; then
          FLTK_LIBS=`$FLTK_CONFIG --ldflags --use-images`
          if test "x$target_mingw32" != "xyes"; then
              if test "x$target_darwin" != "xyes"; then
                  if grep -q "lX11" <<< "$FLTK_LIBS"; then
                      FLTK_LIBS="$FLTK_LIBS";
                  else
                      FLTK_LIBS="$FLTK_LIBS -lm -lX11";
                  fi
              fi
          fi
      else
          FLTK_LIBS=`$FLTK_CONFIG --ldstaticflags --use-images`
      fi
  else
      AC_MSG_NOTICE([not checking for FLTK])
  fi
  AC_SUBST([FLTK_CFLAGS])
  AC_SUBST([FLTK_LIBS])
  AC_DEFINE_UNQUOTED([FLTK_BUILD_VERSION], ["`$FLTK_CONFIG --version`"], [FLTK version])
  AC_DEFINE_UNQUOTED([FLRIG_FLTK_API_MAJOR], [$FLRIG_FLTK_API_MAJOR], [FLTK API major version])
  AC_DEFINE_UNQUOTED([FLRIG_FLTK_API_MINOR], [$FLRIG_FLTK_API_MINOR], [FLTK API minor version])

  AC_ARG_VAR([FLUID], [Fast Light User-Interface Designer])
  AC_CHECK_PROG([FLUID], [fluid], [fluid])
  AM_CONDITIONAL([HAVE_FLUID], [test -n "$FLUID"])
])
