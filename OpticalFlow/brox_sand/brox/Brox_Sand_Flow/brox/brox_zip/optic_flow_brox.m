function [u, v] = optic_flow_brox(img1, img2, alpha, gamma, num_levels, rescale,isshow) % modify by yy [u v x y im] output

% alpha       = 10 ; % Global smoothness variable.
% gamma       = 100.0 ; % Global weight for derivatives.
% rescale     = 0.8;
% num_levels  = 3 ; % for face; Number of pyramid levels. Can also be calculated automatically

[ht, wt, dt] = size( img1 ) ;

im1_hr = gaussianRescaling( img1, power( rescale, num_levels ) ) ; % Rescaling images
im2_hr = gaussianRescaling( img2, power( rescale, num_levels ) ) ; % to the top of the
% laplacian pyramid.
u = zeros(size(rgb2gray(im1_hr))); % Initialization.%%%%modify by yy
v = zeros(size(rgb2gray(im1_hr)));
% u = zeros(size(im1_hr)); % Initialization. %%%yy
% v = zeros(size(im1_hr));                %%yy
for i = num_levels-1 : -1 : 1

    I1 = rgb2gray(im1_hr) ; %modify by yy (ori)
    I2 = rgb2gray(im2_hr) ;
%     I1 = im1_hr; I2 = im2_hr; modify by yy
    % Computing derivatives.
    [Ikx Iky] = imgGrad( I2 ) ;
    [Ikx2 Iky2] = imgGrad( I1 ) ;
    Ikz = double(I2) - double(I1) ;
    Ixz = double(Ikx) - double(Ikx2) ;
    Iyz = double(Iky) - double(Iky2) ;

    % Calling the processing for a particular resolution.
    % Last two arguments are the outer and inner iterations, respectively.
    % 1.8 is the omega value for the SOR iteration.
    [du, dv] = resolutionProcess_brox( Ikz, Ikx, Iky, Ixz, Iyz, alpha, gamma, 1.8, u, v, 3, 500 ) ;

    % Adding up the optical flow.
    u = u + du ;
    v = v + dv ;

    im1_hr = gaussianRescaling( img1, power( rescale, i ) ) ;
    im2_hr = gaussianRescaling( img2, power( rescale, i ) ) ;
    im2_orig = im2_hr ; % Original image without warping for comparison.

    % Resize optical flow to current resolution.
    u = imresize( u, [size(im1_hr, 1), size(im1_hr, 2)], 'bilinear' ) ;
    v = imresize( v, [size(im1_hr, 1), size(im1_hr, 2)], 'bilinear' ) ;

    [h w d] = size(im2_hr) ;                   %%% modify by yy(ori)
    [x y]   = meshgrid( 1:w, 1:h );

    im2_hr = uint8( mywarp_rgb( double( im2_hr ), u, v ) ) ; % taking im1_hr closer to im2_hr.

    if isshow
        % Displaying relevant figures.
        figure(2);
        subplot(3, 3, 1); imshow(im1_hr) ;
        subplot(3, 3, 2); imshow(rgb2gray(im2_hr)) ;
        subplot(3, 3, 3); imshow(rgb2gray(im2_orig)) ;
        subplot(3, 3, 4); imshow(uint8(double(im1_hr)-double(im2_hr))) ;
        subplot(3, 3, 5); imshow(uint8(double(im1_hr)-double(im2_orig))) ;
        subplot(3, 3, 6); imshow(uint8(double(im2_hr)-double(im2_orig))) ;
        subplot(3, 3, 7); imagesc(u) ;
        subplot(3, 3, 8); imagesc(v) ;
        subplot(3, 3, 9); imagesc(sqrt(u.^2 + v.^2)) ;
    end
    % pause;
end

im      = im1_hr;

