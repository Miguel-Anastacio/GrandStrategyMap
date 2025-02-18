#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
DECLARE_DELEGATE_TwoParams(FOptionSelectedDelegate, const FName&, const FName&);
class SDropDownSelectorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDropDownSelectorWidget) {}
		SLATE_ARGUMENT(FName, PropertyName)  // Initial selected tag
		SLATE_ATTRIBUTE(FText, Text)
		SLATE_ARGUMENT(TArray<FName>, AvailableTags) // List of available tags
		SLATE_EVENT(FOptionSelectedDelegate, OnTagChanged) // Event when tag changes
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		PropertyName = InArgs._PropertyName;
		TagChanged = InArgs._OnTagChanged;
		
			for (const FName& AvTag : InArgs._AvailableTags)
			{
				AvailableTagOptions.Add(MakeShared<FName>(AvTag));
			}
		ChildSlot
		[
			SAssignNew(TagButton, SComboButton)
			.OnGetMenuContent(this, &SDropDownSelectorWidget::GenerateTagDropdown)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(InArgs._Text)
			]
		];
	}

private:
	FName PropertyName;
	TArray<TSharedPtr<FName>> AvailableTagOptions;
	FOptionSelectedDelegate TagChanged;
    
	TSharedPtr<SComboButton> TagButton;
	TSharedPtr<SListView<TSharedPtr<FName>>> TagListView;

	// Functions
	TSharedRef<SWidget> GenerateTagDropdown()
	{
		return SNew(SListView<TSharedPtr<FName>>)
		.ListItemsSource(&AvailableTagOptions)
		.OnGenerateRow_Lambda([](TSharedPtr<FName> Item, const TSharedRef<STableViewBase>& OwnerTable)
		{
			return SNew(STableRow<TSharedPtr<FName>>, OwnerTable)
				.Content()
				[
					SNew(STextBlock).Text(FText::FromName(*Item))
				];
		})
		.OnSelectionChanged(this, &SDropDownSelectorWidget::OnTagSelected);
	}
	virtual void OnTagSelected(TSharedPtr<FName> NewTag, ESelectInfo::Type SelectInfo)
	{
		if (NewTag.IsValid())
		{
			// CurrentTag = *NewTag;
			TagButton->SetIsOpen(false); // Close dropdown
			TagChanged.ExecuteIfBound(PropertyName, *NewTag);
		}
	}
};