function model3d = createMeanFace3DModel(dbfile)

    model3d = struct();

    mksqlite('open',dbfile);
    fcQuery = 'SELECT feature_id,descr,code,x,y,z FROM FeatureCoordTypes WHERE NOT x IS NULL';
    res = mksqlite(fcQuery);
    mksqlite('close');
    
    for i=1:numel(res)
        model3d.(res(i).descr) = [res(i).x, res(i).y, res(i).z];
    end
    
    model3d.featurenames = fieldnames(model3d);
    
    %
    model3d.center_of_head = model3d.HeadCenter;

    %
    model3d.center_between_eyes = (model3d.LeftEyeCenter + model3d.RightEyeCenter)./2;

    %
    eyeDistV = model3d.LeftEyeCenter - model3d.RightEyeCenter;
    eyeDist = sqrt(sum(eyeDistV.^2));
    model3d.sphereRadius = eyeDist / 2;
return;
