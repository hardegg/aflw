#!/bin/sh
FACEDBTOOLBIN=../build/release/facedbtool
if [ ! -e $FACEDBTOOLBIN ] ; then
	echo ""
	echo " ERROR: facedbtool binary was not found"
	echo "    Expected location: " $FACEDBTOOLBIN
	echo ""
	return 1
fi

#DBFILE=../../data/aflw.sqlite
if [ $# -gt 0 ] ; then
	DBFILE=$1
else
	DBFILE=../../data/aflw.sqlite
fi
if [ ! -e $DBFILE ] ; then
	echo ""
	echo " ERROR: database not found: '$DBFILE' does not exist"
	echo ""
	return 1;
fi

$FACEDBTOOLBIN --action=calcpose --sqldb-file=$DBFILE
