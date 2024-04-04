// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "BirdEyeController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class INTERACTIVEMAP_API ABirdEyeController : public APlayerController
{
	GENERATED_BODY()

public:
	ABirdEyeController();


	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CameraMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CameraRotAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SelectAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseScrollAction;


	// UI Action to open country editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenCountryEditorAction;
protected:
	// To add mapping context
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Input Actions")
	void MouseClick();
	UFUNCTION(BlueprintCallable, Category = "Input Actions")
	void CameraMovement(const FInputActionInstance& instance);

	UFUNCTION(BlueprintCallable, Category = "Input Actions")
	void CameraZoom(const FInputActionInstance& instance);

	UFUNCTION(BlueprintCallable, Category = "Input Actions")
	void StartMovement(const FVector2D& mousePos);

	UFUNCTION(BlueprintCallable, Category = "Input Actions")
	void ToggleCountryEditor();


protected:
	UPROPERTY(EditAnywhere, Category = "Mouse Click")
	float zOffset = 300.0f;

	bool bProvinceSelected = false;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bMoveCamera;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector2D MoveInput;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector2D ViewportCenter;


	UPROPERTY(Transient)
	class AInteractiveMap* Map;
	UPROPERTY(Transient)
	class AMapPawn* MapPawn;
};


