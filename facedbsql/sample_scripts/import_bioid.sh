#!/bin/sh
BIOIDIMPORTERBIN=../build/debug/bioidimport
if [ ! -e $BIOIDIMPORTERBIN ] ; then
	echo ""
	echo " ERROR: BioIdImporter binary was not found"
	echo "    Expected location: " $BIOIDIMPORTERBIN
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

IMGDMNAME=bioid

if [ $# -gt 0 ] ; then
	IMGFILE=$1
else
	IMGFILE=../../data/bioid
fi
if [ ! -e $IMGFILE ] ; then
	echo ""
	echo " ERROR: imagefiles not found: $IMGFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	EYEFILE=$1
else
	EYEFILE=../../data/bioid
fi
if [ ! -e $EYEFILE ] ; then
	echo ""
	echo " ERROR: eyefiles not found: $EYEFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	POINTSFILE=$1
else
	POINTSFILE=../../data/bioid/bioid_pts/points_20
fi
if [ ! -e $POINTSFILE ] ; then
	echo ""
	echo " ERROR: pointsfiles not found: $POINTSFILE does not exist"
	echo ""
	return 1;
fi

$BIOIDIMPORTERBIN --sqldb-file=$DBFILE --imgdbname=$IMGDMNAME --image-dir=$IMGFILE --eyes-dir=$EYEFILE --points-dir=$POINTSFILE --clear-sqldb=false