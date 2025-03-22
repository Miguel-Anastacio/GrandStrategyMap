// Copyright 2024 An@stacioDev All rights reserved.
#include "ContainerWrappers/ManagerObjectsArray.h"

UTkManagerObjectsArray::UTkManagerObjectsArray(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTkManagerObjectsArray::PostInitProperties()
{
	UObject::PostInitProperties();
	SetDelegates(OnObjectAdded, OnObjectRemoved, OnArraySet, OnArrayCleared);
}

void UTkManagerObjectsArray::Add_BP(UObject* Object)
{
	Add(Object);
}

void UTkManagerObjectsArray::AddMultiple_BP(const TArray<UObject*>& Multiple)
{
	AddMultiple(Multiple);
}

void UTkManagerObjectsArray::Remove_BP(UObject* Object)
{
	Remove(Object);
}

void UTkManagerObjectsArray::Clear_BP()
{
	Clear();
}

TArray<UObject*> UTkManagerObjectsArray::GetArray_BP() const
{
	return Get();
}

void UTkManagerObjectsArray::SetArray_BP(const TArray<UObject*>& NewObjects)
{
	Set(NewObjects);
}
