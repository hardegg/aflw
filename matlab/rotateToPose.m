function pts3d = rotateToPose(pts3d,pose)

    if isstruct(pose)
        yaw = pose.yaw;
        pitch = pose.pitch;
        roll = pose.roll;
    else
        yaw = pose(1);
        pitch = pose(2);
        roll = pose(3);
    end
    
    sy = sin(yaw);
    cy = cos(yaw);
    sp = sin(pitch);
    cp = cos(pitch);
    sr = sin(roll);
    cr = cos(roll);

    Rz = [cr -sr 0; ...
          sr  cr 0; ...
          0    0 1];

    Ry = [ cy 0 -sy; ...
            0 1   0; ...
           sy 0  cy];
 
    Rx = [1   0  0; ...
          0  cp sp; ...     
          0 -sp cp];

    R = Rz*Rx*Ry;
    %R = Ry;

    pts3d = (R*pts3d')';
    
end