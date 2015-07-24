function facedata = getFaceDataFromSQLite(dbfile,face_id)

    facedata = struct();
    facedata.pts = struct();
    facedata.pose = struct();
    facedata.image = struct();

    mksqlite('open',dbfile);
    
    fidQuery = ['SELECT file_id FROM Faces WHERE face_id = ' num2str(face_id)];
    file_id = mksqlite(fidQuery);
    
    if (size(file_id,1) < 1)
        fprintf(['No image file for face_id ' num2str(face_id)])
        return;
    end
    
    file_id = file_id(1).file_id;
    
    poseQuery = ['SELECT roll,pitch,yaw FROM FacePose WHERE face_id = ' num2str(face_id)];
    pose = mksqlite(poseQuery);
    
    imgDataQuery = ['SELECT db_id,filepath,width,height FROM FaceImages WHERE file_id = "' file_id '"'];
    imgdata = mksqlite(imgDataQuery);
    
    ptsQuery = ['SELECT descr,FeatureCoords.x,FeatureCoords.y FROM FeatureCoords,FeatureCoordTypes WHERE face_id =' num2str(face_id) ' AND FeatureCoords.feature_id = FeatureCoordTypes.feature_id'];
    pts = mksqlite(ptsQuery);
    
    if size(pose,1) > 0
        facedata.pose.roll = pose(1).roll;
        facedata.pose.pitch = pose(1).pitch;
        facedata.pose.yaw = pose(1).yaw;
    else
        facedata.pose.roll = 0;
        facedata.pose.pitch = 0;
        facedata.pose.yaw = 0;
    end
    
    facedata.image.width = imgdata(1).width;
    facedata.image.height = imgdata(1).height;
    facedata.image.filepath = imgdata(1).filepath;
    facedata.image.db_id = imgdata(1).db_id;
    
    for i=1:size(pts,1)
        facedata.pts.(pts(i).descr) = [pts(i).x pts(i).y];
    end
    
    mksqlite('close');