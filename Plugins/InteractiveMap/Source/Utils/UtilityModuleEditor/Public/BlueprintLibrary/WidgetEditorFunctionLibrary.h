// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#if WITH_EDITOR
#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetEditorFunctionLibrary.generated.h"

/**
 * Helper functions to modify widgets through c++ code
 */
class UObject;
UCLASS(DisplayName="Widget Editor Function Library")
class UTILITYMODULEEDITOR_API UAtkWidgetEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static class UPanelWidget* GetPanelWidget(const UUserWidget* Widget, const FName& Name);
	static class UWidgetTree* GetWidgetTree(const UUserWidget* Widget);
	static void MarkBlueprintAsModified(const UObject* Object);
	static void CleanupAllOrphanedGUIDs(const UUserWidget* Object, class UWidgetBlueprint* WidgetBP);
	static void RegisterNewlyCreatedWidgets(const TArray<class UWidget*>& NewlyCreatedWidgets, class UWidgetBlueprint* WidgetBP);
	template <typename T>
	static void ClearChildren(class UWidgetBlueprint* WidgetBP, T* GridPanel)
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
	
	// T is type used for root widget
	template <typename T>
	static void CreateRootWidget(class UUserWidget* Widget, const FName MainPanelName)
	{
		UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(Widget);
		UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Widget->GetClass()->ClassGeneratedBy);
    
		if (!MainAssetWidgetTree->RootWidget)
		{
			// Construct the root widget
			T* NewRootWidget = MainAssetWidgetTree->ConstructWidget<T>(T::StaticClass(), MainPanelName);
       
			if (NewRootWidget)
			{
				// Generate and register a GUID for the root widget
				FGuid NewGuid = FGuid::NewGuid();
				WidgetBP->WidgetVariableNameToGuidMap.Add(NewRootWidget->GetFName(), NewGuid);
          
				// Set as root widget
				MainAssetWidgetTree->RootWidget = NewRootWidget;
			}
		}
	}
	
};
#endif
