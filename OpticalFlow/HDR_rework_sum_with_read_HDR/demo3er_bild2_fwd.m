addpath('mex');
% load the frames
L196 = tonemap(hdrimread('p404.hdr'));
L195 = tonemap(hdrimread('p403.hdr'));
L194 = tonemap(hdrimread('p402.hdr'));

%L196 = tonemap(hdrimread('clip_000007.000196.exr'));
%L197 = tonemap(hdrimread('clip_000007.000195.exr'));
%L198 = tonemap(hdrimread('clip_000007.000194.exr'));
%H214 = hdrimread('clip_000007.000194.exr');
H214 = hdrimread('p402.hdr');
% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)
alpha = 0.012;
ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;
para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

tic;
[vx,vy,warpI2] = Coarse2FineTwoFrames(L194,L195,para);%fwd
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L198,L197,para);%bwd
vx_2 = vx;
vy_2 = vy;
clear vx;
clear vy;
[vx,vy,warpI2] = Coarse2FineTwoFrames(L195,L196,para);%fwd
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L197,L196,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;

%for fwd replace with first HDR frame
[x, y] = meshgrid(1:size(H214,2), 1:size(H214,1));
redChannel = H214(:, :, 1);
DR = interp2(redChannel, x-vx_2, y-vy_2);
greenChannel = H214(:, :, 2);
DG = interp2(greenChannel, x-vx_2, y-vy_2);
blueChannel = H214(:, :, 3);
DB = interp2(blueChannel, x-vx_2, y-vy_2);
newRGBImage = cat(3, DR, DG, DB);
hdrimwrite(newRGBImage, 'Poker3FWD.hdr');
rgbExtra = tonemap(newRGBImage);
imwrite(rgbExtra, 'Poker3FWD.jpg');

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
      %  motion_conf_bwd(rows,cols)= angle;%bwd
    end
end

save('Poker3FWD.mat','motion_conf_fwd');%fwd
%save('RiverBWD3.mat','motion_conf_bwd');%bwd
figure;imshow(imflow);
