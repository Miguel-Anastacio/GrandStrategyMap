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


class SButtonWithImage : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SButtonWithImage)
    {}
        SLATE_EVENT(FOnClicked, OnClicked)
        SLATE_ATTRIBUTE(const FSlateBrush*, Image)
        SLATE_ARGUMENT(FName, ID)
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs);

    TAttribute<const FSlateBrush*> ImageAttr;
    FOnClicked OnClickedDelegate;
    FName ID;
};

/**
 * Custom Editor Widget to manage texture selection, details, and display.
 */
DECLARE_DELEGATE_OneParam(FTextureChanged, FName);
class STextureViewer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STextureViewer)
    {}
        SLATE_EVENT(FTextureChanged, OnTextureSelected)
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs);
    void SetTextures(const TArray<TPair<FName, UTexture2D*>>& Textures);

private:
    static TSharedPtr<FSlateBrush> CreateBrush(UTexture2D* Texture, const FVector2D& Size);
    const FSlateBrush* GetBrush(int index) const;
    TSharedRef<SButtonWithImage> CreateImagePreview(int index) const;
    
    TStaticArray<TPair<FName, TSharedPtr<FSlateBrush>>, 4> BrushPairs;
    FTextureChanged TextureSelected;
    
};