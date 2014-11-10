#include <gtest/gtest.h>

// ITK
#include <itkImage.h>
#include <itkSubtractImageFilter.h>
#include <itkStatisticsImageFilter.h>

#include "IOHelper.hxx"
#include "ImageGraphCut3DFilter.h"

class TestSegmentation : public ::testing::Test {
protected:

    // image types
    typedef itk::Image<short, 3> TInput;
    typedef itk::Image<unsigned int, 3> TMask;
    typedef TMask TForeground;
    typedef TMask TBackground;
    typedef TMask TOutput;
    typedef itk::Image<int, 3> TIntImage;

    // graphcut
    typedef itk::ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput> GraphCutFilterType;

    // image compare
    typedef itk::SubtractImageFilter<GraphCutFilterType::OutputImageType, TOutput, TIntImage> TDifferenceFilter;
    typedef itk::StatisticsImageFilter<TDifferenceFilter::OutputImageType> TStatisticsFilter;

    virtual void SetUp() {
        // create all filters
        graphCutFilter = GraphCutFilterType::New();
        substractFilter = TDifferenceFilter::New();
        statisticsFilter = TStatisticsFilter::New();
    }

    // virtual void TearDown() {}

    // graphcut
    GraphCutFilterType::Pointer graphCutFilter;

    // image compare filter
    TDifferenceFilter::Pointer substractFilter;
    TStatisticsFilter::Pointer statisticsFilter;
};

TEST_F(TestSegmentation, MiniTest){
    // path to files
    std::string inputPath = "data/test/3x3x3/input.mhd";
    std::string forgroundPath = "data/test/3x3x3/foregroundMask.mhd";
    std::string backgroundPath = "data/test/3x3x3/backgroundMask.mhd";
    std::string expectedPath = "data/test/3x3x3/expectedResult.mhd";
    std::string outputPath = "data/test/3x3x3/output.mhd";

    // read the images
    TInput::Pointer inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
    TForeground::Pointer foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
    TBackground::Pointer backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
    TOutput::Pointer expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

    // set images
    graphCutFilter->SetInputImage(inputImage);
    graphCutFilter->SetForegroundImage(foregroundMask);
    graphCutFilter->SetBackgroundImage(backgroundMask);

    // set parameters
    graphCutFilter->SetForegroundPixelValue(255);
    graphCutFilter->SetBackgroundPixelValue(0);
    graphCutFilter->SetSigma(50.0);
    graphCutFilter->SetLambda(1.0);
    graphCutFilter->SetBoundaryDirectionTypeToBrightDark();

    // compare the results: I_Result(x)-I_Expected(x)==0
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    IOHelper::writeImage<TOutput>(graphCutFilter->GetOutput(), outputPath.c_str());

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(0, pixelSum);
}

TEST_F(TestSegmentation, CubeGraphCutTest){
    // path to files
    std::string inputPath = "data/test/cube10x10x10/cube.mhd";
    std::string forgroundPath = "data/test/cube10x10x10/foregroundMask.mhd";
    std::string backgroundPath = "data/test/cube10x10x10/backgroundMask.mhd";
    std::string expectedPath = "data/test/cube10x10x10/expectedResult.mhd";
    std::string outputPath = "data/test/cube10x10x10/output.mhd";

    // read the images
    TInput::Pointer inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
    TForeground::Pointer foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
    TBackground::Pointer backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
    TOutput::Pointer expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

    // set images
    graphCutFilter->SetInputImage(inputImage);
    graphCutFilter->SetForegroundImage(foregroundMask);
    graphCutFilter->SetBackgroundImage(backgroundMask);

    // set parameters
    graphCutFilter->SetForegroundPixelValue(255);
    graphCutFilter->SetBackgroundPixelValue(0);
    graphCutFilter->SetSigma(50.0);
    graphCutFilter->SetLambda(1.0);
    graphCutFilter->SetBoundaryDirectionTypeToBrightDark();

    // compare the results: I_Result(x)-I_Expected(x)==0
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    IOHelper::writeImage<TOutput>(graphCutFilter->GetOutput(), outputPath.c_str());

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(0, pixelSum);
}

TEST_F(TestSegmentation, CubeGraphCutTestWithNoise){
    // path to files
    std::string inputPath = "data/test/cube10x10x10/cubeNoisy_0p01.mhd";
    std::string forgroundPath = "data/test/cube10x10x10/foregroundMask.mhd";
    std::string backgroundPath = "data/test/cube10x10x10/backgroundMask.mhd";
    std::string expectedPath = "data/test/cube10x10x10/expectedResult.mhd";

    // read the images
    TInput::Pointer inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
    TForeground::Pointer foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
    TBackground::Pointer backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
    TOutput::Pointer expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

    // set images
    graphCutFilter->SetInputImage(inputImage);
    graphCutFilter->SetForegroundImage(foregroundMask);
    graphCutFilter->SetBackgroundImage(backgroundMask);

    // set parameters
    graphCutFilter->SetForegroundPixelValue(255);
    graphCutFilter->SetBackgroundPixelValue(0);
    graphCutFilter->SetSigma(50.0);
    graphCutFilter->SetLambda(1.0);
    graphCutFilter->SetBoundaryDirectionTypeToBrightDark();

    // compare the results: I_Result(x)-I_Expected(x)==0
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(0, pixelSum);
}

