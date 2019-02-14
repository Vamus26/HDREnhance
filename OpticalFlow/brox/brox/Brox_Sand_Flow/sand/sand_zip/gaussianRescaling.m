function img_scaled = gaussianRescaling(img, scale_factor, sigma)

% Copyright (c)2007 Visesh Chari <visesh [at] research.iiit.net>
% Centre for Visual Information Technology
% International Institute of Information Technology
% http://cvit.iiit.ac.in/
% http://students.iiit.ac.in/~ukvisesh
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% The Software is provided "as is", without warranty of any kind.



if nargin < 3
	sigma = 1.0; % default.
end

if scale_factor < eps
	disp('Too small a scaling factor!!') ;
	return ;
end

% Now apply Gaussian smoothing to the input image. 
img_smooth = gaussianSmooth(img, 1.0/scale_factor, 1e-3) ;

% Now create and fill the new image
img_scaled = imresize_old(img_smooth, scale_factor, 'bilinear', 0);
