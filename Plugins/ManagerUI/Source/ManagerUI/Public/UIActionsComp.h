// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIActionsComp.generated.h"
/*
	Adds input actions usually associated with UI
	(Like pause menu or a inventory menu)
	In your projects you can use this as is and add it to your player controller
	Or you can use it has a base to create your own functionality
*/

class UInputMappingContext;
class UInputAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MANAGERUI_API UUIActionsComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUIActionsComp();
	// To bind the actions in c++ call this fucntion in - YourPlayerController::SetupInputComponent
	void BindActions(class UEnhancedInputComponent* enhancedInputComponent);

	UFUNCTION(BlueprintCallable, Category="Action Mapping")
	void AddUIMappingContext(class UEnhancedInputLocalPlayerSubsystem* subsystem);
	UFUNCTION(BlueprintCallable, Category = "Action Mapping")
	void RemoveUIMappingContext(class UEnhancedInputLocalPlayerSubsystem* subsystem);

	UFUNCTION(BlueprintCallable, Category = Action)
	void ToggleInventory();
	UFUNCTION(BlueprintCallable, Category = Action)
	void Pause();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	int32 MappingContextPriority;
	/** UI MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* UIMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventoryAction;

		
};
