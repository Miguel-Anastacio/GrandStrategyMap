// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Map/MapEnums.h"
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
	void NativeOnInitialized() override;

	/** Sets the map mode based on the clicked button. */
	UFUNCTION()
	void SetMapMode(UCustomButtonWidget* button);

	/** Saves the data to JSON format based on the clicked button. */
	UFUNCTION()
	void SaveDataToJson(UCustomButtonWidget* button);

protected:
	/** Button for switching to the political map mode. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> PoliticalMapButton;

	/** Button for switching to the religious map mode. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> ReligiousMapButton;

	/** Button for switching to the culture map mode. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> CultureMapButton;

	/** Button for switching to the terrain map mode. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> TerrainMapButton;

	/** Button for saving general data. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> SaveDataButton;

	/** Button for saving country-specific data. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> SaveCountryButton;

	/** The directory path for saving province data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString DirectoryPath = FString("D:\\Dev\\Unreal\\GrandStrategyData\\Spain\\");

	/** The filename for saving province data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileName = FString("ProvinceCustom.json");

	/** The filename for saving country data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileNameCountry = FString("CountryCustom.json");

	/** Reference to the interactive map. */
	class AInteractiveMap* GameMap;

};
