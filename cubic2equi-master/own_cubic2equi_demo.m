left = hdrimread('cube_left.hdr');
right = hdrimread('cube_right.hdr');
top = hdrimread('cube_top.hdr');
bottom = hdrimread('cube_bottom.hdr');
front = hdrimread('cube_front.hdr');
back = hdrimread('cube_back.hdr');

close all;
out = own_cubic2equi(top, bottom, left, right, front, back);
imshow(out);
hdrimwrite(out, 'combined_cube.hdr');