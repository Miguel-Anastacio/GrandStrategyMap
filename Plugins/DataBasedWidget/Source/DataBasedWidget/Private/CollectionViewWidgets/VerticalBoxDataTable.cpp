// Copyright 2024 An@stacioDev All rights reserved.

#include "CollectionViewWidgets/VerticalBoxDataTable.h"
#include "GenericStructWidget.h"
#include "GenericUserWidgetInterface.h"

void UWPropGenVerticalBoxDataTable::NativeConstruct()
{
	const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	Execute_InitFromStructs(this, DataTableItems);
	Super::NativeConstruct();
}

void UWPropGenVerticalBoxDataTable::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UWPropGenVerticalBoxDataTable::InitFromStructs_Implementation(const TArray<FInstancedStruct>& Structs)
{
	if(UVerticalBox* VerticalBox = Cast<UVerticalBox>(Execute_GetCollectionContainer(this)))
	{
		VerticalBox->ClearChildren();
		for(FInstancedStruct DataStruct : Structs)
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

void UWPropGenVerticalBoxDataTable::InitFromObjects_Implementation(const TArray<UObject*>& Objects)
{
	if(UVerticalBox* VerticalBox = Cast<UVerticalBox>(Execute_GetCollectionContainer(this)))
	{
		VerticalBox->ClearChildren();
		for(const UObject* Object : Objects)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(this, WidgetClass);
			if(UGenericStructWidget* GenericStructWidget = Cast<UGenericStructWidget>(Widget))
			{
				GenericStructWidget->InitFromObject(Object);
			}
			VerticalBox->AddChildToVerticalBox(Widget);
		}
	}
}

void UWPropGenVerticalBoxDataTable::SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass)
{
	WidgetClass = MembersWidgetClass;
}

void UWPropGenVerticalBoxDataTable::SetDataTable_Implementation(UDataTable* NewDataTable)
{
	if (DataTable.Get() == NewDataTable)
	{
		return;
	}
	
	DataTable = NewDataTable;

#if WITH_EDITOR
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
#endif

	
#if WITH_EDITOR
	return;
#else
	// At runtime load data table and re init ListView
	const TArray<FInstancedStruct> DataTableItems = UDataManagerFunctionLibrary::GetArrayOfInstancedStructsSoft(DataTable);
	InitFromStructs(DataTableItems);
#endif
}




