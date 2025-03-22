// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#if WITH_EDITOR
#include "CoreMinimal.h"
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
	
};
#endif
