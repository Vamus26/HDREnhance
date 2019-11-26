left = hdrimread('CreatedHDRTonemapped_cube_left2.jpg');
right = hdrimread('CreatedHDRTonemapped_cube_right2.jpg');
top = hdrimread('CreatedHDRTonemapped_cube_top2.jpg');
bottom = hdrimread('CreatedHDRTonemapped_cube_bottom2.jpg');
front = hdrimread('CreatedHDRTonemapped_cube_front2.jpg');
back = hdrimread('CreatedHDRTonemapped_cube_back2.jpg');

close all;
out = own_cubic2equi(top, bottom, left, right, front, back);
imshow(out);
hdrimwrite(out, 'CreatedHDRTonemapped_combined_cube2.jpg');