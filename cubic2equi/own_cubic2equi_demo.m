left = hdrimread('STITCHEDld080-leftexp.hdr');
right = hdrimread('STITCHEDld080-rightexp.hdr');
top = hdrimread('STITCHEDld080-topexp.hdr');
bottom = hdrimread('STITCHEDld080-bottomexp.hdr');
front = hdrimread('STITCHEDld080-frontexp.hdr');
back = hdrimread('STITCHEDld080-backexp.hdr');

close all;
out = own_cubic2equi(top, bottom, left, right, front, back);
imshow(out);
hdrimwrite(out, 'l80-comp.hdr');