// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/GenericStructWidget.h"
#include "UObject/Field.h"
#include "UtilityModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "UserWidgets/GenericUserWidgetInterface.h"
//#if WITH_EDITOR
//#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
//#endif
void UGenericStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	WidgetFields.Empty();
	
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
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
}

void UGenericStructWidget::NativePreConstruct()
{
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
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UGenericStructWidget, StructType))
	{
		InitializeWidgetTypesMap();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// when struct changes populate WidgetTypesMap with Names and default Widget 
	// if(PropertyChangedEvent.GetPropertyName() == FName("StructType"))
}
#endif

void UGenericStructWidget::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	InitializeWidgetTypesMap();
#endif
}

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

void UGenericStructWidget::InitializeWidgetTypesMap()
{
	if(StructType)
	{
		WidgetTypesMap.Empty();
		for (TFieldIterator<FProperty> It(StructType); It; ++It)
		{
			const FProperty* Property = *It;
			if (!Property)
			{
				continue;
			}
			WidgetTypesMap.Emplace(FName(*Property->GetAuthoredName()), DefaultWidgetType);
		}
	}
}

void UGenericStructWidget::CreatePanelSlots()
{
#if WITH_EDITOR
	if (!MainPanel)
		return;
	
	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	// UGridPanel* AssetGridPanel = UAssetCreatorFunctionLibrary::GetGridPanel(this, FName("MainPanel"));
	// UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	// if(!AssetGridPanel)
	// {
	// 	UE_LOG(LogUtilityModule, Error, TEXT("Missing Main Panel"));
	// 	return;
	// }
	// if(!StructType)
	// {
	// 	UE_LOG(LogUtilityModule, Error, TEXT("Please Set a StructType"));
	// 	return;
	// }
	// if(!WidgetTree)
	// {
	// 	UE_LOG(LogUtilityModule, Error, TEXT("Widget Tree is Null"));
	// 	return;
	// }
	//
	// AssetGridPanel->ClearChildren();
	// AssetGridPanel->Modify();
	// UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	//
	// int RowIndex = 0;
	// UADStructUtilsFunctionLibrary::ForEachProperty(StructType, [this, AssetGridPanel, &RowIndex, MainAssetWidgetTree](const FProperty* Property)
	// {
	// 	const FName PropertyName = FName(*Property->GetDisplayNameText().ToString());
	// 	const TSubclassOf<UUserWidget>* WidgetTypeFromMap = WidgetTypesMap.Find(PropertyName);
	// 	TSubclassOf<UUserWidget> WidgetType = DefaultWidgetType;
	// 	if(WidgetTypeFromMap && *WidgetTypeFromMap)
	// 	{
	// 		WidgetType = *WidgetTypeFromMap;
	// 	}
	// 	if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(*WidgetType))
	// 	{
	// 		NewWidget->Rename(*PropertyName.ToString());
	// 		AssetGridPanel->AddChildToGrid(NewWidget, RowIndex);
	// 		RowIndex++;
	// 	}
	// });
	// if(AssetGridPanel->GetChildrenCount() == 0)
	// {
	// 	UE_LOG(LogUtilityModule, Error, TEXT("Widget Type is not of type UUserWidget"));
	// }
	//
	// AssetGridPanel->Modify();
	// UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
#endif
}
