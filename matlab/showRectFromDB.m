function showRectFromDB(face_id)

    dbpath = '../data/';
    dbfile = 'aflw.sqlite';

    mksqlite('open',fullfile(dbpath,dbfile));

    if nargin < 1
        %face_id = 39387;
        fidQuery = 'SELECT face_id FROM Faces ORDER BY RANDOM() LIMIT 1';
        res = mksqlite(fidQuery);
        face_id = res(1).face_id;
    end
    
    fprintf('showing rect for face %d\n',face_id);

    fidQuery = ['SELECT file_id FROM Faces WHERE face_id = ' num2str(face_id)];
    file_id = mksqlite(fidQuery);
    if (size(file_id,1) < 1)
        fprintf(['No image file for face_id ' num2str(face_id)])
        return;
    end
    file_id = file_id(1).file_id;
    
    imgDataQuery = ['SELECT db_id,filepath,width,height FROM FaceImages WHERE file_id = "' file_id '"'];
    imgdata = mksqlite(imgDataQuery);
    imageFilepath = imgdata(1).filepath;
    imageDb_id = imgdata(1).db_id;
        
    faceRectQuery = ['SELECT x,y,w,h FROM FaceRect WHERE face_id = ' num2str(face_id)];
    faceRect = mksqlite(faceRectQuery);
    faceRect = faceRect(1);
    
    im = imread([dbpath imageDb_id '/' imageFilepath]);
    figure;
    imshow(im);
    hold on;
    
    rectangle('Position',[faceRect.x faceRect.y faceRect.w faceRect.h],'EdgeColor','r')
    
    
    mksqlite('close');
    