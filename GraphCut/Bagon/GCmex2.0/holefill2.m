newimage=im2double(imread('mixrgb_with_real.jpg'));
 for rows=1:size(newimage,1)
        for cols=1:size(newimage,2)
            %if (x >= minVal) && (x <= maxVal)
            if (newimage(rows,cols,1)>=0.8) &&(newimage(rows,cols,2)>=0.8)&&(newimage(rows,cols,3)>=0.8)
                newimage(rows,cols,:)=NaN;
            end
        end
end
A=newimage;
F = fillmissing(A,'linear');
imtool(A)
iA = A;
for k = 1:3, iA(:,:,k) = inpaintn(A(:,:,k)); end
imtool(iA)