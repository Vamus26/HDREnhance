    newimage=imread('mixrgb_with_real.jpg');
    for rows=1:size(newimage,1)
        for cols=1:size(newimage,2)
            %if (x >= minVal) && (x <= maxVal)
            if (newimage(rows,cols,1)>=210) &&(newimage(rows,cols,2)>=210)&&(newimage(rows,cols,3)>=210)
                newimage(rows,cols,:)=NaN;
            end
        end
    end
    F = fillmissing(newimage,'nearest');
    imtool(F)
    
    R = newimage(:,:,1); % splitting RGB because regionfill need grayscale.
    G = newimage(:,:,2);
    B = newimage(:,:,3);
    maskedRgbImage = bsxfun(@times, newimage, cast(mask,class(newimage)));
    mask = uint8(R) > 252;
    mask = ~imfill(~mask,'holes');
    mask = imerode(mask,strel('disk',10));
    mask = imdilate(mask,strel('disk',20));
    R_autofill = regionfill(R,mask);
    mask = uint8(G) > 252;
    mask = ~imfill(~mask,'holes');
    mask = imerode(mask,strel('disk',10));
    mask = imdilate(mask,strel('disk',20));
    G_autofill = regionfill(G,mask);
    mask = uint8(B) > 252;
    mask = ~imfill(~mask,'holes');
    mask = imerode(mask,strel('disk',10));
    mask = imdilate(mask,strel('disk',20));
    B_autofill = regionfill(B,mask);
    newimage_autofill(:,:,1)=R_autofill;
    newimage_autofill(:,:,2)=G_autofill;
    newimage_autofill(:,:,3)=B_autofill;
    imtool(newimage_autofill)
    
    
    mask = uint8(R) < 2;
    mask = R>=210;
    mask = imfill(mask,'holes');
    mask = imerode(mask,strel('disk',10));
    mask = imdilate(mask,strel('disk',20));
    R_autofill = regionfill(R,mask);
    mask = uint8(G) < 2;
    mask = G>=210;
    mask = imfill(mask,'holes');
    mask = imerode(mask,strel('disk',10));
    mask = imdilate(mask,strel('disk',20));
    G_autofill = regionfill(G,mask);
    mask = uint8(B) < 2;
    mask = B>=210;
    mask = imfill(mask,'holes');
    mask = imerode(mask,strel('disk',10));
    mask = imdilate(mask,strel('disk',20));
    B_autofill = regionfill(B,mask);
    newimage_autofill(:,:,1)=R_autofill;
    newimage_autofill(:,:,2)=G_autofill;
    newimage_autofill(:,:,3)=B_autofill;
    imtool(newimage_autofill)
    
    
    
    A(A < 0.75 & A > 0.5) = NaN;
    F = fillmissing(A,'linear')