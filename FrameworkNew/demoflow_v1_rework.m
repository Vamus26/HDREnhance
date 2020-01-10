clear all;addpath('mex');%Frames must be named correctly. Starting : 1.jpg for LDR frames and 1.exr%for HDR frames. If there is an additional filename before, add the name in the preFilename string.%If you want to use .hdr files you have to change hdrExtension to ".hdr."%Generated files are currently called CreatedHDR_X.hdr and the tonemapped%version is called CreatedHDRTonemapped_X.jpghdrExtension = ".hdr"; %fitting file ending needed, like .hdrldrExtension = ".jpg";hdrFilename = "tonetrycreatedHDRUMU";%"CreatedHDR_panov2_cube";ldrFilename = "tonetrycreatedLDRUMU";%"CreatedHDRTonemapped_panov2_cube";preFilename = "panov2_";mode = 1; %1 if equicubic mode, else full picture at once% load the framesfor cnt=1:100    tmp = sprintf('%s%d%s',preFilename,cnt,hdrExtension);    tmpL = sprintf('%s%d%s',preFilename,cnt,ldrExtension);    if ~isfile(tmpL)        amount = cnt-1;        break;    end    LdrA.(sprintf('L_%d', cnt))=hdrimread(tmpL);    if isfile(tmp)     % File exists.        lastHdrEqualLdr = cnt;        HdrA.(sprintf('H_%d', cnt))=hdrimread(tmp);    else        HdrA.(sprintf('H_%d', cnt))=0;        lastHdr = cnt-1;    endend% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)alpha = 0.012;ratio = 0.75;minWidth = 20;nOuterFPIterations = 7;nInnerFPIterations = 1;nSORIterations = 30;para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];%LdrStack=zeros(1,amount);for count=1:amount        tmpC = sprintf('HdrA.H_%d',count);    tmpD = sprintf('LdrA.L_%d',count);    imag1 = eval(tmpC);%access struct element    LdrStack{count} = eval(tmpD);    if imag1~=0        if ~exist('firstHdr','var')            firstHdr = imag1;            firstHdrIdx = count;        elseif ~exist('secondHdr','var')            secondHdr = imag1;            secondHdrIdx = count;            doFlowCalculation(firstHdr,secondHdr,LdrStack, firstHdrIdx, secondHdrIdx,para, hdrFilename, ldrFilename,mode);            firstHdr =secondHdr;            firstHdrIdx = secondHdrIdx;            clear secondHdr;            %  clear LdrStack;        end    endendfunction [cost_mat] = smoothCostGrad(s1r,s1g,s1b,s2r,s2g,s2b,t1r,t1g,t1b,t2r,t2g,t2b)gradTextR  = abs( s1r - t1r );gradTextV  = abs( s1g - t1g );gradTextB  = abs( s1b - t1b );gradPatchR  = abs( s2r - t2r );gradPatchV  = abs( s2g - t2g );gradPatchB  = abs( s2b - t2b );grad = ((gradTextR + gradTextV + gradTextB) / 3.0 )+ (( gradPatchR + gradPatchV + gradPatchB ) / 3.0);grad= grad +1.0; % to avoid zero division%smoothCostBasiccr = abs( s1r - s2r ) +  abs( t1r - t2r );cg = abs( s1g - s2g ) +  abs( t1g - t2g );cb = abs( s1b - s2b ) +  abs( t1b - t2b );tmp =( ( cr + cg + cb ) / 3.0 );cost_mat = (tmp/ sqrt(grad));endfunction [] =doFlowCalculation(firstHdr,secondHdr,LdrStack, firstHdrIdx, secondHdrIdx,para, hdrFilename, ldrFilename,mode)maximumLDR =size(LdrStack,2);%amount LDR frames%maximumLDR =size(LdrStack,1);%amount LDR framesfor idx=firstHdrIdx:maximumLDR-2%max already HDR so-1    if mode ~= 1        [vx,vy,~] = Coarse2FineTwoFrames(LdrStack{idx},LdrStack{idx+1},para);%fwd        %[vx,vy,~] = Coarse2FineTwoFrames(LdrStack(idx,:,:,:),LdrStack(idx+1,:,:,:),para);%fwd        vx_2 = vx;        vy_2 = vy;        clear vx;        clear vy;        bwdidx = 1;%idx;        %while (bwdidx+1)~=(maximumLDR-bwdidx)        while ((bwdidx)<(maximumLDR-bwdidx))&&((maximumLDR-bwdidx)>(firstHdrIdx))            [vxb,vyb,~] = Coarse2FineTwoFrames(LdrStack{maximumLDR+1-bwdidx},LdrStack{maximumLDR-bwdidx},para);%bwd            % [vxb,vyb,~] = Coarse2FineTwoFrames(LdrStack(maximumLDR+1-bwdidx,:,:,:),LdrStack(maximumLDR-bwdidx,:,:,:),para);%bwd            vxb_2 = vxb;            vyb_2 = vyb;            clear vxb;            clear vyb;            bwdidx = bwdidx+1;        end                [FWDImage, BWDImage, moco_bwd, moco_fwd] =createHDR(firstHdr, secondHdr ,vx_2,vy_2,vxb_2,vyb_2);        doGraphCut(FWDImage, BWDImage, moco_bwd, moco_fwd, firstHdrIdx, secondHdrIdx, LdrStack{idx+1}, idx+1, hdrFilename, ldrFilename, true);    else%panorama part        out_1 = own_equi2cubic(LdrStack{idx}, 1920); % Set to 1920 x 1920 %front,right,back,left,top,bottom        out_2 = own_equi2cubic(LdrStack{idx+1}, 1920); % Set to 1920 x 1920        out_hdr1 = own_equi2cubic(firstHdr, 1920); % Set to 1920 x 1920        out_hdr2 =own_equi2cubic(secondHdr, 1920); % Set to 1920 x 1920                for  pindex=1:numel(out_1)            [vx,vy,~] = Coarse2FineTwoFrames(out_1{pindex},out_2{pindex},para);%fwd            vx_2 = vx;            vy_2 = vy;            clear vx;            clear vy;            bwdidx = 1;%idx;            while ((bwdidx)<(maximumLDR-bwdidx))&&((maximumLDR-bwdidx)>(firstHdrIdx))                out_tmp1 = own_equi2cubic(LdrStack{maximumLDR+1-bwdidx}, 1920);                out_tmp2 = own_equi2cubic(LdrStack{maximumLDR-bwdidx}, 1920);                [vxb,vyb,~] = Coarse2FineTwoFrames(out_tmp1{pindex},out_tmp2{pindex},para);%bwd                vxb_2 = vxb;                vyb_2 = vyb;                clear vxb;                clear vyb;                bwdidx = bwdidx+1;            end                        [FWDImage, BWDImage, moco_bwd, moco_fwd] =createHDR(out_hdr1{pindex}, out_hdr2{pindex} ,vx_2,vy_2,vxb_2,vyb_2);            [hdrEqui{pindex}] = doGraphCut(FWDImage, BWDImage, moco_bwd, moco_fwd, firstHdrIdx, secondHdrIdx, out_2{pindex}, idx+1, hdrFilename, ldrFilename, false);        end        out_hdr = own_cubic2equi(hdrEqui{5}, hdrEqui{6}, hdrEqui{4}, hdrEqui{2}, hdrEqui{1}, hdrEqui{3});%top, bottom, left, right, front, back        out_ldr = tonemap(out_hdr);        hdrfname = sprintf ( '%s%d.hdr', hdrFilename, idx+1 );        ldrfname = sprintf ( '%s%d.jpg', ldrFilename, idx+1 );        imwrite(out_ldr, ldrfname);        hdrimwrite(out_hdr, hdrfname);    endendendfunction [FWDImage, BWDImage, moco_fwd, moco_bwd] =createHDR(leftHdr, rightHdr,vx_1,vy_1,vxb_1,vyb_1)[x, y] = meshgrid(1:size(leftHdr,2), 1:size(leftHdr,1));redChannel = leftHdr(:, :, 1);DR = interp2(redChannel, x-vx_1, y-vy_1);greenChannel = leftHdr(:, :, 2);DG = interp2(greenChannel, x-vx_1, y-vy_1);blueChannel = leftHdr(:, :, 3);DB = interp2(blueChannel, x-vx_1, y-vy_1);FWDImage = cat(3, DR, DG, DB);%hdrimwrite(FWDImage, 'resultExtraFWD10.hdr');[moco_fwd] = calculateMoCo(FWDImage, vx_1,vy_1);[x, y] = meshgrid(1:size(rightHdr,2), 1:size(rightHdr,1));redChannel = rightHdr(:, :, 1);DR = interp2(redChannel, x-vxb_1, y-vyb_1);greenChannel = rightHdr(:, :, 2);DG = interp2(greenChannel, x-vxb_1, y-vyb_1);blueChannel = rightHdr(:, :, 3);DB = interp2(blueChannel, x-vxb_1, y-vyb_1);BWDImage = cat(3, DR, DG, DB);%hdrimwrite(BWDImage, 'resultExtraBWD10.hdr');[moco_bwd] = calculateMoCo(BWDImage, vxb_1,vyb_1);endfunction [moco] =calculateMoCo(HdrImage, vx,vy)clear flow;flow(:,:,1) = vx;flow(:,:,2) = vy;meanImage = imfilter(flow, ones(3)/9);%meansz = size(HdrImage);tmpMoco = zeros(sz(1:2),'double');for rows=1:size(meanImage,1)    for cols=1:size(meanImage,2)        first_angle= atan(HdrImage(rows,cols,1)/HdrImage(rows,cols,2));        sec_angle= atan(meanImage(rows,cols,1)/meanImage(rows,cols,2));        angle = first_angle-sec_angle;        if (angle > 180)            angle = angle-360;        elseif (angle <-180)            angle = angle+360;        end        tmpMoco(rows,cols)= angle;%bwd    endendmoco = tmpMoco;endfunction [mix] =doGraphCut(FWDImage, BWDImage, moco_bwd, moco_fwd, firstHdrIdx, secondHdrIdx, LdrFrame, ldrIdx, hdrFilename, ldrFilename, createFiles)wlImage = FWDImage;%forw imagefi = LdrFrame;%simulated LDR framewrImage = BWDImage;%backw imagewlImageTone = tonemap(wlImage);%forw image tonemappedwrImageTone = tonemap(wrImage);%backw image tonemappedmotion_conf_bwd = moco_bwd;motion_conf_fwd = moco_fwd;frame_index_wl = firstHdrIdx;frame_index_fi = ldrIdx;frame_index_wr = secondHdrIdx;k = 3;%# different labelssz = size(fi);Dc_new = zeros([sz(1:2) k],'single');%Datacost functionfor rows=1:size(fi,1)    for cols=1:size(fi,2)        eukl = double(wlImageTone(rows,cols) - fi(rows,cols));        Dc_2 = norm(eukl);        Df = motion_conf_fwd(rows,cols);        Dd = abs(frame_index_wl-frame_index_fi) / abs(frame_index_wr-frame_index_wl);        Dc_new(rows,cols,1)=Dc_2+Df+Dd;        Df = motion_conf_bwd(rows,cols);        eukl = double(wrImageTone(rows,cols) - fi(rows,cols));        Dc_2 = norm(eukl);        Dd = abs(frame_index_wr-frame_index_fi) / abs(frame_index_wr-frame_index_wl);        Dc_new(rows,cols,3)=Dc_2+Df+Dd;        Dc_new(rows,cols,2) = 500;%0.3 originally    endend%smoothness functionSc_new = [1,1,1;1,1,1;1,1,1];Hc_new = zeros(sz(1:2),'single');%horizontal costVc_new = zeros(sz(1:2),'single');%vertical costfor rows=1:size(fi,1)    for cols=1:size(fi,2)        if (cols-1)<=0            Hc_new(rows,cols)= 99999;%inf;        else            Hc_new(rows,cols)= smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows,cols-1,1), wlImage(rows,cols-1,2), wlImage(rows,cols-1,3),wrImage(rows,cols-1,1),wrImage(rows,cols-1,2),wrImage(rows,cols-1,3));        end        if (rows-1)<=0            Vc_new(rows,cols)= 99999;%inf;        else            Vc_new(rows,cols)= smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows-1,cols,1), wlImage(rows-1,cols,2), wlImage(rows-1,cols,3),wrImage(rows-1,cols,1),wrImage(rows-1,cols,2),wrImage(rows-1,cols,3));        end        %TODO: maybe use at edges        %        if (cols+1)>size(fi,2)        %        	Hc_new(rows,cols)= Hc_new(rows,cols)+ inf;        %        else        %            Hc_new(rows,cols)= Hc_new(rows,cols)+smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows,cols+1,1), wlImage(rows,cols+1,2), wlImage(rows,cols+1,3),wrImage(rows,cols+1,1),wrImage(rows,cols+1,2),wrImage(rows,cols+1,3));        %        end        %        if (rows+1)>size(fi,1)        %        	Vc_new(rows,cols)= Vc_new(rows,cols)+inf;        %        else        %        	Vc_new(rows,cols)= Vc_new(rows,cols)+smoothCostGrad( wlImage(rows,cols,1), wlImage(rows,cols,2), wlImage(rows,cols,3),wrImage(rows,cols,1),wrImage(rows,cols,2),wrImage(rows,cols,3),wlImage(rows+1,cols,1), wlImage(rows+1,cols,2), wlImage(rows+1,cols,3),wrImage(rows+1,cols,1),wrImage(rows+1,cols,2),wrImage(rows+1,cols,3));        %        end    endend%graphcutgch = GraphCut('open', Dc_new, Sc_new, Vc_new, Hc_new);[gch L] = GraphCut('expand',gch); %labels from 0 to (labels-1)- so 0,1,2 here[gch se de] = GraphCut('energy', gch)[gch e] = GraphCut('energy', gch)[gch L] = GraphCut('swap', gch);gch = GraphCut('close', gch);mix = zeros([sz(1:2) k],'double');for rows=1:size(Dc_new,1)    for cols=1:size(Dc_new,2)        if (Dc_new(rows,cols,1)<Dc_new(rows,cols,2))&&(Dc_new(rows,cols,1)<Dc_new(rows,cols,3))            mix(rows,cols,:)= wlImage(rows,cols,:);        elseif (Dc_new(rows,cols,3)<Dc_new(rows,cols,2))&&(Dc_new(rows,cols,3)<Dc_new(rows,cols,1))            mix(rows,cols,:)= wrImage(rows,cols,:);        else            mix(rows,cols,:)= fi(rows,cols,:);        end    endendrgb = tonemap(mix);%if createFiles == true    hdrfname = sprintf ( '%s%d.hdr', hdrFilename, ldrIdx );    ldrfname = sprintf ( '%s%d.jpg', ldrFilename, ldrIdx );    imwrite(rgb, ldrfname);    hdrimwrite(mix, hdrfname);%endendfunction [out] = own_equi2cubic(im, output_size, vfov)% Rotation matrix - x axis% Angle in degrees    function [mat] = rotx(ang)        mat = [1 0 0;            0 cosd(ang) -sind(ang);            0 sind(ang) cosd(ang)];    end% Rotation matrix - y axis% Angle in degrees    function [mat] = roty(ang)        mat = [cosd(ang) 0 sind(ang);            0 1 0;            -sind(ang) 0 cosd(ang)];    end% Rotation matrix - z axis% Angle in degrees    function [mat] = rotz(ang)        mat = [cosd(ang) -sind(ang) 0;            sind(ang) cosd(ang) 0;            0 0 1];    end% Set up default parameters% Default output sizeif nargin == 1    n = size(im, 2);    output_size = 8*(floor(n/pi/8));end% Default vertical FOVif nargin <= 2, vfov = 90; end% Error if invalid # of input argumentsif nargin == 0 || nargin > 3, error('Insufficient arguments'); endvfov = vfov*pi/180; % Convert to radians% Output imagesout = cell(1,6);% Define yaw, pitch and roll for viewing of each cube face% In degreesviews = [0 0 0; % Front    90 0 0; % Right    180 0 0; % Back    -90 0 0; % Left    0 90 0; % Top    0 -90 0]; % Bottom% Define top left point of equirectangular in normalized co-ordinatesoutput_width = output_size;output_height = output_size;input_width = size(im,2);input_height = size(im,1);topLeft = [-tan(vfov/2)*(output_width/output_height), -tan(vfov/2), 1];% Scaling factor for grabbing pixel co-ordinatesuv = [-2*topLeft(1)/output_width, -2*topLeft(2)/output_height, 0];% Equirectangular lookupsres_acos = 2*input_width;res_atan = 2*input_height;step_acos = pi / res_acos;step_atan = pi / res_atan;lookup_acos = [-cos((0:res_acos-1)*step_acos) 1];lookup_atan = [tan(step_atan/2 - pi/2) tan((1:res_atan-1)*step_atan ...    - pi/2) tan(-step_atan/2 + pi/2)];% Output face co-ordinates[X, Y] = meshgrid(0:output_width-1, 0:output_height-1);X = X(:);Y = Y(:);% Input image co-ordinates[XImage, YImage] = meshgrid(1:input_width, 1:input_height);for idx = 1 : size(views,1)    % Get yaw, pitch and roll of a particular view    yaw = views(idx,1);    pitch = views(idx,2);    roll = views(idx,3);        % Get transformation matrix    transform = roty(yaw)*rotx(pitch)*rotz(roll);        % Rotate grid co-ordinates for cube face so that we are    % writing to the right face    points = [topLeft(1) + uv(1)*X.'; topLeft(2) + uv(2)*Y.'; ...        topLeft(3) + uv(3)*ones(1, numel(X))];    moved_points = transform * points;    x_points = moved_points(1,:);    y_points = moved_points(2,:);    z_points = moved_points(3,:);        % Determine correct equirectangular co-ordinates    % for each pixel within the cube face to sample from    nxz = sqrt(x_points.^2 + z_points.^2);    phi = zeros(1, numel(X));    theta = zeros(1, numel(X));        ind = nxz < realmin;    phi(ind & y_points > 0) = pi/2;    phi(ind & y_points <= 0) = -pi/2;        ind = ~ind;    phi(ind) = interp1(lookup_atan, 0:res_atan, ...        y_points(ind)./nxz(ind), 'linear')*step_atan - (pi/2);    theta(ind) = interp1(lookup_acos, 0:res_acos, ...        -z_points(ind)./nxz(ind), 'linear')*step_acos;    theta(ind & x_points < 0) = -theta(ind & x_points < 0);        % Find equivalent pixel co-ordinates    inX = (theta / pi) * (input_width/2) + (input_width/2) + 1;    inY = (phi / (pi/2)) * (input_height/2) + (input_height/2) + 1;        % Cap if out of bounds    inX(inX < 1) = 1;    inX(inX >= input_width) = input_width;    inY(inY < 1) = 1;    inY(inY >= input_height) = input_height;        % Initialize output image    out{idx} = zeros([output_height, output_width, size(im,3)], class(im));        % Store output colours here    out_pix = zeros(numel(X), size(im, 3));        % For each pixel position calculated previously,    % get the corresponding colour values    for c = 1 : size(im, 3)        chan = double(im(:,:,c));        out_pix(:,c) = interp2(XImage, YImage, chan, inX, inY, 'linear');    end        % Each column of out_pix is the output of a single channel    % Reshape so that it becomes a matrix instead and place into output    for c = 1 : size(im, 3)        out{idx}(:,:,c) = cast(reshape(out_pix(:,c), output_height, ...            output_width), class(im));    endendendfunction [out] = own_cubic2equi(top, bottom, left, right, front, back)% Height and width of equirectangular imageheight = size(top, 1);width = 2*height;% Flags to denote what side of the cube we are facing% Z-axis is coming out towards you% X-axis is going out to the right% Y-axis is going upwards% Assuming that the front of the cube is towards the% negative X-axisFACE_Z_POS = 1; % LeftFACE_Z_NEG = 2; % RightFACE_Y_POS = 3; % TopFACE_Y_NEG = 4; % BottomFACE_X_NEG = 5; % FrontFACE_X_POS = 6; % Back% Place in a cell arraystackedImages{FACE_Z_POS} = left;stackedImages{FACE_Z_NEG} = right;stackedImages{FACE_Y_POS} = top;stackedImages{FACE_Y_NEG} = bottom;stackedImages{FACE_X_NEG} = front;stackedImages{FACE_X_POS} = back;% Place in 3 3D matrices - Each matrix corresponds to a colour channelimagesRed = double(zeros(height, height, 6));imagesGreen = double(zeros(height, height, 6));imagesBlue = double(zeros(height, height, 6));% Place each channel into their corresponding matricesfor i = 1 : 6    im = stackedImages{i};    imagesRed(:,:,i) = im(:,:,1);    imagesGreen(:,:,i) = im(:,:,2);    imagesBlue(:,:,i) = im(:,:,3);end% For each co-ordinate in the normalized image...[X, Y] = meshgrid(1:width, 1:height);% Obtain the spherical co-ordinatesY = 2*Y/height - 1;X = 2*X/width - 1;sphereTheta = X*pi;spherePhi = (pi/2)*Y;texX = cos(spherePhi).*cos(sphereTheta);texY = sin(spherePhi);texZ = cos(spherePhi).*sin(sphereTheta);% Figure out which face we are facing for each co-ordinate% First figure out the greatest absolute magnitude for each pointcomp = cat(3, texX, texY, texZ);[~,ind] = max(abs(comp), [], 3);maxVal = zeros(size(ind));% Copy those values - signs and allmaxVal(ind == 1) = texX(ind == 1);maxVal(ind == 2) = texY(ind == 2);maxVal(ind == 3) = texZ(ind == 3);% Set each location in our equirectangular image, figure out which% side we are facinggetFace = -1*ones(size(maxVal));% Backind = abs(maxVal - texX) < 0.00001 & texX < 0;getFace(ind) = FACE_X_POS;% Frontind = abs(maxVal - texX) < 0.00001 & texX >= 0;getFace(ind) = FACE_X_NEG;% Topind = abs(maxVal - texY) < 0.00001 & texY < 0;getFace(ind) = FACE_Y_POS;% Bottomind = abs(maxVal - texY) < 0.00001 & texY >= 0;getFace(ind) = FACE_Y_NEG;% Leftind = abs(maxVal - texZ) < 0.00001 & texZ < 0;getFace(ind) = FACE_Z_POS;% Rightind = abs(maxVal - texZ) < 0.00001 & texZ >= 0;getFace(ind) = FACE_Z_NEG;% Determine the co-ordinates along which image to sample% based on which side we are facingrawX = -1*ones(size(maxVal));rawY = rawX;rawZ = rawX;% Backind = getFace == FACE_X_POS;rawX(ind) = -texZ(ind);rawY(ind) = texY(ind);rawZ(ind) = texX(ind);% Frontind = getFace == FACE_X_NEG;rawX(ind) = texZ(ind);rawY(ind) = texY(ind);rawZ(ind) = texX(ind);% Topind = getFace == FACE_Y_POS;rawX(ind) = texZ(ind);rawY(ind) = texX(ind);rawZ(ind) = texY(ind);% Bottomind = getFace == FACE_Y_NEG;rawX(ind) = texZ(ind);rawY(ind) = -texX(ind);rawZ(ind) = texY(ind);% Leftind = getFace == FACE_Z_POS;rawX(ind) = texX(ind);rawY(ind) = texY(ind);rawZ(ind) = texZ(ind);% Rightind = getFace == FACE_Z_NEG;rawX(ind) = -texX(ind);rawY(ind) = texY(ind);rawZ(ind) = texZ(ind);% Concatenate all for laterrawCoords = cat(3, rawX, rawY, rawZ);% Finally determine co-ordinates (normalized)cubeCoordsX = ((rawCoords(:,:,1) ./ abs(rawCoords(:,:,3))) + 1) / 2;cubeCoordsY = ((rawCoords(:,:,2) ./ abs(rawCoords(:,:,3))) + 1) / 2;cubeCoords = cat(3, cubeCoordsX, cubeCoordsY);% Now obtain where we need to sample the imagenormalizedX = round(cubeCoords(:,:,1) * height);normalizedY = round(cubeCoords(:,:,2) * height);% Just in case.... cap between [1, height] to ensure% no out of bounds behaviournormalizedX(normalizedX < 1) = 1;normalizedX(normalizedX > height) = height;normalizedY(normalizedY < 1) = 1;normalizedY(normalizedY > height) = height;% Place into a stacked matrixnormalizedCoords = cat(3, normalizedX, normalizedY);% Output image allocationout = double(zeros([size(maxVal) 3]));% Obtain column-major indices on where to sample from the% input images% getFace will contain which image we need to sample from% based on the co-ordinates within the equirectangular imageind = sub2ind([height height 6], normalizedCoords(:,:,2), ...    normalizedCoords(:,:,1), getFace);% Do this for each channelout(:,:,1) = imagesRed(ind);out(:,:,2) = imagesGreen(ind);out(:,:,3) = imagesBlue(ind);end