#include <gtest/gtest.h>

// ITK
#include <itkImage.h>
#include <itkSubtractImageFilter.h>
#include <itkStatisticsImageFilter.h>

#include "IOHelper.hxx"
#include "ImageGraphCut3DFilter.h"

class TestImageGraphCut3DFilter : public ::testing::Test {
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

TEST_F(TestImageGraphCut3DFilter, CubeGraphCutTest){
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

    // compare the results: I_Result(x)-I_Expected(x)==0
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(0, pixelSum);
}

TEST_F(TestImageGraphCut3DFilter, CubeGraphCutTestWithNoise){
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

TEST_F(TestImageGraphCut3DFilter, FemurGraphCutTest){
    // path to files
    std::string inputPath = "data/test/femur/input.nrrd";
    std::string forgroundPath = "data/test/femur/foreground.nrrd";
    std::string backgroundPath = "data/test/femur/background.nrrd";
    std::string expectedPath = "data/test/femur/expectedResult.nrrd";

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
    graphCutFilter->SetBoundaryDirectionTypeToNoDirection();

    // compare the results: I_Result(x)-I_Expected(x)==0
    substractFilter->SetInput1(graphCutFilter->GetOutput());
    substractFilter->SetInput2(expectedResultImage);
    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    double pixelSum = statisticsFilter->GetSum();
    ASSERT_DOUBLE_EQ(0, pixelSum);
}