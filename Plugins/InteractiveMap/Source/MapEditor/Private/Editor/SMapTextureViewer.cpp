#include "Editor/SMapTextureViewer.h"
#include "Editor/STextureDisplay.h"
#include "Engine/Texture2D.h"

void STextureViewer::Construct(const FArguments& InArgs)
{
    TextureSelected = InArgs._OnTextureSelected;
    int test = 0;
    for(auto& pair : BrushPairs)
    {
        pair.Value = CreateBrush(nullptr, FVector2D(128, 128));
        pair.Key = FName(*FString::Printf(TEXT("Test%d"), test++));
    }
    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            CreateImagePreview(0)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            CreateImagePreview(1)
        ]
        + SHorizontalBox::Slot()
       .AutoWidth()
       [
           CreateImagePreview(2)
       ]
        + SHorizontalBox::Slot()
       .AutoWidth()
       [
          CreateImagePreview(3)
       ]
    ];
}


void STextureViewer::SetTextures(const TArray<TPair<FName, UTexture2D*>>& Textures)
{
    for(int i = 0; i < Textures.Num(); i++)
    {
        if(i >= BrushPairs.Num())
            break;
        BrushPairs[i].Key = Textures[i].Key;
        BrushPairs[i].Value.Get()->SetResourceObject(Textures[i].Value);
    }
}

const FSlateBrush* STextureViewer::GetBrush(int index) const
{
    if(index >= BrushPairs.Num() || index < 0)
        return nullptr;
    
    return BrushPairs[index].Value.Get();
}

 TSharedRef<SButtonWithImage> STextureViewer::CreateImagePreview(int index) const
{
    return SNew(SButtonWithImage)
     .ID(BrushPairs[index].Key)
     .OnClicked_Lambda([this, index]() -> FReply
     {
         TextureSelected.ExecuteIfBound(BrushPairs[index].Key);
         return FReply::Handled();
     })
     .Image_Lambda([this, index]() -> const FSlateBrush*
     {
           return GetBrush(index);
     });
}


TSharedPtr<FSlateBrush> STextureViewer::CreateBrush(UTexture2D* Texture, const FVector2D& Size)
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

void SButtonWithImage::Construct(const FArguments& InArgs)
{
    ImageAttr = InArgs._Image;
    OnClickedDelegate = InArgs._OnClicked;

    ChildSlot
    [
        SNew(SButton)
        .OnClicked(OnClickedDelegate)
        [
            SNew(SImage)
            .Image(ImageAttr)
        ]
    ];
}
