#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
// #include "Editor/EditorWidgets/Public/SEditorViewport.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"

class FAdvancedPreviewScene;
class UTexture2D;  // Forward declaration for UTexture2D class

/**
 * Custom Editor Widget to manage texture selection, details, and display.
 */
class SMapEditorMenu : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMapEditorMenu)
        : _EditingObject(nullptr)
    {}
        SLATE_ARGUMENT(UTexture*, EditingObject)   // Pointer to the texture that the editor will edit
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs);

    /** Called when a texture is selected from the list */
    void OnTextureSelected(TSharedPtr<UTexture2D> SelectedTexture, ESelectInfo::Type SelectInfo);

    /** Updates the details panel based on the selected texture */
    void RefreshDetailsPanel();

    /** Updates the main window to display the selected texture */
    void RefreshImagePreview() const;

protected:
    /** Creates the texture selection panel */
    TSharedRef<SWidget> CreateTextureSelectionPanel();

    /** Creates the details panel */
    TSharedRef<SWidget> CreateDetailsPanel() ;

    TSharedRef<ITableRow> GenerateTextureRow(TSharedPtr<UTexture2D> Texture, const TSharedRef<STableViewBase>& OwnerTable);
    
    /** Creates the main window to display the texture */
    TSharedRef<SWidget> CreateMainWindow() const;

private:
    /** The list of available textures */
    TArray<TSharedPtr<UTexture2D>> AvailableTextures;

    /** The currently selected texture */
    TSharedPtr<UTexture2D> CurrentTexture;

    /** The details view for the selected texture */
    TSharedPtr<IDetailsView> DetailsView;

    /** The advanced preview scene or other custom rendering for the texture */
    TSharedPtr<FAdvancedPreviewScene> AdvancedPreviewScene;

    /** The preview image or actor for rendering the texture in the main window */
    TSharedPtr<SImage> PreviewImage;

    /** The viewport client, if needed for advanced previewing */
    // TSharedPtr<FEditorViewportClient> ViewportClient;

};