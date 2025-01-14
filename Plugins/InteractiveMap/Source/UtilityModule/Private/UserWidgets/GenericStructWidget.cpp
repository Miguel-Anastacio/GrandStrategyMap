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
	WidgetFields.Empty();
	
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if(!Property)
		{
			continue;
		}
		
		FName FieldName = Property->GetFName();
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

void UGenericStructWidget::InitFromStruct_Implementation(const FInstancedStruct& InstancedStruct)
{
	for(TPair<FName, UUserWidget*>& WidgetPair : WidgetFields)
	{
		const FName& Key = WidgetPair.Key;
		const UUserWidget* Value = WidgetPair.Value;
		if(const UCustomEditableText* EditableText = Cast<UCustomEditableText>(Value))
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
	const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
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
			NewWidget->Rename(*Property->GetName());
			AssetGridPanel->AddChildToGrid(NewWidget, RowIndex);
			RowIndex++;
		}
	}
	
	AssetGridPanel->Modify();
	MainAsset->Modify();
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
#endif
}
