// Copyright 2024 An@stacioDev All rights reserved.
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#if WITH_EDITOR
#include "WidgetBlueprint.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Kismet2/BlueprintEditorUtils.h"

UPanelWidget* UAtkWidgetEditorFunctionLibrary::GetPanelWidget(const UUserWidget* Widget, const FName& Name)
{
	const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Widget->GetClass());
	const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	if (!MainAsset)
	{
		return nullptr;
	}
	if(!MainAsset->WidgetTree)
	{
		return nullptr;
	}
	return Cast<UPanelWidget>(MainAsset->WidgetTree->FindWidget(Name));
}

void UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(const UObject* Object)
{
	const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Object->GetClass());
	if (!WidgetBlueprintGeneratedClass)
		return;
	const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	if (!Package)
		return;
	UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	if (!MainAsset)
		return;
	MainAsset->Modify();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
}


class UWidgetTree* UAtkWidgetEditorFunctionLibrary::GetWidgetTree(const UUserWidget* Widget)
{
	const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Widget->GetClass());
	const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	if (!MainAsset)
	{
		return nullptr;
	}
	return MainAsset->WidgetTree;
}
#endif