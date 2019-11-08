% Read in the image
equi = hdrimread('pano1.hdr');

% Close all figures and create cubic images
close all;
out = own_equi2cubic(equi, 1920); % Set to 500 x 500

% Now show all of the cube faces as a demonstration
names = {'Front Face', 'Right Face', 'Back Face', 'Left Face', ...
    'Top Face', 'Bottom Face'};
names_to_save = {'cube_front.hdr', 'cube_right.hdr', 'cube_back.hdr', ...
    'cube_left.hdr', 'cube_top.hdr', 'cube_bottom.hdr'};

% Show original equirectangular image
figure;
imshow(equi);
title('Equirectangular Image');

% Show the cube faces and also write them to disk
figure;
for idx = 1 : numel(names)
    % Show image in figure and name the face
    subplot(2,3,idx);
    imshow(out{idx});
    title(names{idx});
    % Write the image to disk
    hdrimwrite(out{idx}, names_to_save{idx});
end

% Show a montage
mont_image = horzcat(out{:});
figure;
imshow(mont_image);
title('Montage - Front, Right, Back, Left, Top, Bottom');