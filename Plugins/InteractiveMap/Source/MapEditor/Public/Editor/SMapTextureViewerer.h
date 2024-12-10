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
    {}
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs);

    /** Updates the main window to display the selected texture */
    FReply ChangeImageSelected();

    // UFUNCTION()
    void OnTextureChanged(UTexture2D* texture);

    const FSlateBrush* GetMainBrush() const;

protected:
    static TSharedPtr<FSlateBrush> CreateBrush(UTexture2D* Texture, const FVector2D& Size);
private:
    /** The currently selected texture */
    TArray<TSharedPtr<SImage>> Images;

    TSharedPtr<FSlateBrush> MainBrush;
    TArray<TSharedPtr<FSlateBrush>> Brushes;

};