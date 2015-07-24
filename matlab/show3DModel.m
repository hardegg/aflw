function show3DModel(model3d)

    numPts = size(model3d.featurenames,1);
    
    x = zeros(numPts,1);
    y = zeros(numPts,1);
    z = zeros(numPts,1);
    
    for i=1:numPts
        fn = model3d.featurenames{i};
        pos = model3d.(fn);
        x(i) = pos(1);
        y(i) = pos(2);
        z(i) = pos(3);
    end

    figure;
    %plot3(x,y,z);
    %hold on;
    plot3(x,y,z,'rx');
    hold on;
    for i=1:numPts
        text(x(i),y(i),z(i),model3d.featurenames{i});
    end
end