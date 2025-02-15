#include "SlateWidgets/DropDownSelectorWidget.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SListView.h"

// void SDropDownSelectorWidget::Construct(const FArguments& InArgs)
// {
// 	CurrentTag = InArgs._SelectedTag;
//
// 	// Convert TArray<FName> to TArray<TSharedPtr<FName>>
// 	for (const FName& AvTag : InArgs._AvailableTags)
// 	{
// 		AvailableTagOptions.Add(MakeShared<FName>(AvTag));
// 	}
//
// 	ChildSlot
// 	[
// 		SAssignNew(TagButton, SComboButton)
// 		.OnGetMenuContent(this, &SDropDownSelectorWidget::GenerateTagDropdown)
// 		.ButtonContent()
// 		[
// 			SNew(STextBlock)
// 			.Text_Lambda([this]() { return FText::FromName(CurrentTag); })
// 		]
// 	];
// }
//
// TSharedRef<SWidget> SDropDownSelectorWidget::GenerateTagDropdown()
// {
// 	return SNew(SListView<TSharedPtr<FName>>)
// 		.ListItemsSource(&AvailableTagOptions)
// 		.OnGenerateRow_Lambda([](TSharedPtr<FName> Item, const TSharedRef<STableViewBase>& OwnerTable)
// 		{
// 			return SNew(STableRow<TSharedPtr<FName>>, OwnerTable)
// 				.Content()
// 				[
// 					SNew(STextBlock).Text(FText::FromName(*Item))
// 				];
// 		});
// 		// .OnSelectionChanged(this, &SDropDownSelectorWidget::OnTagSelected);
// }
//
// void SDropDownSelectorWidget::OnTagSelected(const TSharedPtr<FName>& NewTag)
// {
// 	if (NewTag.IsValid())
// 	{
// 		CurrentTag = *NewTag;
// 		TagButton->SetIsOpen(false); // Close dropdown
// 	}
// }