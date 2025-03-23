// Copyright 2024 An@stacioDev All rights reserved.
#include "ContainerWrappers/ManagerObjectsArray.h"

UTkManagerObjectsArray::UTkManagerObjectsArray(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTkManagerObjectsArray::PostInitProperties()
{
	UObject::PostInitProperties();
	ArrayWrapper.SetDelegates(OnObjectAdded, OnObjectRemoved, OnArraySet, OnArrayCleared);
}

void UTkManagerObjectsArray::Add_BP(UObject* Object)
{
	ArrayWrapper.Add(Object);
}

void UTkManagerObjectsArray::AddMultiple_BP(const TArray<UObject*>& Multiple)
{
	ArrayWrapper.AddMultiple(Multiple);
}

void UTkManagerObjectsArray::Remove_BP(UObject* Object)
{
	ArrayWrapper.Remove(Object);
}

void UTkManagerObjectsArray::Clear_BP()
{
	ArrayWrapper.Clear();
}

TArray<UObject*> UTkManagerObjectsArray::GetArray_BP() const
{
	return ArrayWrapper.Get();
}

void UTkManagerObjectsArray::SetArray_BP(const TArray<UObject*>& NewObjects)
{
	ArrayWrapper.Set(NewObjects);
}
