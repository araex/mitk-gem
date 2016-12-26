/**
 *  MITK-GEM: Volume Mesher Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *  Copyright (c) 2016, ETH Zurich, Institute for Biomechanics, B. Helgason
 *  Copyright (c) 2016, University of Iceland, Mechanical Engineering and Computer Science, H. Pállson
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#pragma once

#include <mitkUnstructuredGridSource.h>
#include <vtkSmartPointer.h>

namespace gem
{
    class IMesher;
}

namespace mitk
{
    class Surface;
}

/*!
 * mitk Filter to volume mesh a mitk::Surface
 *
 * @author  Thomas Fitze
 */
class SurfaceToUnstructuredGridFilter : public mitk::UnstructuredGridSource
{
public:
    //! MITK Filter Interface
    //! @{
    mitkClassMacro(SurfaceToUnstructuredGridFilter, mitk::UnstructuredGridSource);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    using itk::ProcessObject::SetInput;

    /*!
     * Sets the input of the filter
     * @param pSurface  Surface to be meshed
     * @param spMesher  Mesher instance to be used
     */
    virtual void SetInput(const mitk::Surface *pSurface, std::shared_ptr <gem::IMesher> spMesher);

    virtual const mitk::Surface *GetInput();
    virtual void GenerateOutputInformation() override;
    virtual void GenerateData() override;
    //! @}

protected:
    //! Construction only via MITK smartpointer. SurfaceToUnstructuredGridFilter::New()
    SurfaceToUnstructuredGridFilter() = default;

private:
    std::shared_ptr <gem::IMesher> m_spMesher;   //!< Mesher instance
};