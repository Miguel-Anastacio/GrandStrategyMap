// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#if WITH_EDITOR
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "TextureFromBufferFactory.generated.h"

UCLASS(hidecategories=Object, MinimalAPI)
class UTexture2DFromBufferFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	/** width of new texture */
	UPROPERTY()
	uint32		Width;

	/** height of new texture */
	UPROPERTY()
	uint32		Height;

	uint8* Buffer = nullptr;
	
	virtual bool ShouldShowInNewMenu() const override;
	virtual UObject* FactoryCreateNew( UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn ) override;
	void SetBuffer(uint8* buffer)
	{
		Buffer = buffer;
	};
};
#endif
