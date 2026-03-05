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
class UTileSelectedWidget;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UGrandStrategyHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/** Sets the reference to the game map. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetGameMapReference();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTileSelectedWidgetData(const FInstancedStruct& Data);
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTileSelectedVisibility(const ESlateVisibility Visible);
	
protected:
	/** Native initialization override. */
	virtual void NativeOnInitialized() override;

protected:
	/** Widget to handle map mode selection */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UMapModeSelectorWidget> UMapModeSelectorWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Widgets")
	UNamedSlot* SelectedSlot;
	
	UPROPERTY(EditAnywhere, Category = "Widgets", meta = (MustImplement = "WidgetDataInterface"))
	TSubclassOf<UUserWidget> TileDisplayWidgetClass;
};
