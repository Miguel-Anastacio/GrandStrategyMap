#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IDetailsView.h"
#include "InstancedStruct.h"
#include "PropertyEditorModule.h"
#include "Asset/MapObject.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"

class UMapObject;

struct FDocumentInfo
{
public:
    FDocumentInfo( const FText& InDisplayName, const FText& InData )
		: DisplayName( InDisplayName ), SomeData(InData)
    {

    }

	FDocumentInfo( const FText& InDisplayName, const FText& InData, const TSharedPtr<FDocumentInfo>& Owner )
	   : DisplayName( InDisplayName ), SomeData(InData), OwnerDocument(Owner)
    {

    }
	TArray<TSharedPtr<FDocumentInfo>> GetSubDocuments() const
	{
		return SubDocuments;
	};

	void AddSubDocument(const TSharedPtr<FDocumentInfo>& DocumentInfo)
	{
		SubDocuments.Add(DocumentInfo);
	}
    FText DisplayName;
    FText SomeData;
	TArray<TSharedPtr<FDocumentInfo>> SubDocuments;
	TSharedPtr<FDocumentInfo> OwnerDocument = nullptr;
	int DocumentIndex = - 1;
	UMapObject* MapObject = nullptr;

	void UpdateStruct(const FString& Property, const FString& Value) const
	{
		if(OwnerDocument)
		{
			OwnerDocument->UpdateStruct(Property, Value);
		}
		else
		{
			if(MapObject)
            {
            	MapObject->UpdateTileProperty(DocumentIndex, Property, Value);
            }
		}
	}

	FString ToString() const
	{
		FString result = FString::Printf(TEXT("%s::%s"), *DisplayName.ToString(), *SomeData.ToString());
		result.Append(FString::Printf(TEXT(" ---- %d::%s"), DocumentIndex, *MapObject->GetFName().ToString()));
		return result;
	}

	
};

DECLARE_DELEGATE_ThreeParams(FSlateEditableTextCommitSignature, class SEditablePropertyWidget*, const FText&, ETextCommit::Type);
DECLARE_DELEGATE_TwoParams(FSlateTextEditedSignature, const FDocumentInfo&, const FString&);
class SEditablePropertyWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SEditablePropertyWidget)
	{}
		SLATE_ARGUMENT(FText, LabelText)
		SLATE_ARGUMENT(FText, Value)
		SLATE_ARGUMENT(FDocumentInfo*, Document)
	SLATE_END_ARGS()

	public:	
	void Construct(const FArguments& args);

	FSlateEditableTextCommitSignature TextCommitDelegate;
	FSlateTextEditedSignature TextEditedDelegate;
	TSharedPtr<STextBlock> TextBlock;
	TSharedPtr<SEditableText> EditableText;
	FName PropertyID;
	FDocumentInfo* DocumentInfo;
	
};


using SDocumentTreeView = STreeView<TSharedPtr<FDocumentInfo>>;
class STreeJsonDisplay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STreeJsonDisplay)
    {}
		SLATE_ARGUMENT(UMapObject*, MapObject)      
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs, FTabManager* InTabManager);

	static bool FillDocuments(const UScriptStruct* StructType, TArray<TSharedPtr<FDocumentInfo>>& Documents,
											 const TArray<FInstancedStruct>& StructInstances);

	static bool FillMapDocuments(UMapObject* MapObject, TArray<TSharedPtr<FDocumentInfo>>& Documents);

	TSharedRef<ITableRow> GenerateListRow(TSharedPtr<FDocumentInfo> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply SummonDocumentButtonClicked( TSharedRef<FDocumentInfo> DocumentName);

	void RebuildTree(const UScriptStruct* StructType, const TArray<FInstancedStruct>& StructInstances);
	void RebuildTreeFromMap(UMapObject* Map);
	
	void OnGetChildren(TSharedPtr<FDocumentInfo>Item, TArray<TSharedPtr<FDocumentInfo>>& OutChildren);
	void OnSelectionChanged(TSharedPtr<FDocumentInfo> Item, ESelectInfo::Type SelectInfo);

	void SelectDocument(const TSharedPtr<FDocumentInfo>& DocumentInfo) const;
	void SelectDocument(uint32 Index) const;
	static void LogDocuments(const TArray<TSharedPtr<FDocumentInfo>>& DocumentInfos);

	static void StructPropertiesToDocument(const FInstancedStruct& StructInstance, TSharedPtr<FDocumentInfo>& RootDocument);
	static void StructPropertiesToDocument(const FInstancedStruct& StructInstance, TSharedPtr<FDocumentInfo>& RootDocument, const TArray<FName>& IgnoredProperties);
	
	UClass* DataClass;

	FTabManager* TabManager;
	UMapObject* MapObject;

	TArray<TSharedPtr<FDocumentInfo>> Documents;
	TSharedPtr<SDocumentTreeView> TreeView;

	bool bButtonOneVisible;
};



