#!/bin/sh
FACETRACERIMPORTERBIN=../build/debug/facetracerimport
if [ ! -e $FACETRACERIMPORTERBIN ] ; then
	echo ""
	echo " ERROR: FaceTracerImporter binary was not found"
	echo "    Expected location: " $FACETRACERIMPORTERBIN
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

IMGDMNAME=facetracer

if [ $# -gt 0 ] ; then
	IMAGEFOLDERFILE=$1
else
	IMAGEFOLDERFILE=../../data/facetracer/image
fi
if [ ! -e $IMAGEFOLDERFILE ] ; then
	echo ""
	echo " ERROR: imagefolderfile not found: $IMAGEFOLDERFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	FACEINDEXFILE=$1
else
	FACEINDEXFILE=../../data/facetracer/faceindex.txt
fi
if [ ! -e $FACEINDEXFILE ] ; then
	echo ""
	echo " ERROR: faceindexfile not found: $FACEINDEXFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	FACESTATSFILE=$1
else
	FACESTATSFILE=../../data/facetracer/facestats.txt
fi
if [ ! -e $FACESTATSFILE ] ; then
	echo ""
	echo " ERROR: facestatsfiles not found: $FACESTATSFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	FACELABELFILE=$1
else
	FACELABELFILE=../../data/facetracer/facelabels.txt
fi
if [ ! -e $FACELABELFILE ] ; then
	echo ""
	echo " ERROR: facelabelfiles not found: $FACELABELFILE does not exist"
	echo ""
	return 1;
fi

if [ $# -gt 0 ] ; then
	ATTRIBUTEFILE=$1
else
	ATTRIBUTEFILE=../../data/facetracer/attributes.txt
fi
if [ ! -e $ATTRIBUTEFILE ] ; then
	echo ""
	echo " ERROR: attributefiles not found: $ATTRIBUTEFILE does not exist"
	echo ""
	return 1;
fi

$FACETRACERIMPORTERBIN --sqldb-file=$DBFILE --imgdbname=$IMGDMNAME --image-folder=$IMAGEFOLDERFILE --faceindex-file=$FACEINDEXFILE --facestats-file=$FACESTATSFILE --facelabel-file=$FACELABELFILE --attribute-file=$ATTRIBUTEFILE --clear-sqldb=false