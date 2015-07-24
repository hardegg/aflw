close all;

%dbpath = '../data/';
%dbpath = '/mount/jupiter/public/projects/mdl/data/facedb/';
dbpath = '/home/pwohlhart/work/data/facedb/';
dbfile = 'aflw.sqlite.tmp';

% get one face_id
mksqlite('open',fullfile(dbpath,dbfile));
fidQuery = 'SELECT face_id,feature_id,x,y FROM FeatureCoords WHERE annot_type_id < 0';
res = mksqlite(fidQuery);
face_ids = res.face_id;
feat_ids = res.feature_id;


for i=1:numel(res)
    fid = res(i).face_id;
    ftid = res(i).feature_id;
    
    fidQuery = sprintf('SELECT face_id,feature_id,x,y,annot_type_id FROM FeatureCoords WHERE face_id = %d AND feature_id = %d',fid,ftid);
    res2 = mksqlite(fidQuery);
    
    res(i).x
end






mksqlite('close');
