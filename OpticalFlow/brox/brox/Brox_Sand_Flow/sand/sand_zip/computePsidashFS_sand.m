function psidashFS = computePsidashFS_brox( u, v, alphaImg )

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



[h, w] = size( u ) ;
psidashFS = zeros( 2 * h + 1, 2 * w + 1 ) ;
alpha = imresize( alphaImg, [2*h+1, 2*w+1], 'bilinear' ) ;

ux = convn( u, [1 -1] ) ;
uy = convn( u, [1 -1]' ) ;

vx = convn( v, [1 -1] ) ;
vy = convn( v, [1 -1]' ) ;

uxd = convn( ux, [1 1] / 2, 'valid' ) ;
vxd = convn( vx, [1 1] / 2, 'valid' ) ;

uyd = convn( uy, [1 1]' / 2, 'valid' ) ;
vyd = convn( vy, [1 1]' / 2, 'valid' ) ;


t = convn( uyd, [1 1] / 2 ) ;
uxpd = ux .^ 2 + t .^ 2 ;
t = convn( uxd, [1 1]' / 2 ) ;
uypd = uy .^ 2 + t .^ 2 ;

t = convn( vyd, [1 1] / 2 ) ;
vxpd = vx .^ 2 + t .^ 2 ;
t = convn( vxd, [1 1]' / 2 ) ;
vypd = vy .^ 2 + t .^ 2 ;

psidashFS( 1:2:end, 2:2:end ) = psiDerivative( uypd + vypd ) ;
psidashFS( 2:2:end, 1:2:end ) = psiDerivative( uxpd + vxpd ) ;

psidashFS = psidashFS .* alpha ;
