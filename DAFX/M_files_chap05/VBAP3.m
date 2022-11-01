function [gains] = VBAP3(pan_dir)
% function [gains] = VBAP3(pan_dir)
% Author: V. Pulkki
% Computes 3D VBAP gains for loudspeaker setup shown in Fig.6.4 
% Change the lousdpeaker directions to match with your system, 
% the directions are defined as azimuth elevation; pairs
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

loudspeakers=[0 0; 50 0; 130 0; -130 0; -50 0;  40 45; 180 45;-40 45];
ls_num=size(loudspeakers,1);
% Select the triangles of from the loudspeakers here
ls_triangles=[1 2 6; 2 3 6; 3 4 7; 4 5 8; 5 1 8; 1 6 8;
    3 6 7; 4 7 8; 6 7 8];
% Go through all triangles
for tripl=1:size(ls_triangles,1)
    ls_tripl=loudspeakers(ls_triangles(tripl,:),:);
    % Panning direction in cartesian coordinates
    cosE=cos(pan_dir(2)/180*pi);
    panvec(1:2)=[cos(pan_dir(1)/180*pi)*cosE sin(pan_dir(1)/180*pi)*cosE];
    panvec(3)=sin(pan_dir(2)/180*pi);
    % Loudspeaker base matrix for current triangle.
    for i=1:3
        cosE=cos(ls_tripl(i,2)/180*pi);
        lsmat(i,1:2)=[cos(ls_tripl(i,1)/180*pi)*cosE...
            sin(ls_tripl(i,1)/180*pi)*cosE];
        lsmat(i,3)=sin(ls_tripl(i,2)/180*pi);
    end
    tempg=panvec*inv(lsmat); % Gain factors for current triangle.
    % If gains nonnegative, normalize g and stop computation
    if min(tempg) > -0.01
        tempg=tempg/sqrt(sum(tempg.^2));
        gains=zeros(1,ls_num);
        gains(1,ls_triangles(tripl,:))=tempg;
        return
    end
end