#include "Editor/SMapEditorMenu.h"
#include "Engine/Texture2D.h"

void SMapEditorMenu::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
       SNew(SSplitter)
       + SSplitter::Slot()
       .Value(0.3f)
       [
           CreateTextureSelectionPanel()
       ]
       + SSplitter::Slot()
       .Value(0.4f)
       [
           CreateDetailsPanel()
       ]
       + SSplitter::Slot()
       .Value(0.3f)
       [
           CreateMainWindow()
       ]
    ];
}


TSharedRef<SWidget> SMapEditorMenu::CreateTextureSelectionPanel()
{
    return SNew(SBox)
        .WidthOverride(300.0f)
        [
            SNew(SListView<TSharedPtr<UTexture2D>>)
            .ItemHeight(64)
            .ListItemsSource(&AvailableTextures) // Your array of textures
            .OnGenerateRow(this, &SMapEditorMenu::GenerateTextureRow)
            .OnSelectionChanged(this, &SMapEditorMenu::OnTextureSelected)
        ];
}

TSharedRef<ITableRow> SMapEditorMenu::GenerateTextureRow(TSharedPtr<UTexture2D> Texture, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<TSharedPtr<UTexture2D>>, OwnerTable)
        [
            SNew(STextBlock).Text(FText::FromString(Texture->GetName()))
        ];
}

TSharedRef<SWidget>SMapEditorMenu::CreateDetailsPanel()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bHideSelectionTip = true;
    DetailsViewArgs.bShowPropertyMatrixButton = false;

    DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    return DetailsView->AsShared();
}

void SMapEditorMenu::RefreshDetailsPanel()
{
    if (CurrentTexture.IsValid())
    {
        DetailsView->SetObject(CurrentTexture.Get());
    }
}

TSharedRef<SWidget> SMapEditorMenu::CreateMainWindow() const
{
    TSharedPtr<FSlateBrush> MyBrush = MakeShareable(new FSlateBrush());
    MyBrush->SetResourceObject(CurrentTexture.Get());
    return SNew(SBorder)
        .Padding(10)
        [
            SNew(SImage).Image(MyBrush.Get())
        ];
}

void SMapEditorMenu::OnTextureSelected(TSharedPtr<UTexture2D> SelectedTexture, ESelectInfo::Type SelectInfo) 
{
    CurrentTexture = SelectedTexture;
    RefreshDetailsPanel();
    RefreshImagePreview();
}

void SMapEditorMenu::RefreshImagePreview() const 
{
    
}