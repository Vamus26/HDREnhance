addpath('mex');

% we provide two sequences "car" and "table"
example = '9095sheartzzest';%'a';
%example = 'car';

% load the two frames
%im1 = im2double(imread('ldr90s.jpg'));%[example '1.png']));
%im2 = im2double(imread('ldr92s.jpg'));%[example '2.png']));
im1 = im2double(imread('ldr90.jpg'));%[example '1.png']));
im2 = im2double(imread('ldr706.jpg'));%[example '2.png']));

%im1 = imread('ldr90s.jpg');%[example '1.png']));
%im2 = imread('ldr95s.jpg');%[example '2.png']));

% im1 = imresize(im1,0.5,'bicubic');
% im2 = imresize(im2,0.5,'bicubic');

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
[vx,vy,warpI2] = Coarse2FineTwoFrames(im1,im2,para);
toc

%figure;imshow(im1);figure;imshow(warpI2);

imwrite(warpI2,fullfile('output',[example '_warped.tif']));

% output gif
clear volume;
volume(:,:,:,1) = im1;
volume(:,:,:,2) = im2;
if exist('output','dir')~=7
    mkdir('output');
end
frame2gif(volume,fullfile('output',[example '_input.gif']));
volume(:,:,:,2) = warpI2;
frame2gif(volume,fullfile('output',[example '_warp.gif']));


% visualize flow field
clear flow;
flow(:,:,1) = vx;
flow(:,:,2) = vy;


size(flow)
%im3 = im2double(imread('ldr95s.jpg'));
%size(im3)
%tmp1 = im3(:,:,1)+vx;
%tmp2 = im3(:,:,1).*vy;
%imX(:,:,1)= tmp1;%.*tmp2;
%tmp1 = im3(:,:,2)+vx;
%tmp2 = im3(:,:,2).*vy;
%imX(:,:,2)= tmp1;%.*tmp2;
%tmp1 = im3(:,:,3)+vx;
%tmp2 = im3(:,:,3).*vy;
%imX(:,:,3)= tmp1;%.*tmp2;

%im3= imread('ldr95s.jpg'); % test image
%[x, y] = meshgrid(1:size(im3,2), 1:size(im3,1));
% generate synthetic test data, for experimenting
%vx = 0.1*y;   % an arbitrary flow field, in this case
%vy = 0.1*x;   % representing shear
% compute the warped image - the subtractions are because we're specifying
% where in the original image each pixel in the new image comes from
%imX = interp2(im3, x-vx, y-vy);
% display the result
%imshow(imX, [])
%[x, y] = meshgrid(1:size(im2,2), 1:size(im2,1));
%imX = interp2(x,y,double(im2), x-vx, y-vy);

%C= im2double(imread('ldr95s.jpg')); % test image
C= im2double(imread('-02690.tif'));
[x, y] = meshgrid(1:size(C,2), 1:size(C,1));
% generate synthetic test data, for experimenting
%vx = 0.1*y;   % an arbitrary flow field, in this case
%vy = 0.1*x;   % representing shear
% compute the warped image - the subtractions are because we're specifying
% where in the original image each pixel in the new image comes from
imX(:,:,1) = interp2(double(C(:,:,1)), x-vx, y-vy);
imX(:,:,2) = interp2(double(C(:,:,2)), x-vx, y-vy);
imX(:,:,3) = interp2(double(C(:,:,3)), x-vx, y-vy);
% display the result
imshow(imX, [])




%vx = vx/mean(vx);
%vx = normalize(vx);
%vy_N = norm(vy)
%vy = vy/mean(vy)
%[r, c, ~] = size(img);
%[M,N,~]=size(im2);
%[x,y]=meshgrid(1:M,1:N);
%[x, y] = meshgrid(1:size(im2,2), 1:size(im2,1));
%warpI3=interp2(x,y,im2,x+vx,y+vy,'*nearest'); % use Matlab interpolation routine
%warpI2=interp2(x,y,i2,x+vx,y+vy,'*linear'); % use Matlab interpolation routine
%I=find(isnan(warpI2));
%warpI2(I)=warpI3(I);


imflow = flowToColor(flow);
figure;imshow(imflow);
%imX = immultiply(im3, imflow);

%size(imX)
imwrite(imX,fullfile('output',[example 'imX.tif']));

imwrite(imflow,fullfile('output',[example '_flow.tif']));%,'quality',100);
