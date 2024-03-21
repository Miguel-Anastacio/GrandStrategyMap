// Fill out your copyright notice in the Description page of Project Settings.


#include "UIActionsComp.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "CoreGameHUD.h"
// Sets default values for this component's properties
UUIActionsComp::UUIActionsComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UUIActionsComp::BindActions(UEnhancedInputComponent* enhancedInputComponent)
{
	if(PauseAction)
		enhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &UUIActionsComp::Pause);
	if(InventoryAction)
		enhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &UUIActionsComp::ToggleInventory);
}
void UUIActionsComp::AddUIMappingContext(UEnhancedInputLocalPlayerSubsystem* subsystem)
{
	if(UIMappingContext)
		subsystem->AddMappingContext(UIMappingContext, MappingContextPriority);
}

void UUIActionsComp::RemoveUIMappingContext(UEnhancedInputLocalPlayerSubsystem* subsystem)
{
	if(UIMappingContext)
		subsystem->RemoveMappingContext(UIMappingContext);
}

void UUIActionsComp::ToggleInventory()
{
	if (!IsValid(GetOwner()))
	{
		return;
	}

	const APlayerController* controller = Cast<APlayerController>(GetOwner());
	if (!IsValid(controller))
	{
		return;
	}

	ACoreGameHUD* hud = Cast<ACoreGameHUD>(controller->GetHUD());
	if (IsValid(hud))
	{
		hud->ToggleInventory();
	}
}

void UUIActionsComp::Pause()
{
	if (!IsValid(GetOwner()))
	{
		return;
	}

	const APlayerController* controller = Cast<APlayerController>(GetOwner());
	if (!IsValid(controller))
	{
		return;
	}

	ACoreGameHUD* hud = Cast<ACoreGameHUD>(controller->GetHUD());
	if (IsValid(hud))
	{
		hud->TogglePause();
	}
}



// Called when the game starts
void UUIActionsComp::BeginPlay()
{
	Super::BeginPlay();

	// ...

}
