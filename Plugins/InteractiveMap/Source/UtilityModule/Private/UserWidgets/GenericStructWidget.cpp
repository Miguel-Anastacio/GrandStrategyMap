// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/GenericStructWidget.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintEditorUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Components/NamedSlot.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UserWidgets/CustomEditableText.h"

void UGenericStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UGenericStructWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UGenericStructWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	// Clear any lingering Slate widgets from the Fields array
	Fields.Empty();

	// If the MainPanel (or AssetGridPanel) has any lingering children, clear them
	if (MainPanel)
	{
		MainPanel->ClearChildren();
	}
}

void UGenericStructWidget::CreateEditableFieldWidget(const FName& FieldName, const FString& FieldValue,UClass* ClassPtr)
{
	if(UCustomEditableText* NewWidget = WidgetTree->ConstructWidget<UCustomEditableText>(ClassPtr))
	{
		NewWidget->SetValues(FText::FromString(FieldValue), FText::FromString(FieldValue));
		NewWidget->SetIDText(FText::FromName(FieldName));
		// StructHolder->AddChildToVerticalBox(NewWidget);
		Fields.Add(NewWidget);
	}
}

void UGenericStructWidget::CreateFieldWidget(const FName& FieldName, const FString& FieldValue)
{
	
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
	UPanelWidget* AssetGridPanel = Cast<UPanelWidget>(MainAsset->WidgetTree->FindWidget("MainPanel"));

	AssetGridPanel->ClearChildren();
	Fields.Empty();
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		const FProperty* Property = *It;
		if (!Property)
		{
			continue;
		}
		if(UUserWidget* NewWidget = MainAsset->WidgetTree->ConstructWidget<UUserWidget>(WidgetType))
		{
			AssetGridPanel->AddChild(NewWidget);
			Fields.Emplace(NewWidget);
		}
	}
	
	AssetGridPanel->Modify();
	MainAsset->Modify();
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
	// FWidgetBlueprintEditorUt
#endif
}
