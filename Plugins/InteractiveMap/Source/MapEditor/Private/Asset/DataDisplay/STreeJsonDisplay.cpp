#include "Asset/DataDisplay/STreeJsonDisplay.h"
#include "MapEditor.h"
#include "Asset/MapObject.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "UserWidgets/GenericStructWidget.h"

void SEditablePropertyWidget::Construct(const FArguments& args)
{
	DocumentInfo = args._Document;
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
			SAssignNew(EditableText, SEditableText).Text(args._Value).OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type CommitType)
			{
				if(CommitType == ETextCommit::Type::OnEnter)
				{
					TextCommitDelegate.ExecuteIfBound(this, Text, CommitType);
					// TextEditedDelegate.ExecuteIfBound(TextBlock.Get()->GetText().ToString(), EditableText.Get()->GetText().ToString());
					if(DocumentInfo)
					{
						DocumentInfo->UpdateStruct(TextBlock.Get()->GetText().ToString(), EditableText.Get()->GetText().ToString());
					}
				}
				
			})
		]
	];
}

void STreeJsonDisplay::Construct(const FArguments& InArgs, FTabManager* InTabManager)
{
	TabManager = InTabManager;
	MapObject = InArgs._MapObject;
	if(InArgs._MapObject)
	{
		RebuildTreeFromMap(InArgs._MapObject);
	}
	bButtonOneVisible = true;
	TreeView = SNew(SDocumentTreeView)
				.SelectionMode(ESelectionMode::Single)
				.ClearSelectionOnClick(false)
				.TreeItemsSource(&Documents)
				.OnGenerateRow(this, &STreeJsonDisplay::GenerateListRow)
				.OnGetChildren(this, &STreeJsonDisplay::OnGetChildren)
				.OnSelectionChanged(this, &STreeJsonDisplay::OnSelectionChanged);
	
	ChildSlot.AttachWidget(TreeView.ToSharedRef());
}

bool STreeJsonDisplay::FillDocuments(const UScriptStruct* StructType, 
	TArray<TSharedPtr<FDocumentInfo>>& Documents, const TArray<FInstancedStruct>& StructInstances)
{
	if (!StructType)
    {
        UE_LOG(LogTemp, Error, TEXT("StructType is null."));
        return false;
    }

    for (const FInstancedStruct& StructInstance : StructInstances)
    {
        if (!StructInstance.IsValid() || StructInstance.GetScriptStruct() != StructType)
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid or mismatched struct instance."));
            continue;
        }

        const void* StructData = StructInstance.GetMemory();
        if (!StructData)
        {
            UE_LOG(LogTemp, Error, TEXT("StructData is null."));
            continue;
        }

        // Process the properties of the struct
        TSharedPtr<FDocumentInfo> RootDocument = MakeShareable(new FDocumentInfo(FText::FromName(StructType->GetFName()), FText::GetEmpty()));

        for (TFieldIterator<FProperty> It(StructType); It; ++It)
        {
            FProperty* Property = *It;
            if (!Property)
            {
                UE_LOG(LogTemp, Warning, TEXT("Encountered null property in struct '%s'."), *StructType->GetName());
                continue;
            }

            bool bResult = false;
            FString PropertyValue = UADStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, StructData, bResult);

        	UE_LOG(LogInteractiveMapEditor, Display, TEXT("Property Name: %s"), *Property->GetFName().ToString());
            if (bResult)
            {
                FName PropertyName = Property->GetFName();

                // If the property name is "ID", create a root document
                if (PropertyName == FName("ID"))
                {
                    RootDocument->DisplayName = FText::FromString(PropertyValue);
                }
                else
                {
                    // Add sub-documents for other properties
                    RootDocument->AddSubDocument(MakeShareable(new FDocumentInfo(FText::FromName(PropertyName), FText::FromString(PropertyValue))));
                }
            }
        }

        // Add the populated root document to the documents array
        if (RootDocument.IsValid())
        {
        	RootDocument->DocumentIndex = Documents.Num();
            Documents.Add(RootDocument);
        }
    }

    return true;
}

