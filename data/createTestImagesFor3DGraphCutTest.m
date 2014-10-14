%
Create sample
images to
test 3D
graph cut
segmentation
%
%
Author: Yves
Pauchard
%
Date: Nov
6, 2013

%A 3D
image
        testIm = zeros(10, 10, 10);
%
A cube
at the
center
        testIm(4:
6,4:6,4:6) = 1;
writeImagesAsUcharMHD('testCube10x10x10.mhd',
im2uint8(testIm)
);
%
The noisy
version
        testImNoise = imnoise(testIm, 'gaussian', 0, 0.01);
writeImagesAsUcharMHD('testCube10x10x10Noisy_0p01.mhd',
im2uint8(testImNoise)
);

%
Forground mask
forgroundMask = zeros(10, 10, 10);
forgroundMask(5:6,5,5) = 1;
writeImagesAsUcharMHD('testForegroundMask.mhd',forgroundMask);
%
Background mask(label
2)
backgroundMask = zeros(10, 10, 10);
backgroundMask(1:2,1:2,1) = 2;
backgroundMask(8:9,8:9,10) = 2;
writeImagesAsUcharMHD('testBackgroundMask.mhd',backgroundMask);
