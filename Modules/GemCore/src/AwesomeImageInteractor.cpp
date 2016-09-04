/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include <AwesomeImageInteractor.h>

#include <mitkImage.h>
#include <mitkImagePixelWriteAccessor.h>
#include <mitkInteractionPositionEvent.h>

#include <limits>

// Helper function to get an image from a data node.
static mitk::Image::Pointer GetImage(mitk::DataNode::Pointer dataNode)
{
  if (dataNode.IsNull())
    mitkThrow();

  mitk::Image::Pointer image = dynamic_cast<mitk::Image*>(dataNode->GetData());

  if (image.IsNull())
    mitkThrow();

  return image;
}

// Helper function to get a geometry of an image for a specific time step.
static mitk::BaseGeometry::Pointer GetGeometry(mitk::Image::Pointer image, unsigned int timeStep)
{
  mitk::TimeGeometry::Pointer timeGeometry = image->GetTimeGeometry();

  if (timeGeometry.IsNull())
    mitkThrow();

  auto geometry = timeGeometry->GetGeometryForTimeStep(timeStep);

  if (geometry.IsNull())
    mitkThrow();

  return geometry;
}

// The actual painting happens here. We're using a write accessor to gain safe
// write access to our image. The whole image volume for a given time step is
// locked. However, it's also possible - and preferable - to lock the slice of
// interest only.
template <typename T>
static void Paint(mitk::Image::Pointer image, itk::Index<3> index, unsigned int timeStep)
{
  // As soon as the ImagePixelWriteAccessor object goes out of scope at the
  // end of this function, the image will be unlocked again (RAII).
  mitk::ImagePixelWriteAccessor<T> writeAccessor(image, image->GetVolumeData(timeStep));
  writeAccessor.SetPixelByIndex(index, std::numeric_limits<T>::min());

  // Don't forget to update the modified time stamp of the image. Otherwise,
  // everything downstream wouldn't recognize that the image changed,
  // including the rendering system.
  image->Modified();
}

// Helper function to multiplex the actual Paint function call for different
// pixel types. As it's cumbersome and ugly, you may want to avoid such
// functions by using ITK for the actual painting and use the ITK access
// macros like we did for the AwesomeImageFilter.
static void Paint(mitk::Image::Pointer image, itk::Index<3> index, unsigned int timeStep)
{
  switch (image->GetPixelType().GetComponentType())
  {
  case itk::ImageIOBase::CHAR:
    Paint<char>(image, index, timeStep);
    break;

  case itk::ImageIOBase::UCHAR:
    Paint<unsigned char>(image, index, timeStep);
    break;

  case itk::ImageIOBase::SHORT:
    Paint<short>(image, index, timeStep);
    break;

  case itk::ImageIOBase::USHORT:
    Paint<unsigned short>(image, index, timeStep);
    break;

  case itk::ImageIOBase::INT:
    Paint<int>(image, index, timeStep);
    break;

  case itk::ImageIOBase::UINT:
    Paint<unsigned int>(image, index, timeStep);
    break;

  default:
    mitkThrow();
  }
}

AwesomeImageInteractor::AwesomeImageInteractor()
{
}

AwesomeImageInteractor::~AwesomeImageInteractor()
{
}

void AwesomeImageInteractor::ConnectActionsAndFunctions()
{
  // Wire up this interactor with the state machine that is described by
  // resource/Interactions/Paint.xml.
  CONNECT_FUNCTION("paint", Paint)
}

void AwesomeImageInteractor::DataNodeChanged()
{
  // You almost always want to reset the state machine when the interactor
  // has been attached to another data node.
  this->ResetToStartState();
}

// The state machine is wired up with this Paint method. We wrote a few helper
// functions at the top of this files to keep this method clear and easy to
// read.
void AwesomeImageInteractor::Paint(mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  try
  {
    auto renderer = event->GetSender();

    auto image = GetImage(this->GetDataNode());
    auto timeStep = renderer->GetTimeStep();
    auto geometry = GetGeometry(image, timeStep);

    // This method is wired up to mouse events. Thus, we can safely assume
    // that the following cast will succeed and we have access to the mouse
    // position and the first intersection point of a ray originating at the
    // mouse position and shot into the scene. Convenient, isn't it? :-)
    auto positionEvent = dynamic_cast<mitk::InteractionPositionEvent*>(event);
    auto position = positionEvent->GetPositionInWorld();

    if (!geometry->IsInside(position))
      return; // Nothing to paint, as we're not inside the image bounds.

    // Okay, we're safe. Convert the mouse position to the index of the pixel
    // we're pointing at.
    itk::Index<3> index;
    geometry->WorldToIndex<3>(position, index);

    // We don't need to paint over and over again while moving the mouse
    // pointer inside the same pixel. That's especially relevant when operating
    // on zoomed images.
    if (index != m_LastPixelIndex)
    {
      // And finally...
      ::Paint(image, index, timeStep);

      // Nearly done. We request the renderer to update the render window in
      // order to see the result immediately. Actually, we should update all
      // of the render windows by caling RequestUpdateAll() instead, as the
      // painted pixels are possibly visible in other render windows, too.
      // However, we decided to prefer performance here.
      mitk::RenderingManager::GetInstance()->RequestUpdate(positionEvent->GetSender()->GetRenderWindow());
      MITK_INFO << index[0] << " " << index[1] << " " << index[2];

      m_LastPixelIndex = index;
    }
  }
  catch (...)
  {
    return;
  }
}
