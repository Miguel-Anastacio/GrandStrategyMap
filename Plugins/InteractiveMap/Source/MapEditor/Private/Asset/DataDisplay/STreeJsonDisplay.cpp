#include "Asset/DataDisplay/STreeJsonDisplay.h"

#include "MapEditor.h"
#include "UserWidgets/CustomStructWidget.h"

void SEditablePropertyWidget::Construct(const FArguments& args)
{
	this->ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(TextBlock, STextBlock).Text(args._LabelText)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f, 0, 0, 0)
		[
			SNew(SEditableText).Text(args._Value).OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type CommitType)
			{
				if(CommitType == ETextCommit::Type::OnEnter)
				{
					TextCommitDelegate.ExecuteIfBound(this, Text, CommitType);
				}
			})
		]
	];
}

void STreeJsonDisplay::Construct(const FArguments& InArgs, FTabManager* InTabManager)
{
	TabManager = InTabManager;

	bool bResult = PopulateDocuments(InArgs._StructType, Documents, InArgs._StructInstances);
	bButtonOneVisible = true;
	TreeView = SNew(SDocumentTreeView)
				.SelectionMode(ESelectionMode::Single)
				.ClearSelectionOnClick(false)
				.TreeItemsSource(&Documents)
				.OnGenerateRow(this, &STreeJsonDisplay::GenerateListRow)
				.OnGetChildren(this, &STreeJsonDisplay::OnGetChildren)
				.OnSelectionChanged(this, &STreeJsonDisplay::OnSelectionChanged);
	
	ChildSlot.AttachWidget(TreeView.ToSharedRef());
	// SAssignNew(TreeView, SNew(SDocumentTreeView)
	// 			.SelectionMode(ESelectionMode::Single)
	// 			.ClearSelectionOnClick(false)
	// 			.TreeItemsSource(&Documents)
	// 			.OnGenerateRow(this, &STreeJsonDisplay::OnGenerateRow)
	// 			.OnGetChildren(this, &STreeJsonDisplay::OnGetChildren)
	// 			.OnSelectionChanged(this, &STreeJsonDisplay::OnSelectionChanged))
	// 	
	// this->ChildSlot
	// [
	// 	SNew(SVerticalBox)
	// 	+ SVerticalBox::Slot()
	// 	[
	// 		SNew(SListView<TSharedRef<FDocumentInfo>>)
	// 		.ItemHeight(24)
	// 		.SelectionMode(ESelectionMode::None)
	// 		.ListItemsSource( &Documents )
	// 		.OnGenerateRow( this, &STreeJsonDisplay::GenerateListRow )
	// 	]
	// 	
	// ];
}

bool STreeJsonDisplay::PopulateDocuments (const UStruct* StructType, TArray<TSharedPtr<FDocumentInfo>>& Documents,
												const TArray<void*>& StructInstances)
{
	for(const auto StructIn : StructInstances)
	{
		if(!StructType || !StructIn)
			return false;
		
		for (TFieldIterator<FProperty> It(StructType); It; ++It)
		{
			FProperty* Property = *It;
			if(!Property)
				continue;
			FName PropertyName = Property->GetFName();
			bool bResult = false;
			FString PropertyValue = UGenericStructWidget::GetPropertyValueAsString(Property, StructIn, bResult);
			if(bResult)
			{
				if(PropertyName == FName("ID"))
				{
					Documents.Add(MakeShareable(new FDocumentInfo(FText::FromName(PropertyName), FText::FromString(PropertyValue))));
				}
				else
				{
					Documents.Last()->AddSubDocument(MakeShareable(new FDocumentInfo(FText::FromName(PropertyName), FText::FromString(PropertyValue))));
				}
				
			}
		}
	}

	return true;
}

TSharedRef<ITableRow> STreeJsonDisplay::GenerateListRow(TSharedPtr< FDocumentInfo > InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew( STableRow< TSharedRef<FDocumentInfo> >, OwnerTable )
		[
			SNew(SEditablePropertyWidget).LabelText(InItem->DisplayName).Value(InItem->SomeData)
		];
}

FReply STreeJsonDisplay::SummonDocumentButtonClicked( TSharedRef<FDocumentInfo> DocumentName )
{
	TabManager->InsertNewDocumentTab
	(
		"DocTest", FTabManager::ESearchPreference::RequireClosedTab,
		SNew( SDockTab )
		.Label( DocumentName->DisplayName )
		.TabRole( ETabRole::DocumentTab )
		[
			SNew(SBox)
			.HAlign(HAlign_Center) .VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text( DocumentName->DisplayName )
			]
		]
		.TabWellContentRight()
		[
			SNew(STextBlock).Text(FText::FromString("Right Content"))
		]
	);

	return FReply::Handled();
}

void STreeJsonDisplay::OnGetChildren(TSharedPtr<FDocumentInfo> Item, TArray<TSharedPtr<FDocumentInfo>>& OutChildren)
{
	const auto& SubCategories = Item->GetSubDocuments();
	OutChildren.Append(SubCategories);
}

void STreeJsonDisplay::OnSelectionChanged(TSharedPtr<FDocumentInfo> Item, ESelectInfo::Type SelectInfo)
{
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("Selected Item: %s"), *Item->DisplayName.ToString());
}

void STreeJsonDisplay::SelectDocument(const TSharedPtr<FDocumentInfo>& DocumentInfo)
{
	if(ensure(TreeView.IsValid()))
	{
		TreeView->SetSelection(DocumentInfo);
	}
}
