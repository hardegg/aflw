function vecs = rotateToPoses(vec,poses)

    vecs = vec;
    for i=1:size(poses,1)
        vecs(i,:) = rotateToPose(vec,poses(i,:));
    end

end