#ifndef __IOHelper_hxx_
#define __IOHelper_hxx_

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

class IOHelper {
public:
    template<class TImage> static typename TImage::Pointer readImage(const char *path){
        typedef itk::ImageFileReader<TImage> TFileReader;
        typename TFileReader::Pointer reader = TFileReader::New();
        reader->SetFileName(path);
        typename TImage::Pointer img = reader->GetOutput();
        reader->Update();
        img->DisconnectPipeline();
        return img;
    }

    template<class TImage> static void writeImage(typename TImage::Pointer img, const char *path){
        typedef itk::ImageFileWriter<TImage> TFileWriter;
        typename TFileWriter::Pointer writer = TFileWriter::New();
        writer->SetFileName(path);
        writer->SetInput(img);
        writer->Update();
    }
};

#endif