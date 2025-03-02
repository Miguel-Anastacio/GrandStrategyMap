// Copyright 2024 An@stacioDev All rights reserved.

#include "GenericStructWidget.h"
#include "UObject/Field.h"
#include "DataBasedWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "GenericUserWidgetInterface.h"
#include "GenericWidgetDataMap.h"
#if WITH_EDITOR
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#endif
void UGenericStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UGenericStructWidget::NativePreConstruct()
{
	// Initialize Widget Fields
	WidgetFields.Empty();
	for (TFieldIterator<FProperty> It(GetStruct()); It; ++It)
	{
		const FProperty* Property = *It;
		if(!Property)
		{
			continue;
		}
		
		FName FieldName(*Property->GetAuthoredName());
		for(const auto& Widget : MainPanel->GetAllChildren())
		{
			if(FieldName == Widget->GetName())
			{
				if(UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
				{
					WidgetFields.Emplace(FieldName, UserWidget);
					break;
				}
			}
		}
	}
	FInstancedStruct Struct;
	Struct.InitializeAs(GetStruct());
	InitFromStruct(Struct);
	Super::NativePreConstruct();
}

void UGenericStructWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MainPanel)
	{
		MainPanel->ClearChildren();
	}
}

#if WITH_EDITOR
void UGenericStructWidget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UGenericStructWidget::InitFromStruct(const FInstancedStruct& InstancedStruct)
{
	for(TPair<FName, UUserWidget*>& WidgetPair : WidgetFields)
	{
		const FName& Key = WidgetPair.Key;
		const UUserWidget* Value = WidgetPair.Value;
		
		if(Value && Value->Implements<UGenericUserWidgetInterface>())
		{
			IGenericUserWidgetInterface::Execute_PerformAction(Value, Key, InstancedStruct);
		}
	}
}
const UScriptStruct* UGenericStructWidget::GetStruct() const
{
	if(!DataAssetWidgetMap)
		return nullptr;
	
	return DataAssetWidgetMap->StructType;
}
#if WITH_EDITOR

void UGenericStructWidget::CreatePanelSlots()
{
	if (!MainPanel)
		return;
	
	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	UGridPanel* AssetGridPanel = UAssetCreatorFunctionLibrary::GetGridPanel(this, FName("MainPanel"));
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	if(!AssetGridPanel)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Missing Main Panel"));
		return;
	}
	if(!DataAssetWidgetMap)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Please Set a DataAssetWidgetMap"));
	}
	if(!DataAssetWidgetMap->StructType)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Please Set a StructType"));
		return;
	}
	if(!WidgetTree)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Widget Tree is Null"));
		return;
	}
	AssetGridPanel->ClearChildren();
	AssetGridPanel->Modify();
	UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	
	uint8 RowIndex = 0;
	uint8 ColumnIndex = 0;
	for(const auto& [PropertyName, WidgetType] : DataAssetWidgetMap->PropertyWidgetMap)
	{
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(WidgetType))
		{
			NewWidget->Rename(*PropertyName.ToString());
			AssetGridPanel->AddChildToGrid(NewWidget, RowIndex, ColumnIndex);
		}
		ColumnIndex++;
		if(ColumnIndex >= Columns)
		{
			ColumnIndex = 0;
			RowIndex++;
		}
	}
	
	if(AssetGridPanel->GetChildrenCount() == 0)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Widget Type is not of type UUserWidget"));
	}
	
	AssetGridPanel->Modify();
	UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
}
#endif
