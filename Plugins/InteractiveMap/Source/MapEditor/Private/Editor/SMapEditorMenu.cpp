#include "Editor/SMapEditorMenu.h"
#include "Engine/Texture2D.h"

void SMapEditorMenu::Construct(const FArguments& InArgs)
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
            .OnClicked(this, &SMapEditorMenu::ChangeImageSelected)
            // [
            //     SNew(STextBlock).Text("Edit")
            // ]
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
               .OnClicked(this, &SMapEditorMenu::ChangeImageSelected)
               [
                   SNew(SImage)
                   .Image(Brushes[0].Get())
               ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
               SNew(SButton)
               .OnClicked(this, &SMapEditorMenu::ChangeImageSelected)
               [
                   SNew(SImage)
                   .Image(Brushes[1].Get())
               ]
            ]
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SImage).Image(this, &SMapEditorMenu::GetMainBrush)
        ]
       //  + SHorizontalBox::Slot()
       // [
       //     SNew(STextBlock).Text(FText::FromString("Change Image"))
       // ]
    ];
}

FReply SMapEditorMenu::ChangeImageSelected()
{
    // MainImage = Images[0];
    return FReply::Handled();
}

void SMapEditorMenu::OnTextureChanged(UTexture2D* texture)
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

const FSlateBrush* SMapEditorMenu::GetMainBrush() const
{
    return  MainBrush.Get();
}

// TSharedRef<SWidget> SMapEditorMenu::CreateMainWindow() const
// {
//     TSharedPtr<FSlateBrush> MyBrush = MakeShareable(new FSlateBrush());
//     // MyBrush->SetResourceObject(CurrentTexture.Get());
//     return SNew(SBorder)
//         .Padding(10)
//         [
//             SNew(SImage).Image(MyBrush.Get())
//         ];
// }

TSharedPtr<FSlateBrush> SMapEditorMenu::CreateBrush(UTexture2D* Texture, const FVector2D& Size)
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
