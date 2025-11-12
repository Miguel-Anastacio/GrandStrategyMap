// Copyright 2024 An@stacioDev All rights reserved.

#include "GenericWidget/GenericStructWidget.h"
#include "UObject/Field.h"
#include "DataBasedWidget.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "GenericWidget/DataDrivenUserWidgetInterface.h"
#include "GenericWidget/GenericWidgetDataMap.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "CollectionViewWidgets/MutableCollectionObjectsView.h"
#include "Kismet2/BlueprintEditorUtils.h"
#if WITH_EDITOR
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#endif

void UWPropGenGeneric::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWPropGenGeneric::NativePreConstruct()
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

void UWPropGenGeneric::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MainPanel)
	{
		MainPanel->ClearChildren();
	}
}

void UWPropGenGeneric::InitFromStruct(const FInstancedStruct& InstancedStruct)
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
}

void UWPropGenGeneric::InitFromObject(const UObject* Object)
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
#if WITH_EDITOR
const UStruct* UWPropGenGeneric::GetDataClass() const
{
	if(!DataAssetWidgetMap)
		return nullptr;
	
	return DataAssetWidgetMap->GetDataClass();
}
#endif
void UWPropGenGeneric::InitFromData(const UStruct* ClassType, const void* Data)
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
	
void UWPropGenGeneric::InitializeWidgetFields()
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
void UWPropGenGeneric::CreateGenericWidget(UPropGenWidgetMapDataAsset* DataWidgetMap)
{
	this->DataAssetWidgetMap = DataWidgetMap;
	CreatePanelSlots();
}

void UWPropGenGeneric::CreatePanelSlots() const
{
    CreateMainPanel();
    
    UGridPanel* AssetGridPanel = Cast<UGridPanel>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(this, FName("MainPanel")));
    UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
    UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(GetClass()->ClassGeneratedBy);
    
    if(!AssetGridPanel)
    {
       UE_LOG(LogDataBasedWidget, Error, TEXT("Missing Main Panel"));
       return;
    }
    if(!IsValid(DataAssetWidgetMap))
    {
       UE_LOG(LogDataBasedWidget, Error, TEXT("Please Set a DataAssetWidgetMap"));
       return;
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
    if(!WidgetBP)
    {
       UE_LOG(LogDataBasedWidget, Error, TEXT("Widget Blueprint is Null"));
       return;
    }
    
    // Clean up GUIDs for widgets that will be removed
	UAtkWidgetEditorFunctionLibrary::ClearChildren(WidgetBP, AssetGridPanel);
    
    // Create new widgets FIRST
    uint8 RowIndex = 0;
    uint8 ColumnIndex = 0;
    TArray<UWidget*> NewlyCreatedWidgets;
    for(const auto& [PropertyName, WidgetType] : DataAssetWidgetMap->PropertyWidgetMap)
    {
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(WidgetType))
		{
			// Generate unique name
			FName UniqueName = MakeUniqueObjectName(MainAssetWidgetTree, WidgetType, FName(*PropertyName.ToString()));
			NewWidget->Rename(*UniqueName.ToString(), MainAssetWidgetTree, REN_DontCreateRedirectors);

			NewWidget->SetCategoryName(PropertyName.ToString());

			// Add to grid (this makes it part of the widget tree)
			AssetGridPanel->AddChildToGrid(NewWidget, RowIndex, ColumnIndex);
			UpdateGridPosition(ColumnIndex, RowIndex);

			// Store for GUID registration
			NewlyCreatedWidgets.Add(NewWidget);
		}
    }
    
    // Clean up any remaining orphaned GUIDs
    // UAtkWidgetEditorFunctionLibrary::CleanupAllOrphanedGUIDs(this, WidgetBP);
    
    // Register GUIDs for all newly created widgets
	UAtkWidgetEditorFunctionLibrary::RegisterNewlyCreatedWidgets(NewlyCreatedWidgets, WidgetBP);
    
    if(AssetGridPanel->GetChildrenCount() == 0)
    {
       UE_LOG(LogDataBasedWidget, Error, TEXT("Widget Type is not of type UUserWidget"));
    }
    
    AssetGridPanel->Modify();
    MainAssetWidgetTree->Modify();
    WidgetBP->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
}
void UWPropGenGeneric::CreateMainPanel() const
{
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(GetClass()->ClassGeneratedBy);
    
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("WidgetTree is null!"));
		return;
	}
    
	if (!WidgetBP)
	{
		UE_LOG(LogDataBasedWidget, Error, TEXT("WidgetBlueprint is null!"));
		return;
	}
    
	if (!MainAssetWidgetTree->RootWidget)
	{
		// Construct the root widget
		UGridPanel* NewRootWidget = MainAssetWidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), FName("MainPanel"));
       
		if (NewRootWidget)
		{
			// Generate and register a GUID for the root widget
			FGuid NewGuid = FGuid::NewGuid();
			WidgetBP->WidgetVariableNameToGuidMap.Add(NewRootWidget->GetFName(), NewGuid);
          
			// Set as root widget
			MainAssetWidgetTree->RootWidget = NewRootWidget;
			// Mark as modified
			UE_LOG(LogDataBasedWidget, Log, TEXT("Created MainPanel with GUID: %s"), *NewGuid.ToString());
		}
		else
		{
			UE_LOG(LogDataBasedWidget, Error, TEXT("Failed to construct MainPanel widget!"));
		}
	}
}

#endif
void UWPropGenGeneric::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	InitFromObject(ListItemObject);
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

void UWPropGenGeneric::UpdateGridPosition(uint8& ColumnIndex, uint8& RowIndex) const
{
	ColumnIndex++;
	if(ColumnIndex >= Columns)
	{
		ColumnIndex = 0;
		RowIndex++;
	}
}

