function alphaImg = getalphaImg( img, alpha_cov )

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


if nargin < 2
	alpha_cov = 2.0 ;
end

img_g = rgb2gray( img ) ;
[ix, iy] = imgGrad( img_g ) ;

alphaImg = ix .^ 2 + iy .^ 2 ;
alphaImg = exp( -alphaImg / ( 2 * alpha_cov .^ 2 ) ) ;
alphaImg = alphaImg / sqrt( 2 * pi * alpha_cov .^ 2 ) ;