bool STreeJsonDisplay::FillMapDocuments(UMapObject* MapObject, TArray<TSharedPtr<FDocumentInfo>>& Documents)
{
	if (!MapObject)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("MapObject is null."));
		return false;
	}
	
	UScriptStruct* StructType = MapObject->StructType;
	const auto StructInstances = MapObject->GetMapData();
	
	if (!StructType)
    {
        UE_LOG(LogInteractiveMapEditor, Error, TEXT("StructType is null."));
        return false;
    }

    for (const FInstancedStruct& StructInstance : StructInstances)
    {
        if (!StructInstance.IsValid() || StructInstance.GetScriptStruct() != StructType)
        {
            UE_LOG(LogInteractiveMapEditor, Error, TEXT("Invalid or mismatched struct instance."));
            continue;
        }

        const void* StructData = StructInstance.GetMemory();
        if (!StructData)
        {
            UE_LOG(LogInteractiveMapEditor, Error, TEXT("StructData is null."));
            continue;
        }

        // Process the properties of the struct
        TSharedPtr<FDocumentInfo> RootDocument = MakeShareable(new FDocumentInfo(FText::FromName(StructType->GetFName()), FText::GetEmpty()));
        for (TFieldIterator<FProperty> It(StructType); It; ++It)
        {
            FProperty* Property = *It;
            if (!Property)
            {
                UE_LOG(LogInteractiveMapEditor, Warning, TEXT("Encountered null property in struct '%s'."), *StructType->GetName());
                continue;
            }

            bool bResult = false;
            FString PropertyValue = UADStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, StructData, bResult);
            if (bResult)
            {
                const FName PropertyName = Property->GetFName();

                // If the property name is "ID", create a root document
                if (PropertyName == FName("ID"))
                {
                    RootDocument->DisplayName = FText::FromString(PropertyValue);
                }
                else
                {
                    // Add sub-documents for other properties
                    RootDocument->AddSubDocument(MakeShareable(new FDocumentInfo(FText::FromName(PropertyName), FText::FromString(PropertyValue), RootDocument)));
                }
            }
        }

        // Add the populated root document to the documents array
        if (RootDocument.IsValid())
        {
        	RootDocument->DocumentIndex = Documents.Num();
        	RootDocument->MapObject = MapObject;
            Documents.Add(RootDocument);
        }
    }
    return true;
}

TSharedRef<ITableRow> STreeJsonDisplay::GenerateListRow(TSharedPtr< FDocumentInfo > InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew( STableRow< TSharedRef<FDocumentInfo> >, OwnerTable )
	[
		SNew(SEditablePropertyWidget).LabelText(InItem->DisplayName).Value(InItem->SomeData).Document(InItem.Get())
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

void STreeJsonDisplay::RebuildTree(const UScriptStruct* StructType, const TArray<FInstancedStruct>& StructInstances)
{
	Documents.Empty();
	bool bResult = FillDocuments(StructType, Documents, StructInstances);
	if( TreeView.IsValid() )
	{
		TreeView->RequestTreeRefresh();
	}
}

void STreeJsonDisplay::RebuildTreeFromMap(UMapObject* Map)
{
	Documents.Empty();
	bool bResult = FillMapDocuments(Map, Documents);
	TArray<FDocumentInfo> DocumentInfos;
	DocumentInfos.Reserve(Documents.Num());
	for(const auto& doc : Documents)
	{
		DocumentInfos.Add(*doc);
	}
	if( TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
	}
}

void STreeJsonDisplay::OnGetChildren(TSharedPtr<FDocumentInfo> Item, TArray<TSharedPtr<FDocumentInfo>>& OutChildren)
{
	const auto& SubCategories = Item->GetSubDocuments();
	OutChildren.Append(SubCategories);
}

void STreeJsonDisplay::OnSelectionChanged(TSharedPtr<FDocumentInfo> Item, ESelectInfo::Type SelectInfo)
{
	// UE_LOG(LogInteractiveMapEditor, Warning, TEXT("Selected Item: %s"), *Item->DisplayName.ToString());
}

void STreeJsonDisplay::SelectDocument(const TSharedPtr<FDocumentInfo>& DocumentInfo)
{
	if(ensure(TreeView.IsValid()))
	{
		TreeView->SetSelection(DocumentInfo);
	}
}

void STreeJsonDisplay::UpdateMap(const FString& Property, const FString& Value)
{
	if(MapObject)
	{
		// MapObject->UpdateTileProperty()
	}
}
