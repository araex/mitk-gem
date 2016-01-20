#include "GemPerspective.h"
#include "berryIViewLayout.h"

GemPerspective::GemPerspective()
{
}

void GemPerspective::CreateInitialLayout(berry::IPageLayout::Pointer _layout)
{
    auto editorArea = _layout->GetEditorArea();
    _layout->AddView("org.mitk.views.datamanager", berry::IPageLayout::LEFT, 0.3f, editorArea);

    auto dataManagerLayout = _layout->GetViewLayout("org.mitk.views.datamanager");
    dataManagerLayout->SetCloseable(false);
    _layout->AddView("org.mitk.views.imagenavigator", berry::IPageLayout::BOTTOM, 0.75f, "org.mitk.views.datamanager");

    auto rightFolder = _layout->CreateFolder("right", berry::IPageLayout::RIGHT, 0.3f, editorArea);
    rightFolder->AddView("org.mitk.views.segmentation");
    rightFolder->AddView("org.mitk.views.imagegraphcut3dsegmentation");
    rightFolder->AddView("org.mitk.views.voxelmasktopolygonmesh");
    rightFolder->AddView("org.mitk.views.volumemesher");
    rightFolder->AddView("org.mitk.views.materialmapping");

    auto bottomRightFolder = _layout->CreateFolder("bottomright", berry::IPageLayout::BOTTOM, 0.75f, "right");
    bottomRightFolder->AddView("org.mitk.views.imagecropper");
    bottomRightFolder->AddView("org.mitk.views.paddingview");
    bottomRightFolder->AddView("org.mitk.views.resampleview");
    bottomRightFolder->AddView("org.mitk.views.remeshing");
}
