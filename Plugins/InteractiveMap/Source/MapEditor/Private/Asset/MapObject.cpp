// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/MapObject.h"

#if WITH_EDITOR
void UMapObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	OnObjectChanged.Broadcast();
}
#endif
