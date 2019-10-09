addpath('mex');
% load the frames
L194 = tonemap(hdrimread('clip_000007.000194.exr'));
L195 = tonemap(hdrimread('clip_000007.000195.exr'));
L196 = tonemap(hdrimread('clip_000007.000196.exr'));
L197 = tonemap(hdrimread('clip_000007.000197.exr'));
L198 = tonemap(hdrimread('clip_000007.000198.exr'));
L199 = tonemap(hdrimread('clip_000007.000199.exr'));
L200 = tonemap(hdrimread('clip_000007.000200.exr'));
L201 = tonemap(hdrimread('clip_000007.000201.exr'));
L202 = tonemap(hdrimread('clip_000007.000202.exr'));
L203 = tonemap(hdrimread('clip_000007.000203.exr'));
L204 = tonemap(hdrimread('clip_000007.000204.exr'));
L205 = tonemap(hdrimread('clip_000007.000205.exr'));
L206 = tonemap(hdrimread('clip_000007.000206.exr'));
L207 = tonemap(hdrimread('clip_000007.000207.exr'));
L208 = tonemap(hdrimread('clip_000007.000208.exr'));
L209 = tonemap(hdrimread('clip_000007.000209.exr'));
L210 = tonemap(hdrimread('clip_000007.000210.exr'));
L211 = tonemap(hdrimread('clip_000007.000211.exr'));
L212 = tonemap(hdrimread('clip_000007.000212.exr'));
L213 = tonemap(hdrimread('clip_000007.000213.exr'));
L214 = tonemap(hdrimread('clip_000007.000214.exr'));


H214 = hdrimread('clip_000007.000214.exr');
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
hdrimwrite(newRGBImage, 'resultExtraBWD10.hdr');
rgbExtra = tonemap(newRGBImage);
imwrite(rgbExtra, 'tonemappedExtraBWD10.jpg');

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

%save('moco_94_04_fwd.mat','motion_conf_fwd');%fwd
save('moco_14_04_bwd.mat','motion_conf_bwd');%bwd
figure;imshow(imflow);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GCPART%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function gc_example()

close all

% read images
wlImage = hdrimread('resultExtraFWD10.hdr');%forw image
fi = tonemap(hdrimread('clip_000007.000204.exr'));%simulated LDR frame
wrImage = hdrimread('resultExtraBWD10.hdr');%backw image
wlImageTone = tonemap(wlImage);%forw image tonemapped
wrImageTone = tonemap(wrImage);%backw image tonemapped
load('moco_94_04_fwd.mat');
load('moco_14_04_bwd.mat');
%image index
frame_index_wl = 4;
frame_index_fi = 14;
frame_index_wr = 24;

k = 3;%# different labels

sz = size(fi);
Dc_new = zeros([sz(1:2) k],'single');

%Datacost function
for rows=1:size(fi,1)
    for cols=1:size(fi,2)
      eukl = double(wlImageTone(rows,cols) - fi(rows,cols));
      Dc_2 = norm(eukl);
      Df = motion_conf_fwd(rows,cols);
      Dd = abs(frame_index_wl-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
      Dc_new(rows,cols,1)=Dc_2+Df+Dd;
      Df = motion_conf_bwd(rows,cols);
      eukl = double(wrImageTone(rows,cols) - fi(rows,cols));
      Dc_2 = norm(eukl);
      Dd = abs(frame_index_wr-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
      Dc_new(rows,cols,3)=Dc_2+Df+Dd;
      Dc_new(rows,cols,2) = 500;%0.3 originally
    end
end

%smoothness function
Sc_new = [1,1,1;1,1,1;1,1,1];
Hc_new = zeros(sz(1:2),'single');%horizontal cost
Vc_new = zeros(sz(1:2),'single');%vertical cost
for rows=1:size(fi,1)
    for cols=1:size(fi,2)
        if (cols-1)<=0
            Hc_new(rows,cols)= 99999;%inf;
        else
        	Hc_new(rows,cols)= smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows,cols-1,1), wlImage(rows,cols-1,2), wlImage(rows,cols-1,3),wrImage(rows,cols-1,1),wrImage(rows,cols-1,2),wrImage(rows,cols-1,3)); 
        end
        if (rows-1)<=0
            Vc_new(rows,cols)= 99999;%inf;
        else
        	Vc_new(rows,cols)= smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows-1,cols,1), wlImage(rows-1,cols,2), wlImage(rows-1,cols,3),wrImage(rows-1,cols,1),wrImage(rows-1,cols,2),wrImage(rows-1,cols,3));
        end
