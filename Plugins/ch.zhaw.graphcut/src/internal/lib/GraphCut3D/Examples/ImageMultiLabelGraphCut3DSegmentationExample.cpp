/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "MultiLabelGraphCut.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

/** This example segments an image and writes the segmentation mask to a file.
* It can operate on a 3D image with one component per dimension (i.e.
* grayscale).
*/
int main(int argc, char *argv[]) {
    // Verify arguments
    if (argc != 6) {
        std::cerr << "Required: image.mhd multiLabelMask.mhd backgroundMask.mhd output.mhd sigma boundaryDirection" << std::endl;
        std::cerr << "image.mhd:           3D image in Hounsfield Units -1024 to 3071" << std::endl;
        std::cerr << "multiLabelMask.mhd:  3D image non-zero pixels indicating foreground and 0 elsewhere" << std::endl;
        std::cerr << "output.mhd:          3D image resulting segmentation" << std::endl;
        std::cerr << "                     Foreground as 127 and Background as 255" << std::endl;
        std::cerr << "sigma                estimated noise in boundary term, try 50.0" << std::endl;
        std::cerr << "boundaryDirection    0->bidirectional; 1->bright to dark; 2->dark to bright" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse arguments
    std::string imageFilename = argv[1];
    std::string multiLabelImageFilename = argv[2];   // This image should have non-zero pixels indicating foreground pixels and 0 elsewhere.
    std::string outputFilename = argv[3];
    double sigma = atof(argv[4]);               // Noise parameter
    int boundaryDirection = atoi(argv[5]);      // 0->bidirectional; 1->bright to dark; 2->dark to bright

    // Print arguments
    std::cout << "imageFilename: " << imageFilename << std::endl
            << "multiLabelImageFilename: " << multiLabelImageFilename << std::endl
            << "outputFilename: " << outputFilename << std::endl
            << "sigma: " << sigma << std::endl
            << "boundaryDirection: " << boundaryDirection << std::endl;

    // Define all image types
    typedef itk::Image<short, 3> ImageType;
    typedef itk::Image<short, 3> MultiLabelMaskType;
    typedef itk::Image<short, 3> OutputImageType;

    // Read the image
    std::cout << "*** Reading image ***" << std::endl;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(imageFilename);

    // Read the foreground and background masks
    std::cout << "*** Reading multi-label mask ***" << std::endl;
    typedef itk::ImageFileReader<MultiLabelMaskType> MultiLabelMaskReaderType;
    MultiLabelMaskReaderType::Pointer multiLabelMaskReader = MultiLabelMaskReaderType::New();
    multiLabelMaskReader->SetFileName(multiLabelImageFilename);


    // Set up the graph cut
    typedef GraphCut::FilterType<ImageType, MultiLabelMaskType, OutputImageType> GraphCutFilterType;
    GraphCutFilterType::Pointer graphCutFilter;
    graphCutFilter = GraphCutFilterType::New();
    graphCutFilter->SetInputImage(reader->GetOutput());
    graphCutFilter->SetMultiLabelImage(multiLabelMaskReader->GetOutput());

    // Set graph cut parameters
    graphCutFilter->SetVerboseOutput(true);
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


    // Start the computation
    std::cout << "*** Performing Graph Cut ***" << std::endl;
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
