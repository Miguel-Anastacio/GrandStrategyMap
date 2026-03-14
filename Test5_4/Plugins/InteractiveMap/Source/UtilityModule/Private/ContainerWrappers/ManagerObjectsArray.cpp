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

bool UTkManagerObjectsArray::Remove_BP(UObject* Object)
{
	return ArrayWrapper.Remove(Object);
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

void UTkManagerObjectsArray::SetAt_BP(const int Index, UObject* Object)
{
	if(ArrayWrapper.SetAt(Index, Object))
	{
		OnObjectChanged.Broadcast(Object);
	}
}

UObject* UTkManagerObjectsArray::At_BP(const int Index)
{
	return *ArrayWrapper.At(Index);
}

UObject* UTkManagerObjectsArray::Last_BP()
{
	if(ArrayWrapper.IsEmpty())
		return nullptr;
	
	return ArrayWrapper.Last();
}

bool UTkManagerObjectsArray::IsEmpty() const
{
	return ArrayWrapper.Get().IsEmpty();
}
