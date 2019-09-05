addpath('mex');
% load the frames
L202 = tonemap(hdrimread('clip_000007.000202.exr'));
L203 = tonemap(hdrimread('clip_000007.000203.exr'));
L204 = tonemap(hdrimread('clip_000007.000204.exr'));
L205 = tonemap(hdrimread('clip_000007.000205.exr'));
L206 = tonemap(hdrimread('clip_000007.000206.exr'));
H202 = hdrimread('clip_000007.000202.exr');
H206 = hdrimread('clip_000007.000206.exr');

% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)
alpha = 0.012;
ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;
para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

tic;
[vx,vy,warpI2] = Coarse2FineTwoFrames(L202,L203,para);%fwd
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L206,L205,para);%bwd
vx_2 = vx;
vy_2 = vy;
clear vx;
clear vy;
[vx,vy,warpI2] = Coarse2FineTwoFrames(L203,L204,para);%fwd
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L205,L204,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;

%[vx,vy,warpI2] = Coarse2FineTwoFrames(L203,L204,para,H202,H206, vx_2,vy_2);
%for fwd replace with first HDR frame
[x, y] = meshgrid(1:size(H202,2), 1:size(H202,1));
redChannel = H202(:, :, 1);
DR = interp2(redChannel, x-vx_2, y-vy_2);
greenChannel = H202(:, :, 2);
DG = interp2(greenChannel, x-vx_2, y-vy_2);
blueChannel = H202(:, :, 3);
DB = interp2(blueChannel, x-vx_2, y-vy_2);
newRGBImage = cat(3, DR, DG, DB);
hdrimwrite(newRGBImage, 'resultExtraFWD.hdr');
rgbExtra = tonemap(newRGBImage);
imwrite(rgbExtra, 'tonemappedExtraFWD.jpg');

%[vx,vy,warpI2] = Coarse2FineTwoFrames(L205,L204,para,H206,H202, vx_2,vy_2);

toc
%rgb = tonemap(warpI2);
%imwrite(rgb, 'tonemapped.jpg');
%hdrimwrite(warpI2, 'result.hdr');

% visualize flow field
clear flow;
flow(:,:,1) = vx_2;
flow(:,:,2) = vy_2;
imflow = flowToColor(flow);
%mean
meanImage = imfilter(flow, ones(3)/9);
sz = size(newRGBImage);
motion_conf_fwd = zeros(sz(1:2),'double');%fwd
%motion_conf_bwd = zeros(sz(1:2),'double');%bwd
for rows=1:size(meanImage,1)
    for cols=1:size(meanImage,2)
        first_angle= atan(newRGBImage(rows,cols,1)/newRGBImage(rows,cols,2));
        sec_angle= atan(meanImage(rows,cols,1)/meanImage(rows,cols,2));
        
        angle = first_angle-sec_angle;
        if (angle > 180)
            angle = angle-360;
        elseif (angle <-180)
            angle = angle+360;
        end
        motion_conf_fwd(rows,cols)= angle;%fwd
        %motion_conf_bwd(rows,cols)= angle;%bwd
    end
end

save('moco_02_04_fwd.mat','motion_conf_fwd');%fwd
%save('moco_06_04_bwd.mat','motion_conf_bwd');%bwd
figure;imshow(imflow);
