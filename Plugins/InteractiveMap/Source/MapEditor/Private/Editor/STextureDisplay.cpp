#include "Editor/STextureDisplay.h"
#include "Engine/Texture2D.h"

void STextureDisplay::Construct(const FArguments& InArgs)
{
    MainBrush = CreateBrush(nullptr, FVector2D(1028, 1028));
    
    ChildSlot
    [
       SNew(SBorder).Padding(FMargin(20.0f))
       [
           SNew(SImage).Image(this, &STextureDisplay::GetMainBrush)
       ]
    ];
}

const FSlateBrush* STextureDisplay::GetMainBrush() const
{
    return  MainBrush.Get();
}

void STextureDisplay::SetMainBrush(const TSharedPtr<FSlateBrush>& InBrush)
{
    MainBrush = InBrush;
}

void STextureDisplay::SetMainBrush(UTexture2D* InTexture)
{
    if(!InTexture)
        return;
    
    if(!MainBrush.IsValid() )
    {
        MainBrush = CreateBrush(InTexture, FVector2D(1028, 1028));
        MainBrush->SetResourceObject(InTexture);
        // MainImage->SetImage(MainBrush.Get());
    }
    else if (MainBrush.IsValid() && InTexture)
    {
        MainBrush->SetResourceObject(InTexture);
    }
}

FReply STextureDisplay::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    float oldZoom = CurrentZoom;
    float WheelDelta = MouseEvent.GetWheelDelta(); // Get the mouse wheel delta
    
    CurrentZoom = FMath::Clamp(CurrentZoom += WheelDelta * 0.1f, 1.0f, 10.0f); // Clamp zoom range
    
    // Calculate new UVRegion for zoom
    const FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    const FVector2D ImageSize = MyGeometry.GetLocalSize();
    const FVector2D MouseUV = LocalMousePosition / ImageSize;

    const FVector2D UVSize = (1.0f / CurrentZoom) * FVector2D(1.0f, 1.0f);
    const FVector2D MouseOffset = MouseUV - CurrentUVRegion.Min;
    const FVector2D ScaledOffset = MouseOffset * (oldZoom / CurrentZoom);
    
    CurrentUVRegion.Min = MouseUV - ScaledOffset;
    CurrentUVRegion.Max = CurrentUVRegion.Min + UVSize;

    // Clamp UVRegion to valid texture bounds [0, 1]
    ClampUVRegion(CurrentUVRegion);
    MainBrush->SetUVRegion(CurrentUVRegion);
    return SCompoundWidget::OnMouseWheel(MyGeometry, MouseEvent);
}

FReply STextureDisplay::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        bIsDragging = true;
        DragStartLocation = MouseEvent.GetScreenSpacePosition();
    }
    return SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply STextureDisplay::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = false;
    }
    return SCompoundWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply STextureDisplay::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsMouseButtonDown(FKey(EKeys::LeftMouseButton)))
    {
        // Calculate the mouse movement delta
        FVector2D CurrentMousePosition = MouseEvent.GetScreenSpacePosition();
        FVector2D MouseDelta = CurrentMousePosition - DragStartLocation;

        // Convert delta to UV space
        FVector2D UVDelta = (MouseDelta / CurrentZoom * 1.0f) / MainBrush->ImageSize;

        // Update the UVRegion based on the delta
        CurrentUVRegion.Min -= UVDelta;
        CurrentUVRegion.Max -= UVDelta;

        // Clamp the UV region to avoid showing empty areas outside the texture
        ClampUVRegion(CurrentUVRegion);

        MainBrush->SetUVRegion(CurrentUVRegion);
        DragStartLocation = CurrentMousePosition;
    }
    return SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);
}

void STextureDisplay::ClampUVRegion(FBox2D& uvRegion)
{
    FVector2D UVSize = uvRegion.Max - uvRegion.Min;

    // Ensure the UV region stays within [0, 1]
    uvRegion.Min = uvRegion.Min.ClampAxes(0.0f, 1.0f - UVSize.X);
    uvRegion.Max = uvRegion.Max.ClampAxes(UVSize.X, 1.0f);
}

TSharedPtr<FSlateBrush> STextureDisplay::CreateBrush(UTexture2D* Texture, const FVector2D& Size)
{
    TSharedPtr<FSlateBrush> Brush = MakeShared<FSlateBrush>();
    if (Texture)
    {
        Brush->SetResourceObject(Texture);
    }
    Brush->DrawAs = ESlateBrushDrawType::Image;
    Brush->ImageSize = Size;
    return Brush;
}
