#include <gtest/gtest.h>

// ITK
#include <itkImage.h>
#include <itkSubtractImageFilter.h>
#include <itkStatisticsImageFilter.h>
#include <lib/gtest-1.7.0/include/gtest/gtest.h>

// helpers
#include "IOHelper.hxx"
#include "ImageGraphCut3D.h"

class TestImageGraphCut3D : public ::testing::Test {
protected:

    // image types
    typedef itk::Image<short, 3> TInput;
    typedef itk::Image<unsigned int, 3> TMask;
    typedef TMask TForeground;
    typedef TMask TBackground;
    typedef TMask TOutput;
    typedef itk::Image<int, 3> TIntImage;

    // graphcut
    typedef ImageGraphCut3D <TInput, TForeground, TBackground, TOutput> Graph3DType;

    // image compare
    typedef itk::SubtractImageFilter<Graph3DType::OutputImageType, TOutput, TIntImage> TDifferenceFilter;
    typedef itk::StatisticsImageFilter<TDifferenceFilter::OutputImageType> TStatisticsFilter;

    virtual void SetUp() {
        // path to files
        std::string inputPath = "data/test/cube10x10x10/cube.mhd";
        std::string inputNoisyPath = "data/test/cube10x10x10/cubeNoisy_0p01.mhd";
        std::string forgroundPath = "data/test/cube10x10x10/foregroundMask.mhd";
        std::string backgroundPath = "data/test/cube10x10x10/backgroundMask.mhd";
        std::string expectedPath = "data/test/cube10x10x10/expectedResult.mhd";

        // read
        inputImage = IOHelper::readImage<TInput>(inputPath.c_str());
        inputImageNoisy = IOHelper::readImage<TInput>(inputNoisyPath.c_str());
        foregroundMask = IOHelper::readImage<TForeground>(forgroundPath.c_str());
        backgroundMask = IOHelper::readImage<TBackground>(backgroundPath.c_str());
        expectedResultImage = IOHelper::readImage<TOutput>(expectedPath.c_str());

        // calculate non zero pixels for hard constraints
        foregroundPixels = getNonZeroPixels<TMask>(foregroundMask);
        backgroundPixels = getNonZeroPixels<TMask>(backgroundMask);

        // setup filters to compare the results
        substractFilter = TDifferenceFilter::New();
        statisticsFilter = TStatisticsFilter::New();
    }

    // virtual void TearDown() {}

    template<typename TImage>
    std::vector<itk::Index<3> > getNonZeroPixels(const TImage *const image) {
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

    // image references
    TInput::Pointer inputImage;
    TInput::Pointer inputImageNoisy;
    TForeground::Pointer foregroundMask;
    TBackground::Pointer backgroundMask;
    Graph3DType::OutputImageType::Pointer outputImage;
    TOutput::Pointer expectedResultImage;

    // hard constraints
    std::vector<itk::Index<3> > foregroundPixels;
    std::vector<itk::Index<3> > backgroundPixels;

    // image compare filter
    TDifferenceFilter::Pointer substractFilter;
    TStatisticsFilter::Pointer statisticsFilter;
};

TEST_F(TestImageGraphCut3D, BasicGraphTest){
    Graph3DType graphCut;
    graphCut.SetInputImage(inputImage);
    graphCut.SetBoundaryDirectionTypeToBrightDark();
    graphCut.SetSources(foregroundPixels);
    graphCut.SetSinks(backgroundPixels);
    graphCut.PerformSegmentation();
    outputImage = graphCut.GetSegmentMask();

    substractFilter->SetInput1(outputImage);
    substractFilter->SetInput2(expectedResultImage);
    substractFilter->Update();

    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    int pixelSum = statisticsFilter->GetSum();

    ASSERT_EQ(pixelSum, 0);
}

TEST_F(TestImageGraphCut3D, BasicGraphTestWithNoise){
    Graph3DType graphCut;
    graphCut.SetInputImage(inputImageNoisy);
    graphCut.SetBoundaryDirectionTypeToBrightDark();
    graphCut.SetSources(foregroundPixels);
    graphCut.SetSinks(backgroundPixels);
    graphCut.PerformSegmentation();
    outputImage = graphCut.GetSegmentMask();

    substractFilter->SetInput1(outputImage);
    substractFilter->SetInput2(expectedResultImage);
    substractFilter->Update();

    statisticsFilter->SetInput(substractFilter->GetOutput());
    statisticsFilter->Update();

    int pixelSum = statisticsFilter->GetSum();

    ASSERT_EQ(pixelSum, 0);
}