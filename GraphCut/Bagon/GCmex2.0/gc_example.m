function gc_example()
% An example of how to segment a color image according to pixel colors.
% Fisrt stage identifies k distinct clusters in the color space of the
% image. Then the image is segmented according to these regions; each pixel
% is assigned to its cluster and the GraphCut poses smoothness constraint
% on this labeling.

close all

% read an image
%im = im2double(imread('outdoor_small.jpg'));
fi = hdrimread('br-00062_pregamma_1_reinhard05_brightness_-10_chromatic_adaptation_0_light_adaptation_1.jpg');
%fi = im2double(imread('outdoor_small.jpg'));%'IMGcamera_0-vid-00000.tif'));%ldr image
wlImage = hdrimread('br-00058.hdr');
%wlImage = im2double(imread('outdoor_small2.jpg'));%im2double(imread('IMGcamera_0-vid-00000.tif'));%forw image
wrImage = hdrimread('br-00066.hdr');
%wrImage = im2double(imread('outdoor_small3.jpg'));%im2double(imread('IMGcamera_0-vid-00000.tif'));%backw image
%wl = ToVector(wlImage);
%wr = ToVector(wrImage);
%fiV = ToVector(fi);
frame_index_wl = 58;%0;
frame_index_wr = 62;%10;
frame_index_fi = 66;%5;

%Dres = zeros( size(wl) );
%Dvec = ToVector(Dres);
sz = size(fi);

% try to segment the image into k different regions
k = 3;
c = 0.3;

% color space distance
distance = 'sqEuclidean';

% cluster the image colors into k regions
data = ToVector(fi);
[idx c] = kmeans(data, k, 'distance', distance,'maxiter',200);

% calculate the data cost per cluster center
Dc = zeros([sz(1:2) k],'single');
Dc_new = zeros([sz(1:2) k],'single');
%Dc_new(:,:,2) = c;

%Datacost function
for rows=1:size(fi,1)
    for cols=1:size(fi,2)
      Dc_2 = norm(wlImage(rows,cols) - fi(rows,cols));
      Df = 1-1;%TODO change motion_confidence(wl(indx));
      Dd = abs(frame_index_wl-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
      Dc_new(rows,cols,1)=Dc_2+Df+Dd;
      Dc_2 = norm(wrImage(rows,cols) - fi(rows,cols));
      Dd = abs(frame_index_wr-frame_index_fi) / abs(frame_index_wr-frame_index_wl);
      Dc_new(rows,cols,3)=Dc_2+Df+Dd;
      Dc_new(rows,cols,2) = 5;%0.3;
    end
end

for ci=1:k
    % use covariance matrix per cluster
    icv = inv(cov(data(idx==ci,:)));    
    dif = data - repmat(c(ci,:), [size(data,1) 1]);
    % data cost is minus log likelihood of the pixel to belong to each
    % cluster according to its RGB value
    Dc(:,:,ci) = reshape(sum((dif*icv).*dif./2,2),sz(1:2));
end


%for indx=1:length(Dvec) 
%end


%loss functions:
%Dc(p,l) =||wl(p)-fi(p)|| eukl. dist Dc = norm(wl - fi)
%Df(p,l) = 1-motion confidence(wl(p)) 
%Dd(l,fi) = |frame index(wl)-i| / |frame index(wright)-frame index(wleft)|

%function [vals,derivs] = myCostFunc(params)
% Extract the current design variable values from the parameter object, params.
%x = params.Value;
% Compute the requirements (objective and constraint violations) and 
% assign them to vals, the output of the cost function. 
%vals.F = x.^2;
%vals.Cleq = x.^2-4*x+1;
% Compute the cost and constraint derivatives.
%derivs.F = 2*x;
%derivs.Cleq = 2*x-4;
%end

% cut the graph

% smoothness term: 
% constant part
Sc = ones(k) - eye(k);
% spatialy varying part
% [Hc Vc] = gradient(imfilter(rgb2gray(im),fspecial('gauss',[3 3]),'symmetric'));
[Hc Vc] = SpatialCues(fi);
%my smoothnessfkt
Hc_new = zeros(sz(1:2),'single');
Vc_new = zeros(sz(1:2),'single');
for rows=1:size(fi,1)
    for cols=1:size(fi,2)
        if (cols-1)<=0
            Hc_new(rows,cols)= inf;
        else
        	Hc_new(rows,cols)= smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows,cols-1,1), wlImage(rows,cols-1,2), wlImage(rows,cols-1,3),wrImage(rows,cols-1,1),wrImage(rows,cols-1,2),wrImage(rows,cols-1,3)); 
        end
        if (rows-1)<=0
            Vc_new(rows,cols)= inf;
        else
        	Vc_new(rows,cols)= smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows-1,cols,1), wlImage(rows-1,cols,2), wlImage(rows-1,cols,3),wrImage(rows-1,cols,1),wrImage(rows-1,cols,2),wrImage(rows-1,cols,3));
        end
        if (cols+1)>size(fi,2)
        	Hc_new(rows,cols)= Hc_new(rows,cols)+ inf;
        else
            Hc_new(rows,cols)= Hc_new(rows,cols)+smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows,cols+1,1), wlImage(rows,cols+1,2), wlImage(rows,cols+1,3),wrImage(rows,cols+1,1),wrImage(rows,cols+1,2),wrImage(rows,cols+1,3));
        end
        if (rows+1)>size(fi,1)
        	Vc_new(rows,cols)= Vc_new(rows,cols)+inf;
        else
        	Vc_new(rows,cols)= Vc_new(rows,cols)+smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows+1,cols,1), wlImage(rows+1,cols,2), wlImage(rows+1,cols,3),wrImage(rows+1,cols,1),wrImage(rows+1,cols,2),wrImage(rows+1,cols,3));
        end
    end
end
%gch = GraphCut('open', Dc, 10*Sc, exp(-Vc*5), exp(-Hc*5));
%gch = GraphCut('open', Dc_new, Sc, exp(-Vc*5), exp(-Hc*5));
gch = GraphCut('open', Dc_new, Sc, Vc_new, Hc_new);
[gch L] = GraphCut('expand',gch);
[gch se de] = GraphCut('energy', gch)
[gch e] = GraphCut('energy', gch)
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
imwrite(rgb, 'mixrgb.jpg');
hdrimwrite(mix, 'mixim.hdr');
imshow(mix)

%---------------- Aux Functions ----------------%
function v = ToVector(im)
% takes MxNx3 picture and returns (MN)x3 vector
sz = size(im);
v = reshape(im, [prod(sz(1:2)) 3]);

%-----------------------------------------------%
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
function [hC vC] = SpatialCues(im)
g = fspecial('gauss', [13 13], sqrt(13));
dy = fspecial('sobel');
vf = conv2(g, dy, 'valid');
sz = size(im);

vC = zeros(sz(1:2));
hC = vC;

for b=1:size(im,3)
    vC = max(vC, abs(imfilter(im(:,:,b), vf, 'symmetric')));
    hC = max(hC, abs(imfilter(im(:,:,b), vf', 'symmetric')));
end
%-----------------------------------------------%
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
    