// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "ManagerHUD.generated.h"


struct FProvinceData;
class UGrandStrategyHUDWidget; 
class UProvinceEditorWidget; 
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
	 * Displays the province editor widget with the given province data and ID.
	 *
	 * @param provinceData The data of the province to display in the editor.
	 * @param id The ID of the province.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DisplayProvinceEditorWidget(const FInstancedStruct& ProvinceData, int Id);

	/**
	 * Sets the visibility of the province editor widget.
	 *
	 * @param Visibility The visibility state to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetProvinceEditorVisibility(ESlateVisibility Visibility);
	
	/**
	 * Sets the reference to the interactive map.
	 *
	 * @param Map Reference to the interactive map.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetInteractiveMapReference(class AClickableMap* Map);

protected:
	void BeginPlay() override;

	/** Class of the HUD widget. */
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UGrandStrategyHUDWidget> HudWidgetClass;

	/** Class of the province editor widget. */
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UProvinceEditorWidget> ProvincedEditorWidgetClass;

	/** Reference to the HUD widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TObjectPtr<UGrandStrategyHUDWidget> HudWidget;

	/** Reference to the province editor widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TObjectPtr<UProvinceEditorWidget> ProvinceEditorWidget;

	/** Reference to the interactive map. */
	UPROPERTY(Transient)
	class AClickableMap* GameMapReference;
};
