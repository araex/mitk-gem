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

#ifndef AwesomeImageInteractor_h
#define AwesomeImageInteractor_h

#include <mitkDataInteractor.h>
#include <itkIndex.h>

#include <MyAwesomeLibExports.h>

// See AwesomeImageFilter.h for details on typical class declarations
// in MITK. The actual functionality of this class is commented in its
// implementation file.

class MyAwesomeLib_EXPORT AwesomeImageInteractor final : public mitk::DataInteractor
{
public:
  mitkClassMacro(AwesomeImageInteractor, DataInteractor)
  itkFactorylessNewMacro(Self)

private:
  AwesomeImageInteractor();
  ~AwesomeImageInteractor();

  void ConnectActionsAndFunctions() override;
  void DataNodeChanged() override;

  void Paint(mitk::StateMachineAction* action, mitk::InteractionEvent* event);

  itk::Index<3> m_LastPixelIndex;
};

#endif