TEST_F(TestSegmentation, FemurGraphCutTest){
    // path to files
    std::string inputPath = "data/test/femur/input.nrrd";
    std::string forgroundPath = "data/test/femur/foreground.nrrd";
    std::string backgroundPath = "data/test/femur/background.nrrd";
    std::string expectedPath = "data/test/femur/expectedResult.nrrd";
    std::string resultPath = "data/test/femur/output.nrrd";

    // read the images
    TInput::Pointer inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
    TForeground::Pointer foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
    TBackground::Pointer backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
    TOutput::Pointer expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

    // set images
    graphCutFilter->SetInputImage(inputImage);
    graphCutFilter->SetForegroundImage(foregroundMask);
    graphCutFilter->SetBackgroundImage(backgroundMask);

    // set parameters
    graphCutFilter->SetForegroundPixelValue(255);
    graphCutFilter->SetBackgroundPixelValue(0);
    graphCutFilter->SetSigma(50.0);
    graphCutFilter->SetLambda(1.0);
    graphCutFilter->SetBoundaryDirectionTypeToNoDirection();

    // compare the results: I_Result(x)-I_Expected(x)==0
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    IOHelper::writeImage<TOutput>(graphCutFilter->GetOutput(), resultPath.c_str());

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(0, pixelSum);
}

TEST_F(TestSegmentation, SetPixelValues){
    // path to files
    std::string inputPath = "data/test/cube10x10x10/cube.mhd";
    std::string forgroundPath = "data/test/cube10x10x10/foregroundMask.mhd";
    std::string backgroundPath = "data/test/cube10x10x10/backgroundMask.mhd";
    std::string expectedPath = "data/test/cube10x10x10/expectedResult.mhd";

    // read the images
    TInput::Pointer inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
    TForeground::Pointer foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
    TBackground::Pointer backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
    TOutput::Pointer expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

    // set images
    graphCutFilter->SetInputImage(inputImage);
    graphCutFilter->SetForegroundImage(foregroundMask);
    graphCutFilter->SetBackgroundImage(backgroundMask);

    // set parameters
    graphCutFilter->SetSigma(50.0);
    graphCutFilter->SetLambda(1.0);
    graphCutFilter->SetBoundaryDirectionTypeToBrightDark();

    // set pixel values
    int fgPixelValue = 33;
    int bgPixelValue = 5;
    graphCutFilter->SetForegroundPixelValue(fgPixelValue);
    graphCutFilter->SetBackgroundPixelValue(bgPixelValue);

    // compare the results: I_Result(x)-I_Expected(x)
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    // calculate our expected values
    int totalNumberOfVoxels = 10*10*10;
    int expectedNumberOfFgVoxels = 3*3*3; // as defined by the test data generation script
    int expectedNumberOfBgVoxels = totalNumberOfVoxels - expectedNumberOfFgVoxels;

    int imageExpectedResultPixelSum = expectedNumberOfFgVoxels * 255 + expectedNumberOfBgVoxels * 0;
    int imageResultPixelSum = expectedNumberOfFgVoxels * fgPixelValue + expectedNumberOfBgVoxels * bgPixelValue;
    int expectedPixelSum = imageResultPixelSum - imageExpectedResultPixelSum;

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(expectedPixelSum, pixelSum);
}

TEST_F(TestSegmentation, ConvertIndex){
    // path to files
    std::string inputPath = "data/test/cube10x10x10/cube.mhd";
    std::string forgroundPath = "data/test/cube10x10x10/foregroundMask.mhd";
    std::string backgroundPath = "data/test/cube10x10x10/backgroundMask.mhd";
    std::string expectedPath = "data/test/cube10x10x10/expectedResult.mhd";

    // read the images
    TInput::Pointer inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
    TForeground::Pointer foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
    TBackground::Pointer backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
    TOutput::Pointer expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

    // set images
    graphCutFilter->SetInputImage(inputImage);
    graphCutFilter->SetForegroundImage(foregroundMask);
    graphCutFilter->SetBackgroundImage(backgroundMask);

    // test some indizes
    TInput::RegionType region = inputImage->GetLargestPossibleRegion();
    itk::Index<3> i1 = {{0,0,0}};
    int r1 = graphCutFilter->ConvertIndexToVertexDescriptor(i1, region);
    ASSERT_EQ(0, r1);

    itk::Index<3> i2 = {{9,0,0}};
    int r2 = graphCutFilter->ConvertIndexToVertexDescriptor(i2, region);
    ASSERT_EQ(9, r2);

    itk::Index<3> i3 = {{0,1,0}};
    int r3 = graphCutFilter->ConvertIndexToVertexDescriptor(i3, region);
    ASSERT_EQ(10, r3);

    itk::Index<3> i4 = {{1,1,0}};
    int r4 = graphCutFilter->ConvertIndexToVertexDescriptor(i4, region);
    ASSERT_EQ(11, r4);

    itk::Index<3> i5 = {{0,0,1}};
    int r5 = graphCutFilter->ConvertIndexToVertexDescriptor(i5, region);
    ASSERT_EQ(100, r5);

    itk::Index<3> i6 = {{9,9,9}};
    int r6 = graphCutFilter->ConvertIndexToVertexDescriptor(i6, region);
    ASSERT_EQ(999, r6);
}