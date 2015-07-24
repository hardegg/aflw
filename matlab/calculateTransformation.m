function [rot,trans] = calculateTransformation(camera,pts2d,pts3d,pose)

    numPts = size(pts2d,1);

    %pts2d = pts2d - repmat(camera.center,[numPts 1]);

    %rotPts = rotateToPose(pts3d,pose);
    
    %rotPts(:,2)
    %rotPts(:,2) = -rotPts(:,2); % to image coords -> y is inverted
    
    % 3d plots of original input points
%     figure;
%     plot3(pts3d(:,1),pts3d(:,2),pts3d(:,3),'bx-');
%     xlabel('x');
%     ylabel('y');
%     zlabel('z');
%     axis equal;
    % 3d plots of input points rotated by pose
%     figure;
%     plot3(rotPts(:,1),rotPts(:,2),rotPts(:,3),'rx-');
%     xlabel('x');
%     ylabel('y');
%     zlabel('z');
%     axis equal;
    
    % plot input 2d points
    figure;
    plot(pts2d(:,1),pts2d(:,2),'rx');
    for i=1:numPts
        text(pts2d(i,1),pts2d(i,2),num2str(i));
    end
    set(gca,'YDir','reverse');
    axis equal;
    
%     rotPts(:,3) = rotPts(:,3) - 5; %% huhu
%     figure;
%     plot(rotPts(:,1)./-rotPts(:,3),-rotPts(:,2)./-rotPts(:,3),'bx');
%     for i=1:numPts
%         text(rotPts(i,1)./-rotPts(i,3),-rotPts(i,2)./-rotPts(i,3),num2str(i));
%     end
%     set(gca,'YDir','reverse');
%     axis equal;

%     figure;
%     plot(pts3d(:,1),-pts3d(:,2),'gx');
%     for i=1:numPts
%         text(pts3d(i,1),-pts3d(i,2),num2str(i));
%     end
%     set(gca,'YDir','reverse');
%     axis equal;
    
    [rot,trans] = modernPosit(pts2d,pts3d,camera.viewPlaneDistance,camera.center);
    %rot
    %trans

    % show coordinate systems
    figure
    quiver3(0,0,0,1,0,0,'r')
    hold on;
    quiver3(0,0,0,0,1,0,'g')
    quiver3(0,0,0,0,0,1,'b')
    quiver3(trans(1),trans(2),trans(3),rot(1,1),rot(2,1),rot(3,1),'r');
    quiver3(trans(1),trans(2),trans(3),rot(1,2),rot(2,2),rot(3,2),'g');
    quiver3(trans(1),trans(2),trans(3),rot(1,3),rot(2,3),rot(3,3),'b');
    axis equal;
end
