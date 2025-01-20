// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/GenericStructWidget.h"

#include "UtilityModule.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UserWidgets/CustomEditableText.h"
#include "UserWidgets/GenericUserWidgetInterface.h"

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
		
		FName FieldName(*Property->GetDisplayNameText().ToString());
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

void UGenericStructWidget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// when struct changes populate WidgetTypesMap with Names and default Widget 
	if(PropertyChangedEvent.GetPropertyName() == FName("StructType"))
	{
		WidgetTypesMap.Empty();
		if(StructType)
		{
			for (TFieldIterator<FProperty> It(StructType); It; ++It)
			{
				const FProperty* Property = *It;
				if (!Property)
				{
					continue;
				}
				WidgetTypesMap.Emplace(FName(*Property->GetDisplayNameText().ToString()), DefaultWidgetType);
			}
		}
	}
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

void UGenericStructWidget::CreatePanelSlots()
{
#if WITH_EDITOR
	if (!MainPanel)
		return;

	const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass());
	const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());

	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	UGridPanel* AssetGridPanel = Cast<UGridPanel>(MainAsset->WidgetTree->FindWidget("MainPanel"));
	if(!AssetGridPanel)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Missing Main Panel"));
		return;
	}
	if(!StructType)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Please Set a StructType"));
		return;
	}
	
	AssetGridPanel->ClearChildren();
	int RowIndex = 0;
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property)
		{
			continue;
		}
		FName PropertyName = FName(*Property->GetDisplayNameText().ToString());
		if(const TSubclassOf<UUserWidget>* WidgetType = WidgetTypesMap.Find(PropertyName))
		{
			// TODO - BUG WHEN CREATING PANEL SLOTS THAT ALREADY EXIST
			if(UUserWidget* NewWidget = MainAsset->WidgetTree->ConstructWidget<UUserWidget>(*WidgetType))
			{
				NewWidget->Rename(*PropertyName.ToString());
				AssetGridPanel->AddChildToGrid(NewWidget, RowIndex);
				RowIndex++;
			}
		}
	}
	
	AssetGridPanel->Modify();
	MainAsset->Modify();
	if(AssetGridPanel->GetChildrenCount() == 0)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Widget Type is not of type UUserWidget"));
	}
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
#endif
}
