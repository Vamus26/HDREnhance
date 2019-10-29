addpath('mex');
% load the frames
L194 = tonemap(hdrimread('p394.hdr'));
L195 = tonemap(hdrimread('p395.hdr'));
L196 = tonemap(hdrimread('p396.hdr'));
L197 = tonemap(hdrimread('p397.hdr'));
L198 = tonemap(hdrimread('p398.hdr'));
L199 = tonemap(hdrimread('p399.hdr'));
L200 = tonemap(hdrimread('p400.hdr'));
L201 = tonemap(hdrimread('p401.hdr'));
L202 = tonemap(hdrimread('p402.hdr'));
L203 = tonemap(hdrimread('p403.hdr'));
L204 = tonemap(hdrimread('p404.hdr'));
L205 = tonemap(hdrimread('p405.hdr'));
L206 = tonemap(hdrimread('p406.hdr'));
L207 = tonemap(hdrimread('p407.hdr'));
L208 = tonemap(hdrimread('p408.hdr'));
L209 = tonemap(hdrimread('p409.hdr'));
L210 = tonemap(hdrimread('p410.hdr'));
L211 = tonemap(hdrimread('p411.hdr'));
L212 = tonemap(hdrimread('p412.hdr'));
L213 = tonemap(hdrimread('p413.hdr'));
L214 = tonemap(hdrimread('p414.hdr'));


H214 = hdrimread('p414.hdr');
% H203 = hdrimread('clip_000007.000203.exr');
% H205 = hdrimread('clip_000007.000205.exr');
%H206 = hdrimread('clip_000007.000206.exr');

% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)
alpha = 0.012;
ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;
para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

tic;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L194,L195,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L214,L213,para);%bwd
vx_2 = vx;
vy_2 = vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L195,L196,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L213,L212,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L196,L197,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L212,L211,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L197,L198,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L211,L210,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L198,L199,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L210,L209,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L199,L200,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L209,L208,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L200,L201,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L208,L207,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L201,L202,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L207,L206,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L202,L203,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L206,L205,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;
%[vx,vy,warpI2] = Coarse2FineTwoFrames(L203,L204,para);%fwd
[vx,vy,warpI2] = Coarse2FineTwoFrames(L205,L204,para);%bwd
vx_2 = vx_2+vx;
vy_2 = vy_2+vy;
clear vx;
clear vy;

%[vx,vy,warpI2] = Coarse2FineTwoFrames(L203,L204,para,H202,H206, vx_2,vy_2);
%for fwd replace with first HDR frame
[x, y] = meshgrid(1:size(H214,2), 1:size(H214,1));
redChannel = H214(:, :, 1);
DR = interp2(redChannel, x-vx_2, y-vy_2);
greenChannel = H214(:, :, 2);
DG = interp2(greenChannel, x-vx_2, y-vy_2);
blueChannel = H214(:, :, 3);
DB = interp2(blueChannel, x-vx_2, y-vy_2);
newRGBImage = cat(3, DR, DG, DB);
hdrimwrite(newRGBImage, 'Poker20BWD.hdr');
rgbExtra = tonemap(newRGBImage);
imwrite(rgbExtra, 'Poker20BWD.jpg');

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
%motion_conf_fwd = zeros(sz(1:2),'double');%fwd
motion_conf_bwd = zeros(sz(1:2),'double');%bwd
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
       % motion_conf_fwd(rows,cols)= angle;%fwd
        motion_conf_bwd(rows,cols)= angle;%bwd
    end
end

%save('BridgeFWD20_94_04_fwd.mat','motion_conf_fwd');%fwd
save('Poker20BWD.mat','motion_conf_bwd');%bwd
figure;imshow(imflow);
