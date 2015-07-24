close all;

dbpath = '../data/';
dbfile = 'aflw.sqlite';

model3d = createMeanFace3DModel(fullfile(dbpath,dbfile));

% get one face_id
mksqlite('open',fullfile(dbpath,dbfile));
fidQuery = 'SELECT face_id FROM Faces LIMIT 1';
res = mksqlite(fidQuery);
mksqlite('close');
face_id = res(1).face_id;
% or alternatively select one here
%face_id = 39341;

facedata = getFaceDataFromSQLite([dbpath dbfile],face_id);

feature2dNames = fieldnames(facedata.pts);
num2dPts = size(feature2dNames,1);

pts2d = zeros(num2dPts,2);
pts3d = zeros(num2dPts,3);

for i=1:num2dPts
    fn = feature2dNames{i};
    if isfield(model3d,fn)
        pts3d(i,:) = model3d.(fn);
        pts2d(i,:) = facedata.pts.(fn);
    else
        fprintf('Error: "%s" is not a feature name in the 3D model\n',fn);
    end
end

camera = struct();
camera.center = [facedata.image.width/2 facedata.image.height/2];
camera.viewPlaneDistance = 1.5*facedata.image.width;

[rot,trans] = calculateTransformation(camera,pts2d,pts3d,facedata.pose);


% rotate model points
rotPts = (rot*pts3d' + repmat(trans,[1 num2dPts]))';

% project model points
projPts = [rotPts(:,1)./rotPts(:,3) rotPts(:,2)./rotPts(:,3)];
projPts = projPts .* camera.viewPlaneDistance + repmat(camera.center,[num2dPts 1]);

model_center = model3d.center_between_eyes;
%model_center = model3d.center_of_head;
rotCenter = (rot*model_center' + trans)';
projCenter = [rotCenter(:,1)./rotCenter(:,3) rotCenter(:,2)./rotCenter(:,3)];
projCenter = projCenter .* camera.viewPlaneDistance + camera.center;

rotCenterUp = (rot*(model_center + [0 0.2 0])' + trans)';
projCenterUp = [rotCenterUp(:,1)./rotCenterUp(:,3) rotCenterUp(:,2)./rotCenterUp(:,3)];
projCenterUp = projCenterUp .* camera.viewPlaneDistance + camera.center;

% show rotated, translated and projected 3d points
%im = imread(imgFile);
im = imread([dbpath facedata.image.db_id '/' facedata.image.filepath]);
figure;
imshow(im);
hold on;

showModel = true;
if showModel
    % show whole model
    myworld = vrworld('meanFaceSimplified.wrl');
    open(myworld);
    x = get(myworld);
    model_pts = x.Nodes(1).point;
    numMPts = size(model_pts,1);

    % rotate model points
    rotMPts = (rot*model_pts' + repmat(trans,[1 numMPts]))';

    % project model points
    projMPts = [rotMPts(:,1)./rotMPts(:,3) rotMPts(:,2)./rotMPts(:,3)];
    projMPts = projMPts .* camera.viewPlaneDistance + repmat(camera.center,[numMPts 1]);

    plot(projMPts(:,1),projMPts(:,2),'y.');
    
    close(myworld);
end

plot(projPts(1:end,1),projPts(1:end,2),'r.');
plot(projCenter(1),projCenter(2),'go');
plot(projCenterUp(1),projCenterUp(2),'bo');
plot(pts2d(:,1),pts2d(:,2),'b.');
for i=1:num2dPts
    text(pts2d(i,1)+10,pts2d(i,2),feature2dNames{i},'Color','r');
end
for i=1:num2dPts
    text(projPts(i,1)+10,projPts(i,2)+5,feature2dNames{i},'Color','b');
end
%set(gca,'YDir','reverse');
%axis equal;

modelCenterDist = sqrt(sum(rotCenter.^2));
cameraModel3dYAngle = atan(rotCenter(2)/sqrt(rotCenter(3)^2 + rotCenter(1)^2));
cameraModel3dXAngle = atan(rotCenter(1)/sqrt(rotCenter(3)^2 + rotCenter(2)^2));
sphereCenterBorderAngle = asin(model3d.sphereRadius/modelCenterDist);
sphereProjTop    = tan(cameraModel3dYAngle - sphereCenterBorderAngle)*camera.viewPlaneDistance;
sphereProjBottom = tan(cameraModel3dYAngle + sphereCenterBorderAngle)*camera.viewPlaneDistance;
sphereProjLeft   = tan(cameraModel3dXAngle - sphereCenterBorderAngle)*camera.viewPlaneDistance;
sphereProjRight  = tan(cameraModel3dXAngle + sphereCenterBorderAngle)*camera.viewPlaneDistance;
sphereProjTop    = sphereProjTop + camera.center(2);
sphereProjBottom = sphereProjBottom + camera.center(2);
sphereProjLeft   = sphereProjLeft + camera.center(1);
sphereProjRight  = sphereProjRight + camera.center(1);

plot(projCenter(1),sphereProjTop,'g.')
plot(projCenter(1),sphereProjBottom,'g.')
plot(sphereProjLeft,projCenter(2),'g.')
plot(sphereProjRight,projCenter(2),'g.')


% rectangle 
targetCenter = [64 37]; % all faces are centered on this point
w_out = 128;
h_out = 128;
normEyeDist = 50.0;

scale = max(sphereProjBottom-sphereProjTop,sphereProjRight-sphereProjLeft)/normEyeDist;

x = projCenter(1) - targetCenter(1)*scale;
y = projCenter(2) - targetCenter(2)*scale;
r = [x y w_out*scale h_out*scale];
rectangle('Position',r,'LineWidth',2,'EdgeColor','b');


