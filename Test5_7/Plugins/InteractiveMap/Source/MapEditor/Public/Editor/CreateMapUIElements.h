// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetTree.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "CreateMapUIElements.generated.h"

class UMapObject;

/**
 * 
 */
UCLASS()
class MAPEDITOR_API UCreateMapUIElements : public UAssetActionUtility
{
	GENERATED_BODY()
	UCreateMapUIElements();

	struct DataWidgetsBps
	{
		UBlueprint* BlueprintLand;
		UBlueprint* BlueprintOcean;
	};

public:
	/*
	 * @param DefaultWidgetForFields (optional)
	 * widget used for each property of map data structs (this can be modified after on a per property basis)
	 */
	UFUNCTION(CallInEditor, Category = "Create Map UI Elements")
	virtual void CreateMapDataWidgets(
		UPARAM(meta = (AllowedClasses = "UserWidget", MustImplement = "/Script/DataBasedWidget.PropGenDataDrivenUserWidgetInterface"))
			TSubclassOf<UUserWidget>
				DefaultWidgetForFields = nullptr) const;
	
	UFUNCTION(CallInEditor, Category = "Create Map UI Elements")
	virtual void CreateMapModeSelectorWidget(
		UPARAM(meta = (AllowedClasses = "CustomButtonWidget"))
			TSubclassOf<class UCustomButtonWidget>
				DefaultWidgetForButtons = nullptr) const;
	
	
	UFUNCTION(CallInEditor, Category = "Create Map UI Elements")
	virtual void CreateAll(
		UPARAM(meta = (AllowedClasses = "UserWidget", MustImplement = "/Script/DataBasedWidget.PropGenDataDrivenUserWidgetInterface"))
			TSubclassOf<UUserWidget>
			DefaultWidgetForFields = nullptr,
		UPARAM(meta = (AllowedClasses = "CustomButtonWidget"))
			TSubclassOf<class UCustomButtonWidget>
			DefaultWidgetForButtons = nullptr) const;

protected:
	static DataWidgetsBps CreateMapDataWidgetsImpl(TSubclassOf<UUserWidget> DefaultWidgetForFields);
	static UBlueprint* CreateWidgetForStruct(UScriptStruct* Struct, const FString& PackagePath, TSubclassOf<UUserWidget> DefaultWidgetForFields = nullptr);
	
	static UBlueprint* CreateMapModeSelectorWidgetImpl(TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons);
	static UBlueprint* CreateMapModeWidget(UMapObject* MapObject,  const FString& PackagePath, TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons = nullptr);
	
	static UBlueprint* CreateTileSelectedWidget(UBlueprint* LandDataBp, UBlueprint* OceanDataBp);
	static UBlueprint* CreateTileSelectedWidget(UMapObject* MapObject, const FString& PackagePath, UBlueprint* LandDataBp, UBlueprint* OceanDataBp);
	static void AddDataWidgetsToMain(class UTileSelectedWidget* TileSelectedWidget, TSubclassOf<UUserWidget> LandWidget, TSubclassOf<UUserWidget> OceanWidget);
	
	static void CreateMapHUDWidget(UBlueprint* TileSelectedWidgetBp, UBlueprint* MapModeSelectorWidgetBlueprint);
	static void CreateMapHUDWidget(UMapObject* MapObject, const FString& PackagePath, UBlueprint* TileSelectedWidgetBp, UBlueprint* MapModeSelectorWidgetBlueprint);
	
	
	template <typename T>
	static T* GetDefaultObject(UBlueprint *Blueprint)
	{
		if (!Blueprint)
		{
			return nullptr;
		}
		if (const UBlueprintGeneratedClass *BPGeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
		{
			 T *DefaultObject = Cast<T>(BPGeneratedClass->GetDefaultObject());
			return DefaultObject;
		}
		return nullptr;
	}

};
