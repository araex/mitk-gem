## ch.zhaw.voxel2mesh MITK Workbench Plugin
Creates meshes from segmentations. Uses a customized MITK filter based on vtk. Individual steps can be enabled / disabled and customized. 

The filter chain used looks like this:
```vtkImageMedian3D || vtkImageGaussianSmooth -> vtkMarchingCubes -> vtkSmoothPolyDataFilter -> vtkDecimatePro```

Only works with unsigned char (-> binary in MITK) segmentations. Intensity != 0 is assumed to be part of the segmentation.

## Build instructions
1. Drop the repo into your existing MITK applications MITK-build/Plugin folder.
2. Add `Plugins/ch.zhaw.voxel2mesh:ON` to the `PROJECT_PLUGINS` variable in your 'Plugins.cmake' file.
3. Build your application