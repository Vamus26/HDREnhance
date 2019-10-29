function gc_example()

close all

% read images
%wlImage = hdrimread('flooowhdr_forward.hdr');
wlImage = hdrimread('Poker20FWD.hdr');%forw image
fi = tonemap(hdrimread('p404.hdr'));%simulated LDR frame
%wrImage = hdrimread('flooowhdr_back.hdr');
wrImage = hdrimread('Poker20BWD.hdr');%backw image
wlImageTone = tonemap(wlImage);%forw image tonemapped
wrImageTone = tonemap(wrImage);%backw image tonemapped
load('Poker20FWD.mat');
load('Poker20BWD.mat');
%image index %TODO set correct
frame_index_wl = 1;
frame_index_fi = 11;%3;%
frame_index_wr = 21;%5;%

k = 3;%# different labels

sz = size(fi);
Dc_new = zeros([sz(1:2) k],'single');

%Datacost function
for rows=1:size(fi,1)
    for cols=1:size(fi,2)
      eukl = double(wlImageTone(rows,cols) - fi(rows,cols));
      Dc_2 = norm(eukl);
      Df = motion_conf_fwd(rows,cols);%%1-1;%TODO change motion_confidence(wl(indx));
      Dd = abs(frame_index_wl-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
      Dc_new(rows,cols,1)=Dc_2+Df+Dd;
      %motion conf new, next 2 lines
      %load('motion_conf_200_210_backward.mat');
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
imwrite(rgb, 'Poker20RGB.jpg');
hdrimwrite(mix, 'Poker20HDR.hdr');
imshow(mix)
SSIM_HDR_OWN = ssim(mix,hdrimread('p404.hdr'));
SSIM_LDR_OWN = ssim(rgb,fi);
img_ref = hdrimread('p404.hdr');
img_dist = mix;
[img_ref , img_dist , ~, mxt] = checkDomains(img_ref , img_dist );
if(~exist('max_value', 'var'))
max_value = -1000; 
end
if(max_value < 0.0) 
max_value = mxt; 
end
mse = MSE(img_ref , img_dist );
if(mse > 0.0) 
psnr = 20 * log10(max_value / sqrt(mse)); 
else 
disp('PSNR: the images are the same!'); 
psnr = 1000; 
end 
X = sprintf('HDR-SSIM: %d PSNR:%d MSE:%d',SSIM_HDR_OWN,psnr,mse );
disp(X)

img_ref = fi;
img_dist = rgb;
[img_ref , img_dist , ~, mxt] = checkDomains(img_ref , img_dist );
if(~exist('max_value', 'var'))
max_value = -1000; 
end
if(max_value < 0.0) 
max_value = mxt; 
end
mse = MSE(img_ref , img_dist );
if(mse > 0.0) 
psnr = 20 * log10(max_value / sqrt(mse)); 
else 
disp('PSNR: the images are the same!'); 
psnr = 1000; 
end 
psnr = psnr(fi,rgb)
X = sprintf('LDR-SSIM: %d PSNR:%d MSE:%d',SSIM_LDR_OWN,psnr,mse);
disp(X)

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
    