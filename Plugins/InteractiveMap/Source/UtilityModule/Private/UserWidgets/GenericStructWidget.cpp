// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/GenericStructWidget.h"

#include "UtilityModule.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UserWidgets/CustomEditableText.h"

void UGenericStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	int32 Index = 0;
	WidgetFields.Empty();
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property)
		{
			continue;
		}
		// MainPanel->AddChildToGrid(NewWidget, RowIndex);
		if(Index >= MainPanel->GetChildrenCount())
		{
			return;
		}
		
		UUserWidget* Widget = Cast<UUserWidget>(MainPanel->GetChildAt(Index));
		Index++;
		if(!Widget)
		{
			continue;
		}
		
		WidgetFields.Emplace(Property->GetName(), Widget);
	}
}

void UGenericStructWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UGenericStructWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	// Clear any lingering Slate widgets from the Fields array
	// WidgetFields.Empty();

	// If the MainPanel (or AssetGridPanel) has any lingering children, clear them
	if (MainPanel)
	{
		MainPanel->ClearChildren();
	}
}

void UGenericStructWidget::InitFromStruct_Implementation(const FInstancedStruct& InstancedStruct)
{
	UE_LOG(LogUtilityModule, Error, TEXT("Fields Size %d"), WidgetFields.Num());
	for(TPair<FName, TWeakObjectPtr<UUserWidget>>& WidgetPair : WidgetFields)
	{
		const FName& Key = WidgetPair.Key;
		TWeakObjectPtr<UUserWidget> Value = WidgetPair.Value;
		 if(UCustomEditableText* EditableText = Cast<UCustomEditableText>(Value.Get()))
		 {
		 	EditableText->SetIDText(FText::FromName(Key));
		 	bool bResult = false;
		 	FString Text = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(InstancedStruct, Key.ToString(), bResult);
		 	EditableText->SetValues(FText::FromString(Text), FText::FromString(Text));
		}
	}
}

void UGenericStructWidget::CreatePanelSlots()
{
#if WITH_EDITOR
	if (!MainPanel)
		return;

	const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass());

	UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());

	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	UGridPanel* AssetGridPanel = Cast<UGridPanel>(MainAsset->WidgetTree->FindWidget("MainPanel"));
	if(!AssetGridPanel)
	{
		// log
		return;
	}
	AssetGridPanel->ClearChildren();
	WidgetFields.Empty();
	int RowIndex = 0;
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property)
		{
			continue;
		}
		if(UUserWidget* NewWidget = MainAsset->WidgetTree->ConstructWidget<UUserWidget>(WidgetType))
		{
			AssetGridPanel->AddChildToGrid(NewWidget, RowIndex);
			WidgetFields.Emplace(Property->GetName(), NewWidget);
			RowIndex++;
		}
	}
	
	AssetGridPanel->Modify();
	MainAsset->Modify();
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
#endif
}
