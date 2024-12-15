#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"

class FAdvancedPreviewScene;
class UTexture2D;  // Forward declaration for UTexture2D class

/**
 * Custom Editor Widget to manage texture selection, details, and display.
 */
class STextureDisplay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STextureDisplay)
    {}
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs);

    // UFUNCTION()

    const FSlateBrush* GetMainBrush() const;
    void SetMainBrush(const TSharedPtr<FSlateBrush>& InBrush);
    void SetMainBrush(UTexture2D* InTexture);

    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

protected:
    static TSharedPtr<FSlateBrush> CreateBrush(UTexture2D* Texture, const FVector2D& Size);
private:

    TSharedPtr<FSlateBrush> MainBrush;

    float CurrentZoom = 1.0f;
    FBox2D CurrentUVRegion = FBox2D(FVector2D(0, 0), FVector2D(1, 1));
    FVector2D DragStartLocation;
    bool bIsDragging = false;

    static void ClampUVRegion(FBox2D& uvRegion);

};