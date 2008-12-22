#!/bin/sh

UHEXEN2_TOP=../..
. $UHEXEN2_TOP/scripts/cross_defs_w64

if test "$1" = "strip"; then
	$STRIPPER hwrcon.exe hwterm.exe
	exit 0
fi

HOST_OS=`uname|sed -e s/_.*//|tr '[:upper:]' '[:lower:]'`

case "$HOST_OS" in
linux)
	MAKE_CMD=make
	;;
freebsd|openbsd|netbsd)
	MAKE_CMD=gmake
	;;
*)
	MAKE_CMD=make
	;;
esac

exec $MAKE_CMD $*

