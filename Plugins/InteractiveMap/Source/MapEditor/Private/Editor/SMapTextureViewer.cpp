#include "Editor/SMapTextureViewer.h"
#include "Engine/Texture2D.h"

void STextureViewer::Construct(const FArguments& InArgs)
{
    MainBrush = CreateBrush(nullptr, FVector2D(1028, 1028));
    for(int i = 0; i < 4; i++)
    {
        Brushes.Add(CreateBrush(nullptr, FVector2D(128, 128)));
    }
    
    ChildSlot
    [
       SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
               SNew(SButton)
               .OnClicked_Lambda([this]() -> FReply
               {
                   MainBrush = Brushes[0];
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
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SButton)
                .OnClicked_Lambda([this]() -> FReply
                {
                  MainBrush = Brushes[1];
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
            + SVerticalBox::Slot()
           .AutoHeight()
           [
               SNew(SButton)
               .OnClicked_Lambda([this]() -> FReply
               {
                 MainBrush = Brushes[2];
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
            + SVerticalBox::Slot()
           .AutoHeight()
           [
               SNew(SButton)
               .OnClicked_Lambda([this]() -> FReply
               {
                 MainBrush = Brushes[3];
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
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SImage).Image(this, &STextureViewer::GetMainBrush)
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
    if(!texture)
        return;
    
    if(!MainBrush.IsValid() )
    {
        MainBrush = CreateBrush(texture, FVector2D(1028, 1028));
        MainBrush->SetResourceObject(texture);
        // MainImage->SetImage(MainBrush.Get());
    }
    else if (MainBrush.IsValid() && texture)
    {
        MainBrush->SetResourceObject(texture);
    }
}

const FSlateBrush* STextureViewer::GetMainBrush() const
{
    return  MainBrush.Get();
}

void STextureViewer::SetTextures(TArray<UTexture2D*> textures)
{
    for(int i = 0; i < textures.Num(); i++)
    {
        if(i >= Brushes.Num())
            break;
        Brushes[i]->SetResourceObject(textures[i]);
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
        // Brush->ImageSize.X = InArgs._ItemImage->GetSurfaceWidth();
        // Brush->ImageSize.Y = InArgs._ItemImage->GetSurfaceHeight();
    }
    Brush->DrawAs = ESlateBrushDrawType::Image;
    Brush->ImageSize = Size;
    return Brush;
}
