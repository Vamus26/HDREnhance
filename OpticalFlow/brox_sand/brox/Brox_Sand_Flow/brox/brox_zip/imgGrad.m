function [xd, yd] = imgGrad( I , sigma )
% This function outputs the x-derivative and y-derivative of the
% input I. If I is 3D, then derivatives of each channel are
% available in xd and yd.

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
	sigma = 1.0 ; % default sigma for gaussian.
end
if ~isequal( class( I ), 'double' )
	I = double( I ) ;
end

gd = gaussDeriv( sigma ) ;

% Right now the convolution takes the middle part of the result
% This can be changed as per our requirement.
xd = convn( I, gd, 'same' ) ;
yd = convn( I, gd', 'same' ) ;
