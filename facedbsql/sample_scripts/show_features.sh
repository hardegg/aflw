#!/bin/sh
FACEDBTOOLBIN=../build/debug/facedbtool
if [ ! -e $FACEDBTOOLBIN ] ; then
	echo ""
	echo " ERROR: facedbtool binary was not found"
	echo "    Expected location: " $FACEDBTOOLBIN
	echo ""
	return 1
fi

if [ $# -gt 1 ] ; then
	DBFILE=$2
else
	DBFILE=../../data/aflw.sqlite
fi
if [ ! -e $DBFILE ] ; then
	echo ""
	echo " ERROR: database not found: $DBFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	FACEID=$1
else
	FACEID=`sqlite3 $DBFILE "SELECT face_id FROM Faces LIMIT 1"`
fi
echo "Showing face with ID " $FACEID 

$FACEDBTOOLBIN --action=showfeat --sqldb-file=$DBFILE --face-id=$FACEID
