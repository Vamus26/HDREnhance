% Read in the image
%equi = hdrimread('pano_end.hdr');
equi = imread('panos-00617.jpg');
% Close all figures and create cubic images
close all;
out = own_equi2cubic(equi, 1920); % Set to 500 x 500

% Now show all of the cube faces as a demonstration
names = {'Front Face', 'Right Face', 'Back Face', 'Left Face', ...
    'Top Face', 'Bottom Face'};
%names_to_save = {'cube_front617.hdr', 'cube_right617.hdr', 'cube_back617.hdr', ...
%    'cube_left617.hdr', 'cube_top617.hdr', 'cube_bottom617.hdr'};
names_to_save = {'cube_front617.jpg', 'cube_right617.jpg', 'cube_back617.jpg', ...
    'cube_left617.jpg', 'cube_top617.jpg', 'cube_bottom617.jpg'};

% Show original equirectangular image
%figure;
%imshow(equi);
%title('Equirectangular Image');

% Show the cube faces and also write them to disk
%figure;
for idx = 1 : numel(names)
    % Show image in figure and name the face
    subplot(2,3,idx);
    imshow(out{idx});
    title(names{idx});
    % Write the image to disk
   % hdrimwrite(out{idx}, names_to_save{idx});
    imwrite(out{idx}, names_to_save{idx});
end

% Show a montage
%mont_image = horzcat(out{:});
%figure;
%imshow(mont_image);
%title('Montage - Front, Right, Back, Left, Top, Bottom');