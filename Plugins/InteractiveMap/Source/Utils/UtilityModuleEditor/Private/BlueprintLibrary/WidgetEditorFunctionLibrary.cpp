// Copyright 2024 An@stacioDev All rights reserved.
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"

#include "UtilityModuleEditor.h"
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

void UAtkWidgetEditorFunctionLibrary::CleanupAllOrphanedGUIDs(const UUserWidget* UserWidget, UWidgetBlueprint* WidgetBP)
{
	const UWidgetTree* Tree = GetWidgetTree(UserWidget);
    
	if(!WidgetBP || !Tree)
	{
		return;
	}
    
	// Collect all valid widget names currently in the widget tree
	TSet<FName> ValidWidgetNames;
	Tree->ForEachWidget([&ValidWidgetNames](const UWidget* Widget)
	{
		if(Widget)
		{
			ValidWidgetNames.Add(Widget->GetFName());
		}
	});
    
	// Find and remove all orphaned GUIDs
	TArray<FName> OrphanedNames;
	for(const auto& Pair : WidgetBP->WidgetVariableNameToGuidMap)
	{
		if(!ValidWidgetNames.Contains(Pair.Key))
		{
			OrphanedNames.Add(Pair.Key);
		}
	}
    
	// Remove all orphaned entries
	for(const FName& OrphanedName : OrphanedNames)
	{
		WidgetBP->WidgetVariableNameToGuidMap.Remove(OrphanedName);
		UE_LOG(LogUtilityModuleEditor, Log, TEXT("Cleaned up orphaned GUID for: %s"), *OrphanedName.ToString());
	}
    
	if(OrphanedNames.Num() > 0)
	{
		WidgetBP->Modify();
		UE_LOG(LogUtilityModuleEditor, Log, TEXT("Removed %d orphaned GUID entries"), OrphanedNames.Num());
	}
}

void UAtkWidgetEditorFunctionLibrary::RegisterNewlyCreatedWidgets(const TArray<UWidget*>& NewlyCreatedWidgets,
	UWidgetBlueprint* WidgetBP)
{
	for(const UWidget* NewWidget : NewlyCreatedWidgets)
	{
		if(NewWidget && !WidgetBP->WidgetVariableNameToGuidMap.Contains(NewWidget->GetFName()))
		{
			FGuid NewGuid = FGuid::NewGuid();
			WidgetBP->WidgetVariableNameToGuidMap.Add(NewWidget->GetFName(), NewGuid);
			// UE_LOG(LogUtilityModuleEditor, Log, TEXT("Added GUID for widget: %s = %s"), *NewWidget->GetFName().ToString(), *NewGuid.ToString());
		}
	}
}

void UAtkWidgetEditorFunctionLibrary::ClearChildren(UWidgetBlueprint* WidgetBP, UGridPanel* GridPanel)
{
	if(!WidgetBP || !GridPanel)
	{
		return;
	}
    
	// Get the widget tree
	UWidgetTree* WidgetTree = WidgetBP->WidgetTree;
	if(!WidgetTree)
	{
		return;
	}
    
	// Remove GUIDs and remove from widget tree
	for(UWidget* Child : GridPanel->GetAllChildren())
	{
		if(Child)
		{
			FName WidgetName = Child->GetFName();
			WidgetBP->WidgetVariableNameToGuidMap.Remove(WidgetName);
			// UE_LOG(LogUtilityModuleEditor, Log, TEXT("Removing GUID for: %s"), *WidgetName.ToString());

			// IMPORTANT: Actually garbage collect the old widgets
			Child->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
			Child->MarkAsGarbage();
			
			if(WidgetTree->RemoveWidget(Child))
			{
				// UE_LOG(LogUtilityModuleEditor, Log, TEXT("Removed widget %s from tree"), *WidgetName.ToString());
			}
		}
	}
    
	// Now clear the panel (should already be empty, but just in case)
	GridPanel->ClearChildren();
	WidgetTree->Modify();
}

#endif
