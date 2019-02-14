function y = psiDerivative( x, epsilon )
% The function being considered here is y = psi( x )
% psi = "sqrt( x + eps )"
% Return value psi'(x) where ' represents derivative

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
	epsilon = 1e-3 ;
end

% Might be changed in the future to allow other possible functions.
y = 1 ./ (2 * sqrt( x + epsilon ) ) ;
