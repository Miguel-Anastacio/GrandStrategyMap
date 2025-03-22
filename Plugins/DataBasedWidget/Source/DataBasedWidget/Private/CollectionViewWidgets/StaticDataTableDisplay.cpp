// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/StaticDataTableDisplay.h"
#include "GenericStructWidget.h"
#include "GenericUserWidgetInterface.h"
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"

void UWPropGenStaticDataTableDisplay::NativeOnInitialized()
{
	Init();
	Super::NativeConstruct();
}

void UWPropGenStaticDataTableDisplay::Init()
{
	const TArray<FInstancedStruct> DataTableItems = UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	if(UVerticalBox* VerticalBox = Cast<UVerticalBox>(Execute_GetCollectionContainer(this)))
	{
		VerticalBox->ClearChildren();
		for(FInstancedStruct DataStruct : DataTableItems)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(this, WidgetClass);
			if(UGenericStructWidget* GenericStructWidget = Cast<UGenericStructWidget>(Widget))
			{
				GenericStructWidget->InitFromStruct(DataStruct);
			}
			VerticalBox->AddChildToVerticalBox(Widget);
		}
	}
}

void UWPropGenStaticDataTableDisplay::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass)
{
	WidgetClass = MembersWidgetClass;
}

void UWPropGenStaticDataTableDisplay::SetDataTable_Implementation(UDataTable* NewDataTable)
{
	DataTable = NewDataTable;
	
	if(FApp::IsGame())
	{
		// Init();
	}
	else
	{
#if WITH_EDITOR
	RefreshContainer();
#endif
	}
}

#if WITH_EDITOR
void UWPropGenStaticDataTableDisplay::RefreshContainer() const
{
	UVerticalBox* AssetVerticalBox = Cast<UVerticalBox>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(this, FName("Container")));
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	if(!AssetVerticalBox)
	{
		return;
	}
	if(!MainAssetWidgetTree)
	{
		return;
	}
	AssetVerticalBox->ClearChildren();
	AssetVerticalBox->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
	for(const FInstancedStruct& Struct : UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable))
	{
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(WidgetClass))
		{
			if(UGenericStructWidget* GenericStructWidget = Cast<UGenericStructWidget>(NewWidget))
			{
				GenericStructWidget->InitFromStruct(Struct);
			}
			AssetVerticalBox->AddChildToVerticalBox(NewWidget);
		}
	}
	AssetVerticalBox->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
}
#endif





