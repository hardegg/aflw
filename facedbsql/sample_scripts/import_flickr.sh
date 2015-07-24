#!/bin/sh
FLICKRIMPORTERBIN=../build/debug/flickrimport
if [ ! -e $FLICKRIMPORTERBIN ] ; then
	echo ""
	echo " ERROR: FlickrImporter binary was not found"
	echo "    Expected location: " $FLICKRIMPORTERBIN
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

IMGDMNAME=flickr

if [ $# -gt 0 ] ; then
	IMGFILE=$1
else
	IMGFILE=../../data/flickr
fi
if [ ! -e $IMGFILE ] ; then
	echo ""
	echo " ERROR: imagefiles not found: $IMGFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	FEATFILE=$1
else
	FEATFILE=../../data/flickr/annotation/features.txt
fi
if [ ! -e $FEATFILE ] ; then
	echo ""
	echo " ERROR: featurefile not found: $FEATFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	POSEFILE=$1
else
	POSEFILE=../../data/flickr/annotation/angles.txt
fi
if [ ! -e $POSEFILE ] ; then
	echo ""
	echo " ERROR: posefile not found: $POSEFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	METAFILE=$1
else
	METAFILE=../../data/flickr/annotation/metaData.txt
fi
if [ ! -e $METAFILE ] ; then
	echo ""
	echo " ERROR: metafile not found: $METAFILE does not exist"
	echo ""
	return 1;
fi
$FLICKRIMPORTERBIN --sqldb-file=$DBFILE --imgdbname=$IMGDMNAME --feat-file=$FEATFILE --pose-file=$POSEFILE --meta-file=$METAFILE