%TODO: maybe use at edges
%        if (cols+1)>size(fi,2)
%        	Hc_new(rows,cols)= Hc_new(rows,cols)+ inf;
%        else
%            Hc_new(rows,cols)= Hc_new(rows,cols)+smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows,cols+1,1), wlImage(rows,cols+1,2), wlImage(rows,cols+1,3),wrImage(rows,cols+1,1),wrImage(rows,cols+1,2),wrImage(rows,cols+1,3));
%        end
%        if (rows+1)>size(fi,1)
%        	Vc_new(rows,cols)= Vc_new(rows,cols)+inf;
%        else
%        	Vc_new(rows,cols)= Vc_new(rows,cols)+smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows+1,cols,1), wlImage(rows+1,cols,2), wlImage(rows+1,cols,3),wrImage(rows+1,cols,1),wrImage(rows+1,cols,2),wrImage(rows+1,cols,3));
%        end
    end
end

%graphcut
gch = GraphCut('open', Dc_new, Sc_new, Vc_new, Hc_new);
[gch L] = GraphCut('expand',gch); %labels from 0 to (labels-1)- so 0,1,2 here
[gch se de] = GraphCut('energy', gch)
[gch e] = GraphCut('energy', gch)
[gch L] = GraphCut('swap', gch);
gch = GraphCut('close', gch);

% show results
imshow(fi);
hold on;
PlotLabels(L);
%combine labels per pixel for result
mix = zeros([sz(1:2) k],'double');
for rows=1:size(Dc_new,1)
    for cols=1:size(Dc_new,2)
        if (Dc_new(rows,cols,1)<Dc_new(rows,cols,2))&&(Dc_new(rows,cols,1)<Dc_new(rows,cols,3))
           mix(rows,cols,:)= wlImage(rows,cols,:);
        elseif (Dc_new(rows,cols,3)<Dc_new(rows,cols,2))&&(Dc_new(rows,cols,3)<Dc_new(rows,cols,1))
            mix(rows,cols,:)= wrImage(rows,cols,:);
        else
            mix(rows,cols,:)= fi(rows,cols,:);
        end
    end
end
rgb = tonemap(mix);
imwrite(rgb, 'mixrgb_with_extract_1frame10.jpg');
hdrimwrite(mix, 'mixim_with_extract_1frame10.hdr');

%---------------- Aux Functions ----------------%
function ih = PlotLabels(L)

L = single(L);

bL = imdilate( abs( imfilter(L, fspecial('log'), 'symmetric') ) > 0.1, strel('disk', 1));
LL = zeros(size(L),class(L));
LL(bL) = L(bL);
Am = zeros(size(L));
Am(bL) = .5;
ih = imagesc(LL); 
set(ih, 'AlphaData', Am);
colorbar;
colormap 'jet';

%-----------------------------------------------%
%unused so far
function [cost_mat] = myCostFunc(pxl,label)
    if label ==fi
        cost_mat(pxl)=c;
    elseif label(pxl)==null
        cost_mat(pxl)=inf;
    else
        Dc = norm(label(pxl) - fi(pxl));
        Df = 1-1;%TODO change motion_confidence(wl(indx));
        if label ==wl
            Dd = abs(frame_index_wl-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
        else
            Dd = abs(frame_index_wr-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
        end
        cost_mat(pxl)=Dc+Df+Dd;
    end
%-----------------------------------------------%

function [cost_mat] = smoothCostGrad(s1r,s1g,s1b,s2r,s2g,s2b,t1r,t1g,t1b,t2r,t2g,t2b)
	gradTextR  = abs( s1r - t1r );
	gradTextV  = abs( s1g - t1g );
	gradTextB  = abs( s1b - t1b );
	gradPatchR  = abs( s2r - t2r );
	gradPatchV  = abs( s2g - t2g );
	gradPatchB  = abs( s2b - t2b );

	grad = ((gradTextR + gradTextV + gradTextB) / 3.0 )+ (( gradPatchR + gradPatchV + gradPatchB ) / 3.0);
	grad= grad +1.0; % to avoid zero division 

    %smoothCostBasic
    cr = abs( s1r - s2r ) +  abs( t1r - t2r );
	cg = abs( s1g - s2g ) +  abs( t1g - t2g );
	cb = abs( s1b - s2b ) +  abs( t1b - t2b );
	tmp =( ( cr + cg + cb ) / 3.0 );
    
	cost_mat = (tmp/ sqrt(grad));  
    

