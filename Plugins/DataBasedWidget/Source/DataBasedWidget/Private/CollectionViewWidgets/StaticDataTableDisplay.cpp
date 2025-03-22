// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/VerticalBoxDataTable.h"
#include "GenericStructWidget.h"
#include "GenericUserWidgetInterface.h"

void UWPropGenStaticDataTableDisplay::NativeOnInitialized()
{
	Init();
	Super::NativeConstruct();
}

void UWPropGenStaticDataTableDisplay::Init()
{
	const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
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
	UVerticalBox* AssetVerticalBox = Cast<UVerticalBox>(UAssetCreatorFunctionLibrary::GetPanelWidget(this, FName("Container")));
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
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
	UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	for(const FInstancedStruct& Struct : UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable))
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
	UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
}
#endif





