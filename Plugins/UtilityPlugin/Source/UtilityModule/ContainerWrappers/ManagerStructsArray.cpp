// Copyright 2024 An@stacioDev All rights reserved.
#include "ContainerWrappers/ManagerObjectsArray.h"

void UTkManagerObjectsArray::Add(UObject* Object)
{
	if(!Object)
		return;
	
	Objects.Add(Object);
	OnObjectAdded.Broadcast(Object);
}	

void UTkManagerObjectsArray::Remove(UObject* Object)
{
	if(!Object)
		return;

	int32 Index = Objects.IndexOfByPredicate([Object](const UObject* Element)
	{
		return Element == Object;
	});
	if (Index != INDEX_NONE)
	{
		Objects.RemoveAtSwap(Index);
		OnObjectRemoved.Broadcast(Object);
	}
}

void UTkManagerObjectsArray::Clear()
{
	Objects.Empty();
	OnArrayCleared.Broadcast();
}

TArray<UObject*> UTkManagerObjectsArray::GetObjects() const
{
	return Objects;
}

void UTkManagerObjectsArray::SetObjects(const TArray<UObject*>& NewObjects)
{
	Objects = NewObjects;
	OnArraySet.Broadcast(Objects);
}
