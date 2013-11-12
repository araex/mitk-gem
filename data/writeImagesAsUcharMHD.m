function []=writeImagesAsUcharMHD(outfname,im)
[rows, cols, slices] = size(im);

% we need relative path in header
s = strfind(outfname,'/');
if isempty(s)
    headeroutname = strcat(outfname(1:end-4),'.raw');
else
    headeroutname = strcat(outfname(s(end)+1:end-4),'.raw');
end

%Write header file
fid = fopen(outfname,'wt');
fprintf(fid,'ObjectType = Image\nNDims = 3\nBinaryData = True\nBinaryDataByteOrderMSB = False\nCompressedData = False\n');
fprintf(fid,'TransformMatrix = 1 0 0 0 1 0 0 0 1\n');
fprintf(fid,'Offset = 0 0 0\n');
fprintf(fid,'CenterOfRotation = 0 0 0\n');
fprintf(fid,'AnatomicalOrientation = RAI\n');
fprintf(fid,'ElementSpacing = 1.0 1.0 1.0\n'); % we'll transpose see below
fprintf(fid,'DimSize = %d %d %d\n',rows,cols,slices);
fprintf(fid,'ElementType = MET_UCHAR\n');
fprintf(fid,'ElementDataFile = %s\n',headeroutname);
fclose(fid);

%Write Image data

fid = fopen(strcat(outfname(1:end-4),'.raw'), 'wt');
count = fwrite(fid,im,'uint8');
fclose(fid);
end
