// Copyright 2024 An@stacioDev All rights reserved.

#include "GenericStructWidget.h"
#include "UObject/Field.h"
#include "DataBasedWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "GenericUserWidgetInterface.h"
#include "GenericWidgetDataMap.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "CollectionViewWidgets/CollectionViewWidgets.h"
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
#if WITH_EDITOR
	if (!GIsPlayInEditorWorld)
	{
		if(const UClass* DataClass = DataAssetWidgetMap->GetClassAsUClass())
		{
			InitFromObject(DataClass->ClassDefaultObject);
		}
		else if(const UScriptStruct* ScriptStruct = DataAssetWidgetMap->GetClassAsScriptStruct())
		{
			const FInstancedStruct InstanceStruct(ScriptStruct);
			InitFromStruct(InstanceStruct);
		}
	}
#endif
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

void UGenericStructWidget::InitFromStruct(const FInstancedStruct& InstancedStruct)
{
	if(WidgetFields.IsEmpty())
	{
		InitializeWidgetFields();
	}
	
	for(TPair<FName, UUserWidget*>& WidgetPair : WidgetFields)
	{
		const FName& PropertyName = WidgetPair.Key;
		UUserWidget* Widget = WidgetPair.Value;
		
		if(Widget && Widget->Implements<UPropGenDataDrivenUserWidgetInterface>())
		{
			IPropGenDataDrivenUserWidgetInterface::Execute_InitFromStruct(Widget, PropertyName, InstancedStruct);
		}
	}
	UADStructUtilsFunctionLibrary::LogInstancedStruct(InstancedStruct);
}

void UGenericStructWidget::InitFromObject(const UObject* Object)
{
	// Handle Instance Struct Wrappers
	if(const UPropGenStructWrapper* WrapperForStruct = Cast<UPropGenStructWrapper>(Object))
	{
		InitFromStruct(WrapperForStruct->GetStructInstance());
		return;
	}

	if(WidgetFields.IsEmpty())
	{
		InitializeWidgetFields();
	}
	
	for(TPair<FName, UUserWidget*>& WidgetPair : WidgetFields)
	{
		const FName& PropertyName = WidgetPair.Key;
		UUserWidget* Widget = WidgetPair.Value;
			
		if(Widget && Widget->Implements<UPropGenDataDrivenUserWidgetInterface>())
		{
			IPropGenDataDrivenUserWidgetInterface::Execute_InitFromUObject(Widget, PropertyName, Object);
		}
	}
}

const UStruct* UGenericStructWidget::GetDataClass() const
{
	if(!DataAssetWidgetMap)
		return nullptr;
	
	return DataAssetWidgetMap->GetDataClass();
}

void UGenericStructWidget::InitFromData(const UStruct* ClassType, const void* Data)
{
	for(TPair<FName, UUserWidget*>& WidgetPair : WidgetFields)
	{
		const FName& PropertyName = WidgetPair.Key;
		UUserWidget* Widget = WidgetPair.Value;
		
		if(Widget && Widget->Implements<UPropGenDataDrivenUserWidgetInterface>())
		{
			if (const IPropGenDataDrivenUserWidgetInterface* InterfaceInstance = Cast<IPropGenDataDrivenUserWidgetInterface>(Widget))
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
			if(FieldName == Widget->GetCategoryName())
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

#if WITH_EDITOR
void UGenericStructWidget::CreateGenericWidget(UWidgetMapDataAsset* DataWidgetMap)
{
	this->DataAssetWidgetMap = DataWidgetMap;
	CreatePanelSlots();
}

void UGenericStructWidget::CreatePanelSlots() const
{
	CreateMainPanel();
	
	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	UGridPanel* AssetGridPanel = Cast<UGridPanel>(UAssetCreatorFunctionLibrary::GetPanelWidget(this, FName("MainPanel")));
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	if(!AssetGridPanel)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("Missing Main Panel"));
		return;
	}
	if(!IsValid(DataAssetWidgetMap))
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
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(WidgetType, PropertyName))
		{
			NewWidget->SetCategoryName(PropertyName.ToString());
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

void UGenericStructWidget::CreateMainPanel() const
{
	UWidgetTree* MainAssetWidgetTree = UAssetCreatorFunctionLibrary::GetWidgetTree(this);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetTree is null!"));
		return;
	}
	
	if (!MainAssetWidgetTree->RootWidget)
	{
		MainAssetWidgetTree->RootWidget = MainAssetWidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), FName("MainPanel"));
		// MainAssetWidgetTree->RootWidget = MainPanel;
		// MainPanel = Cast<UGridPanel>(MainAssetWidgetTree->RootWidget);
		MainAssetWidgetTree->Modify();
		UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(this);
	}
}

void UGenericStructWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	InitFromObject(ListItemObject);
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
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
#endif

