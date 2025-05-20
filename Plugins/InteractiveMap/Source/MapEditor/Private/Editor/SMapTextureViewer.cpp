#include "Editor/SMapTextureViewer.h"
#include "Editor/STextureDisplay.h"
#include "Engine/Texture2D.h"

void STextureViewer::Construct(const FArguments& InArgs)
{
    for(int i = 0; i < 4; i++)
    {
        Brushes.Add(CreateBrush(nullptr, FVector2D(128, 128)));
    }
    
    // MainBrush = Brushes[0];
    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
           SNew(SButton)
           .OnClicked_Lambda([this]() -> FReply
           {
               TextureDisplay->SetMainBrush(Brushes[0]);
               return FReply::Handled();
           })
           [
               SNew(SImage)
              .Image_Lambda([this]() -> const FSlateBrush*
              {
                  return GetBrush(0);
              })
           ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .OnClicked_Lambda([this]() -> FReply
            {
                TextureDisplay->SetMainBrush(Brushes[1]);
              return FReply::Handled();
            })
           [
               SNew(SImage)
               .Image_Lambda([this]() -> const FSlateBrush*
               {
                   return GetBrush(1);
               })
           ]
        ]
        + SHorizontalBox::Slot()
       .AutoWidth()
       [
           SNew(SButton)
           .OnClicked_Lambda([this]() -> FReply
           {
               TextureDisplay->SetMainBrush(Brushes[2]);
             return FReply::Handled();
           })
          [
              SNew(SImage)
              .Image_Lambda([this]() -> const FSlateBrush*
              {
                  return GetBrush(2);
              })
          ]
       ]
        + SHorizontalBox::Slot()
       .AutoWidth()
       [
           SNew(SButton)
           .OnClicked_Lambda([this]() -> FReply
           {
               TextureDisplay->SetMainBrush(Brushes[3]);
               return FReply::Handled();
           })
          [
              SNew(SImage)
              .Image_Lambda([this]() -> const FSlateBrush*
              {
                  return GetBrush(3);
              })
          ]
       ]
    ];
}

FReply STextureViewer::ChangeImageSelected()
{
    // MainImage = Images[0];
    // if()
    return FReply::Handled();
}

void STextureViewer::OnTextureChanged(UTexture2D* texture)
{
    TextureDisplay->SetMainBrush(texture);
}

void STextureViewer::SetTextures(const TArray<UTexture2D*>& Textures)
{
    for(int i = 0; i < Textures.Num(); i++)
    {
        if(i >= Brushes.Num())
            break;
        Brushes[i]->SetResourceObject(Textures[i]);
    }
}

const FSlateBrush* STextureViewer::GetBrush(int index) const
{
    return Brushes[index].Get();
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
