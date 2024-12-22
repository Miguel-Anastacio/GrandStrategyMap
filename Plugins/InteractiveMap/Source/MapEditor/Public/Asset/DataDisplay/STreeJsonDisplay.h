#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Misc/Optional.h"
#include "Framework/Docking/TabManager.h"

struct FDocumentInfo
{
public:
    FDocumentInfo( const FText& InDisplayName, const FText& InData )
		: DisplayName( InDisplayName ), SomeData(InData)
    {

    }
	const TArray<TSharedPtr<FDocumentInfo>> GetSubDocuments() const
	{
		return SubDocuments;
	};

	void AddSubDocument(const TSharedPtr<FDocumentInfo> DocumentInfo)
	{
		SubDocuments.Add(DocumentInfo);
	}
    FText DisplayName;
    FText SomeData;
	TArray< TSharedPtr<FDocumentInfo>> SubDocuments;
};

DECLARE_DELEGATE_ThreeParams(FSlateEditableTextCommitSignature, class SEditablePropertyWidget*, const FText&, ETextCommit::Type);
class SEditablePropertyWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SEditablePropertyWidget)
	{}
		SLATE_ARGUMENT(FText, LabelText)
		SLATE_ARGUMENT(FText, Value)
	SLATE_END_ARGS()

	public:	
	void Construct(const FArguments& args);

	FSlateEditableTextCommitSignature TextCommitDelegate;
	TSharedPtr<STextBlock> TextBlock;
	
};


using SDocumentTreeView = STreeView<TSharedPtr<FDocumentInfo>>;
class STreeJsonDisplay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STreeJsonDisplay)
    {}
    	SLATE_ARGUMENT(const UStruct*, StructType)        // The type of the struct
		SLATE_ARGUMENT(TArray<void*>, StructInstances)            // Pointer to the struct instance
    SLATE_END_ARGS()

    /** Constructor and widget setup */
    void Construct(const FArguments& InArgs, FTabManager* InTabManager);

	~STreeJsonDisplay()
	{
		
	};	
	static bool PopulateDocuments (const UStruct* StructType, TArray<TSharedPtr<FDocumentInfo>>& Documents,
									const TArray<void*>& StructInstances);


	TSharedRef<ITableRow> GenerateListRow(TSharedPtr<FDocumentInfo> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply SummonDocumentButtonClicked( TSharedRef<FDocumentInfo> DocumentName);


	void OnGetChildren(TSharedPtr<FDocumentInfo>Item, TArray<TSharedPtr<FDocumentInfo>>& OutChildren);
	void OnSelectionChanged(TSharedPtr<FDocumentInfo> Item, ESelectInfo::Type SelectInfo);

	void SelectDocument(const TSharedPtr<FDocumentInfo>& DocumentInfo);
	
	UClass* DataClass;

	FTabManager* TabManager;

	TArray<TSharedPtr<FDocumentInfo>> Documents;
	TSharedPtr<SDocumentTreeView> TreeView;

	bool bButtonOneVisible;
};



