addpath('mex');

% we provide two sequences "car" and "table"
example = 'BWD_LDR093_099_HDR093_099_SUMMED_FORWARD';
%example = 'car';

% load the two frames
im1 = hdrimread('br-00058_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');%im2double(imread('l093.jpg'));
im2 = hdrimread('br-00060_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');%im2double(imread('l095.jpg'));
im3 = hdrimread('br-00058.hdr');%im2double(imread('hdr-00093.tif'));
im4 = hdrimread('br-00066.hdr');%im2double(imread('hdr-00099.tif'));

% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)
alpha = 0.012;
ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;

para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

% this is the core part of calling the mexed dll file for computing optical flow
% it also returns the time that is needed for two-frame estimation
tic;
%original
[vx,vy,warpI2] = Coarse2FineTwoFrames(im1,im2,para);
%figure;imshow(warpI2);
%imwrite(warpI2,fullfile('output',[example '_orig_warped.tif']));
%clear flow;
%flow(:,:,1) = vx;
%flow(:,:,2) = vy;
%imflow = flowToColor(flow);

%figure;imshow(imflow);
%imwrite(imflow,fullfile('output',[example '_orig_flow.jpg']),'quality',100);
vx_2 = vx;
vy_2 = vy;
max(max(vx))
max(max(vy))
clear vx;
clear vy;

im1 = hdrimread('br-00060_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');%im2double(imread('l095.jpg'));
im2 = hdrimread('br-00062_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');%im2double(imread('l097.jpg'));
[vx,vy,warpI2] = Coarse2FineTwoFrames(im1,im2,para);

max(max(vx))
max(max(vy))
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
max(max(vx_2))
max(max(vy_2))
clear vx;
clear vy;

im1 = hdrimread('br-00062_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');%im2double(imread('l097.jpg'));
im2 = hdrimread('br-00066_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');%im2double(imread('l099.jpg'));
[vx,vy,warpI2] = Coarse2FineTwoFrames(im1,im2,para);
max(max(vx))
max(max(vy))
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
max(max(vx_2))
max(max(vy_2))
clear vx;
clear vy;

%[vx,vy,warpI2,warpI3] = Coarse2FineTwoFrames(im1,im2,para,im3,im4);
[vx,vy,warpI2] = Coarse2FineTwoFrames(im1,im2,para,im3,im4, vx_2,vy_2);
toc

%figure;imshow(im1);
figure;imshow(warpI2);
%figure;imshow(warpI3);
%K = imabsdiff(warpI2,warpI3);
%figure;imshow(K,[]);
rgb = tonemap(warpI2);
imwrite(rgb, 'mixrgb.jpg');
hdrimwrite(warpI2, 'mixim.hdr');

imwrite(warpI2,fullfile('output',[example '_ext_warped.tif']));
%imwrite(warpI3,fullfile('output',[example '_CALC.tif']));
%imwrite(warpI3,fullfile('output',[example '_HDR_warped.jpg']));
%imwrite(K,fullfile('output',[example '_differences.tif']));
%imwrite(K,fullfile('output',[example '_differences.jpg']));
% output gif
clear volume;
volume(:,:,:,1) = im2;
volume(:,:,:,2) = im1;
if exist('output','dir')~=7
    mkdir('output');
end
frame2gif(volume,fullfile('output',[example '_input.gif']));
volume(:,:,:,2) = warpI2;
volume(:,:,:,3) = im1;
frame2gif(volume,fullfile('output',[example '_warp.gif']));


% visualize flow field
clear flow;
flow(:,:,1) = vx_2;
flow(:,:,2) = vy_2;
imflow = flowToColor(flow);
%mean
meanImage = imfilter(flow, ones(3)/9);
sz = size(warpI2);
motion_conf = zeros(sz(1:2),'double');
for rows=1:size(meanImage,1)
    for cols=1:size(meanImage,2)
        first_angle= atan(warpI2(rows,cols,1)/warpI2(rows,cols,2));
        sec_angle= atan(meanImage(rows,cols,1)/meanImage(rows,cols,2));
        
        angle = first_angle-sec_angle;
        if (angle > 180)
            angle = angle-360;
        elseif (angle <-180)
            angle = angle+360;
        end
        
        motion_conf(rows,cols)= angle;
    end
end


figure;imshow(imflow);
imwrite(imflow,fullfile('output',[example '_ext_flow.jpg']),'quality',100);
