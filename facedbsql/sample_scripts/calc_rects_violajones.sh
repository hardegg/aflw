#!/bin/sh
FACEDBTOOLBIN=../build/debug/facedbtool
if [ ! -e $FACEDBTOOLBIN ] ; then
	echo ""
	echo " ERROR: facedbtool binary was not found"
	echo "    Expected location: " $FACEDBTOOLBIN
	echo ""
	return 1
fi

if [ $# -gt 0 ] ; then
	DBFILE=$1
else
	DBFILE=../../data/aflw.sqlite
fi
if [ ! -e $DBFILE ] ; then
	echo ""
	echo " ERROR: database not found: $DBFILE does not exist"
	echo ""
	return 1;
fi

$FACEDBTOOLBIN --action=calcviolajonesrects --sqldb-file=$DBFILE 
