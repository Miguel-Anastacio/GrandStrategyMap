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
	InitializeWidgetFields();
	if(const UClass* DataClass = GetDataClass())
	{
		InitFromData(DataClass, DataClass->ClassDefaultObject);
	}
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
	// this crashes
	// const FName PropertyName =  PropertyChangedEvent.Property->GetFName();
	// if (PropertyName == GET_MEMBER_NAME_CHECKED(UGenericStructWidget, DataAssetWidgetMap)
	// 	|| PropertyName == GET_MEMBER_NAME_CHECKED(UGenericStructWidget, Columns))
	// {
	// 	CreatePanelSlots();
	// }
}
#endif

void UGenericStructWidget::InitFromStruct(const FInstancedStruct& InstancedStruct)
{
	InitFromData(InstancedStruct.GetScriptStruct()->GetClass(), InstancedStruct.GetMemory());
}

void UGenericStructWidget::InitFromObject(const UObject* Object)
{
	InitFromData(Object->GetClass(), Object);
}

const UClass* UGenericStructWidget::GetDataClass() const
{
	if(!DataAssetWidgetMap)
		return nullptr;
	
	return DataAssetWidgetMap->GetDataClass();
}

#if WITH_EDITOR
void UGenericStructWidget::CreateGenericWidget(UWidgetMapDataAsset* DataWidgetMap)
{
	this->DataAssetWidgetMap = DataWidgetMap;

	// The problem lies in creating the mainPanel
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetTree is null!"));
		return;
	}
	
	// Check if there is already a root widget
	if (!MainAssetWidgetTree->RootWidget)
	{
		MainPanel = MainAssetWidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), FName("MainPanel"));
		if (MainPanel)
		{
			MainAssetWidgetTree->RootWidget = MainPanel	;
		}
	}
	MainAssetWidgetTree->Modify();
	UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	
	CreatePanelSlots();
}

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
	if(!DataAssetWidgetMap->IsValid())
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Please provide a valid DataAssetWidgetMap"));
		return;
	}
	if(!MainAssetWidgetTree)
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
		
		UpdateGridPosition(ColumnIndex, RowIndex);
	}
	
	if(AssetGridPanel->GetChildrenCount() == 0)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Widget Type is not of type UUserWidget"));
	}
	
	AssetGridPanel->Modify();
	UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
}

void UGenericStructWidget::UpdateGridPosition(uint8& ColumnIndex, uint8& RowIndex) const
{
	ColumnIndex++;
	if(ColumnIndex >= Columns)
	{
		ColumnIndex = 0;
		RowIndex++;
	}
}

void UGenericStructWidget::InitFromData(const UClass* ClassType, const void* Data)
{
	for(TPair<FName, UUserWidget*>& WidgetPair : WidgetFields)
	{
		const FName& PropertyName = WidgetPair.Key;
		const UUserWidget* Widget = WidgetPair.Value;
		
		if(Widget && Widget->Implements<UGenericUserWidgetInterface>())
		{
			if (const IGenericUserWidgetInterface* InterfaceInstance = Cast<IGenericUserWidgetInterface>(Widget))
			{
				InterfaceInstance->InitFromData(PropertyName, ClassType, Data);
			}
		}
	}
}

void UGenericStructWidget::InitializeWidgetFields()
{
	if(!MainPanel)
		return;
	
	WidgetFields.Empty();
	for (TFieldIterator<FProperty> It(GetDataClass()); It; ++It)
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
#endif
