/* * *
 * Image GraphCut 3D Segmentation
 * Copyright (C) 2014 Visual Computing Lab (visualcomputinglab@dornbirn.zhaw.ch)
 * Institute of Applied Information Technology, Zurich University of Applied Sciences
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * * */

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