#include "Editor/SMapTextureViewer.h"
#include "Engine/Texture2D.h"

void STextureViewer::Construct(const FArguments& InArgs)
{
    MainBrush = CreateBrush(nullptr, FVector2D(1028, 1028));
    TSharedRef<SVerticalBox> ButtonsBox = SNew(SVerticalBox)
    + SVerticalBox::Slot();
    for(int i = 0; i < 4; i++)
    {
        Brushes.Add(CreateBrush(nullptr, FVector2D(128, 128)));
        ButtonsBox->AddSlot()
        .AutoHeight()
        [
            SNew(SButton)
            .OnClicked(this, &STextureViewer::ChangeImageSelected)
        ];
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
               .OnClicked(this, &STextureViewer::ChangeImageSelected)
               [
                   SNew(SImage)
                   .Image(Brushes[0].Get())
               ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
               SNew(SButton)
               .OnClicked(this, &STextureViewer::ChangeImageSelected)
               [
                   SNew(SImage)
                   .Image(Brushes[1].Get())
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
