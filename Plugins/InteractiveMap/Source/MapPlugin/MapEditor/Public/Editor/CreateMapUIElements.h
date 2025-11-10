// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
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
	static void CreateWidgetForStruct(UScriptStruct* Struct, const FString& PackagePath, TSubclassOf<UUserWidget> DefaultWidgetForFields = nullptr);
	static void CreateMapModeWidget(UMapObject* MapObject,  const FString& PackagePath, TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons = nullptr);
};
