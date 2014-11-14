/*
Author: Yves Pauchard, yves.pauchard@zhaw.ch
Date: Nov 5, 2013
Modified: Jan 21+22, 2014: Now exposes new option of the graphCut class.

Description: Adaptation of the 2D RGB version by David Doria to 3D single value images.


Based on ImageGraphCutSegmentationExample.cpp:
Copyright (C) 2012 David Doria, daviddoria@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ImageGraphCut3DFilter.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIteratorWithIndex.h"


/** Adapted version of GetPixelsWithValue from ITKHelpers */
template<typename TImage>
std::vector<itk::Index<3> > GetPixelsWithValueLargerThanZero(const TImage *const image) {

    std::vector<itk::Index<3> > pixelsWithValueLargerThanZero;

    itk::ImageRegionConstIterator<TImage> regionIterator(image, image->GetLargestPossibleRegion());
    while (!regionIterator.IsAtEnd()) {
        if (regionIterator.Get() > itk::NumericTraits<typename TImage::PixelType>::Zero) {
            pixelsWithValueLargerThanZero.push_back(regionIterator.GetIndex());
        }
        ++regionIterator;
    }

    return pixelsWithValueLargerThanZero;
}

/** This example segments an image and writes the segmentation mask to a file.
* It can operate on a 3D image with one component per dimension (i.e.
* grayscale).
*/
int main(int argc, char *argv[]) {
    // Verify arguments
    if (argc != 7) {
        std::cerr << "Required: image.mhd foregroundMask.mhd backgroundMask.mhd output.mhd sigma boundaryDirection" << std::endl;
        std::cerr << "image.mhd:           3D image in Hounsfield Units -1024 to 3071" << std::endl;
        std::cerr << "foregroundMask.mhd:  3D image non-zero pixels indicating foreground and 0 elsewhere" << std::endl;
        std::cerr << "backgroundMask.mhd:  3D image non-zero pixels indicating background and 0 elsewhere" << std::endl;
        std::cerr << "output.mhd:          3D image resulting segmentation" << std::endl;
        std::cerr << "                     Foreground as 127 and Background as 255" << std::endl;
        std::cerr << "sigma                estimated noise in boundary term, try 50.0" << std::endl;
        std::cerr << "boundaryDirection    0->bidirectional; 1->bright to dark; 2->dark to bright" << std::endl;

        return EXIT_FAILURE;
    }

    // Parse arguments
    std::string imageFilename = argv[1];
    std::string foregroundFilename = argv[2];   // This image should have non-zero pixels indicating foreground pixels and 0 elsewhere.
    std::string backgroundFilename = argv[3];   // This image should have non-zero pixels indicating background pixels and 0 elsewhere.
    std::string outputFilename = argv[4];
    double sigma = atof(argv[5]);                //Noise parameter
    int boundaryDirection = atoi(argv[6]);      //0->bidirectional; 1->bright to dark; 2->dark to bright

    // Output arguments
    std::cout << "imageFilename: " << imageFilename << std::endl
            << "foregroundFilename: " << foregroundFilename << std::endl
            << "backgroundFilename: " << backgroundFilename << std::endl
            << "outputFilename: " << outputFilename << std::endl
            << "sigma: " << sigma << std::endl
            << "boundaryDirection: " << boundaryDirection << std::endl;

    // define all the image types
    typedef itk::Image<short, 3> ImageType;
    typedef itk::Image<unsigned char, 3> ForegroundMaskType;
    typedef itk::Image<unsigned char, 3> BackgroundMaskType;
    typedef itk::Image<unsigned char, 3> OutputImageType;

    // Read the image
    std::cout << "*** Reading image ***" << std::endl;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(imageFilename);

    // Read the foreground and background masks
    std::cout << "*** Reading foreground mask ***" << std::endl;
    typedef itk::ImageFileReader<ForegroundMaskType> ForegroundMaskReaderType;
    ForegroundMaskReaderType::Pointer foregroundMaskReader = ForegroundMaskReaderType::New();
    foregroundMaskReader->SetFileName(foregroundFilename);

    std::cout << "*** Reading background mask ***" << std::endl;
    typedef itk::ImageFileReader<BackgroundMaskType> BackgroundMaskReaderType;
    BackgroundMaskReaderType::Pointer backgroundMaskReader = BackgroundMaskReaderType::New();
    backgroundMaskReader->SetFileName(backgroundFilename);

    // Set up the graph cut
    std::cout << "*** Performing Graph Cut ***" << std::endl;
    typedef itk::ImageGraphCut3DFilter<ImageType, ForegroundMaskType, BackgroundMaskType, OutputImageType> GraphCutFilterType;
    GraphCutFilterType::Pointer graphCutFilter = GraphCutFilterType::New();
    graphCutFilter->SetInputImage(reader->GetOutput());
    graphCutFilter->SetForegroundImage(foregroundMaskReader->GetOutput());
    graphCutFilter->SetBackgroundImage(backgroundMaskReader->GetOutput());

    // set graph cut parameters
    graphCutFilter->SetSigma(sigma);
    switch (boundaryDirection) {
        case 1:
            graphCutFilter->SetBoundaryDirectionTypeToBrightDark();
            break;
        case 2:
            graphCutFilter->SetBoundaryDirectionTypeToDarkBright();
            break;
        default:
            graphCutFilter->SetBoundaryDirectionTypeToNoDirection();
    }

    // define the color values of the output
    graphCutFilter->SetForegroundPixelValue(255);
    graphCutFilter->SetBackgroundPixelValue(0);
    graphCutFilter->Update();

    // Get and write the result
    std::cout << "*** Writing Result ***" << std::endl;
    typedef itk::ImageFileWriter<GraphCutFilterType::OutputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(graphCutFilter->GetOutput());
    try {
        std::cout << "Writing output image " << outputFilename << std::endl;
        writer->Update();
    }
    catch (itk::ExceptionObject &err) {
        std::cerr << "ERROR: Exception caught while writing output image" << std::endl;
        std::cerr << err << std::endl;
    }
}
