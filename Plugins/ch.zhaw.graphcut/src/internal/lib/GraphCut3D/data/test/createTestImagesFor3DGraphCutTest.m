% create sample images for automated tests
outputDir = 'cube10x10x10/';

% 10x10x10 image
testIm = zeros(10, 10, 10);

% A cube at the center
testIm(4:6,4:6,4:6) = 1;
writeImagesAsUcharMHD(strcat(outputDir, 'cube.mhd'), im2uint8(testIm));

% The noisy version 
testImNoise = imnoise(testIm, 'gaussian', 0, 0.01);
writeImagesAsUcharMHD(strcat(outputDir, 'cubeNoisy_0p01.mhd'), im2uint8(testImNoise));

% Forground mask
forgroundMask = zeros(10, 10, 10);
forgroundMask(5:6,5,5) = 1;
writeImagesAsUcharMHD(strcat(outputDir, 'foregroundMask.mhd'),forgroundMask);

% Background mask
backgroundMask = zeros(10, 10, 10);
backgroundMask(1:2,1:2,1) = 2;
backgroundMask(8:9,8:9,10) = 2;
writeImagesAsUcharMHD(strcat(outputDir, 'backgroundMask.mhd'), backgroundMask);

% expected result
expectedResult = testIm;
writeImagesAsUcharMHD(strcat(outputDir, 'expectedResult.mhd'), im2uint8(testIm));

% create sample images for automated tests
outputDir = '3x3x3/';
testIm = zeros(3,3,3);
testIm(1:3,1:2,1:2) = 1;
writeImagesAsUcharMHD(strcat(outputDir, 'input.mhd'), im2uint8(testIm));
writeImagesAsUcharMHD(strcat(outputDir, 'expectedResult.mhd'), im2uint8(testIm));

forgroundMask = zeros(3,3,3);
forgroundMask(1:1,1:1,1:1) = 1;
writeImagesAsUcharMHD(strcat(outputDir, 'foregroundMask.mhd'), im2uint8(forgroundMask));

backgroundMask = zeros(3,3,3);
backgroundMask(3:3,3:3,3:3) = 1;
writeImagesAsUcharMHD(strcat(outputDir, 'backgroundMask.mhd'), im2uint8(backgroundMask));
