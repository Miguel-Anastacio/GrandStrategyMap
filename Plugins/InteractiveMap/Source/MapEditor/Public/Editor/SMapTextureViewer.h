#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"

class STextureDisplay;
class FAdvancedPreviewScene;
class UTexture2D;  // Forward declaration for UTexture2D class

/**
 * Custom Editor Widget to manage texture selection, details, and display.
 */
class STextureViewer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STextureViewer)
    {}
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs);

    /** Updates the main window to display the selected texture */
    FReply ChangeImageSelected();

    // UFUNCTION()
    void OnTextureChanged(UTexture2D* texture);

    const FSlateBrush* GetMainBrush() const;

    void SetTextures(TArray<UTexture2D*> textures);

protected:
    static TSharedPtr<FSlateBrush> CreateBrush(UTexture2D* Texture, const FVector2D& Size);
private:
    const FSlateBrush* GetBrush(int index) const;
    TArray<TSharedPtr<FSlateBrush>> Brushes;
    TSharedPtr<STextureDisplay> TextureDisplay;
    
};