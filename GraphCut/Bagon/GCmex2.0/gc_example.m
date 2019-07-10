function gc_example()
% An example of how to segment a color image according to pixel colors.
% Fisrt stage identifies k distinct clusters in the color space of the
% image. Then the image is segmented according to these regions; each pixel
% is assigned to its cluster and the GraphCut poses smoothness constraint
% on this labeling.

close all

% read an image
%im = im2double(imread('outdoor_small.jpg'));
fi = im2double(imread('outdoor_small.jpg'));%'IMGcamera_0-vid-00000.tif'));%ldr image
%wlImage = im2double(imread('IMGcamera_0-vid-00000.tif'));%forw image
%wrImage = im2double(imread('IMGcamera_0-vid-00000.tif'));%backw image
%wl = ToVector(wlImage);
%wr = ToVector(wrImage);
%fiV = ToVector(fi);
frame_index_wl = 0;
frame_index_wr = 10;
frame_index_fi = 5;

%Dres = zeros( size(wl) );
%Dvec = ToVector(Dres);
sz = size(fi);

% try to segment the image into k different regions
k = 3;

% color space distance
distance = 'sqEuclidean';

% cluster the image colors into k regions
data = ToVector(fi);
[idx c] = kmeans(data, k, 'distance', distance,'maxiter',200);

% calculate the data cost per cluster center
Dc = zeros([sz(1:2) k],'single');
for ci=1:k
    % use covariance matrix per cluster
    icv = inv(cov(data(idx==ci,:)));    
    dif = data - repmat(c(ci,:), [size(data,1) 1]);
    % data cost is minus log likelihood of the pixel to belong to each
    % cluster according to its RGB value
    Dc(:,:,ci) = reshape(sum((dif*icv).*dif./2,2),sz(1:2));
end

c = 0.3;
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

gch = GraphCut('open', Dc, 10*Sc, exp(-Vc*5), exp(-Hc*5));
[gch L] = GraphCut('expand',gch);
[gch se de] = GraphCut('energy', gch)
[gch e] = GraphCut('energy', gch)
gch = GraphCut('close', gch);

% show results
imshow(fi);
hold on;
PlotLabels(L);



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