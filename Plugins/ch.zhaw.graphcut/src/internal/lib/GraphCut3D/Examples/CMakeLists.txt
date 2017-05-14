ADD_EXECUTABLE(ImageGraphCut3DSegmentationExample ImageGraphCut3DSegmentationExample.cpp)
TARGET_LINK_LIBRARIES(ImageGraphCut3DSegmentationExample
${ITK_LIBRARIES}
${ImageGraphCut3DSegmentation_libraries})


ADD_EXECUTABLE(ImageMultiLabelGraphCut3DSegmentationExample ImageMultiLabelGraphCut3DSegmentationExample.cpp)
TARGET_LINK_LIBRARIES(ImageMultiLabelGraphCut3DSegmentationExample
        ${ITK_LIBRARIES}
        ${ImageGraphCut3DSegmentation_libraries})
