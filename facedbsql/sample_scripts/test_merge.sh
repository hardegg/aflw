#!/bin/sh
cp /home/pwohlhart/work/data/facedb/merge_test/facedb-profile-merged.db /home/pwohlhart/work/data/facedb/merge_test/test-merged.db
./facedbtool --action=merge --sqldb-file=/home/pwohlhart/work/data/facedb/merge_test/test-merged.db --sqldb-file-source=/home/pwohlhart/work/data/facedb/merge_test/facedb-frontal-hillary-02-08-11.db 
./facedbtool --action=merge --sqldb-file=/home/pwohlhart/work/data/facedb/merge_test/test-merged.db --sqldb-file-source=/home/pwohlhart/work/data/facedb/merge_test/facedb-frontal_20110727_isabella.db
