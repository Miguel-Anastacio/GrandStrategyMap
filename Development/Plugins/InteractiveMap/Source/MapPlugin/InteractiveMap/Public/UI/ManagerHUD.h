// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "ManagerHUD.generated.h"


class UGrandStrategyHUDWidget; 
class UTileSelectedWidget; 
class UEditCountriesContainerWidget; 

/**
 * AManagerHUD is the main HUD (Heads-Up Display) class responsible for managing widgets and interactions in the interactive map.
 */
UCLASS()
class INTERACTIVEMAP_API AManagerHUD : public AHUD
{
	GENERATED_BODY()

public:
	/**
	 * Displays the Tile Selected widget with the given province data and ID.
	 *
	 * @param Data The data of the province to display in the editor.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DisplayTileSelectedWidget(const FInstancedStruct& Data);

	/**
	 * Sets the visibility of the Tile Selected widget.
	 *
	 * @param Visibility The visibility state to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTileSelectedWidgetVisibility(ESlateVisibility Visibility);
	
protected:
	void BeginPlay() override;

	/** Class of the HUD widget. */
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UGrandStrategyHUDWidget> HudWidgetClass;

	/** Reference to the HUD widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TObjectPtr<UGrandStrategyHUDWidget> HudWidget;
};
