// Copyright 2024 An@stacioDev All rights reserved.
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
	virtual void NativeOnInitialized() override;

	/** Sets the map mode based on the clicked button. */
	UFUNCTION()
	void SetMapMode(UCustomButtonWidget* button);

	/** Saves the data to JSON format based on the clicked button. */
	UFUNCTION()
	void SaveDataToJson(UCustomButtonWidget* button);

protected:
	/** Button for saving general data. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UMapModeSelectorWidget> UMapModeSelectorWidget;

	/** Button for saving general data. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> SaveDataButton;

	/** Button for saving country-specific data. */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> SaveCountryButton;

	/** The directory path for saving province data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString DirectoryPath = FString("\\MapDataFiles\\");

	/** The filename for saving province data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileName = FString("ProvinceCustom.json");

	/** The filename for saving country data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileNameCountry = FString("CountryCustom.json");

	/** Reference to the interactive map. */
	UPROPERTY()
	class AClickableMap* GameMap;

};
