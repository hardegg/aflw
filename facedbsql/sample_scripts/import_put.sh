#!/bin/sh
PUTIMPORTERBIN=../build/debug/putimport
if [ ! -e $PUTIMPORTERBIN ] ; then
	echo ""
	echo " ERROR: PutImporter binary was not found"
	echo "    Expected location: " $PUTIMPORTERBIN
	echo ""
	return 1
fi

if [ $# -gt 0 ] ; then
	DBFILE=$1
else
	DBFILE=../../data/test_expanded.sqlite
fi
if [ ! -e $DBFILE ] ; then
	echo ""
	echo " ERROR: database not found: $DBFILE does not exist"
	echo ""
	return 1;
fi

IMGDMNAME=put

if [ $# -gt 0 ] ; then
	IMGFILE=$1
else
	IMGFILE=../../data/put/images
fi
if [ ! -e $IMGFILE ] ; then
	echo ""
	echo " ERROR: imagefiles not found: $IMGFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	LANDMARKSFILE=$1
else
	LANDMARKSFILE=../../data/put/landmarks
fi
if [ ! -e $LANDMARKSFILE ] ; then
	echo ""
	echo " ERROR: landmarkfiles not found: $LANDMARKSFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	REGIONSFILE=$1
else
	REGIONSFILE=../../data/put/regions
fi
if [ ! -e $REGIONSFILE ] ; then
	echo ""
	echo " ERROR: regionfiles not found: $REGIONSFILE does not exist"
	echo ""
	return 1;
fi

$PUTIMPORTERBIN --sqldb-file=$DBFILE --imgdbname=$IMGDMNAME --image-dir=$IMGFILE --landmarks-dir=$LANDMARKSFILE  --regions-dir=$REGIONSFILE --clear-sqldb=false