// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GrandStrategyHUDWidget.generated.h"

/**
 * This is the base HUD widget for the Grand Strategy map plugin.
 * It provides functionality to interact with the game map and save data.
 */
class UCustomButtonWidget;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UGrandStrategyHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/** Sets the reference to the game map. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetGameMapReference();

protected:
	/** Native initialization override. */
	virtual void NativeOnInitialized() override;

protected:
	/** Widget to handle map mode selection */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UMapModeSelectorWidget> UMapModeSelectorWidget;

	/** Reference to the interactive map. */
	UPROPERTY()
	class AClickableMap* GameMap;

};
