// Copyright 2024 An@stacioDev All rights reserved.
#include "ContainerWrappers/ManagerStructsArray.h"

UTkManagerStructsArray::UTkManagerStructsArray(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTkManagerStructsArray::PostInitProperties()
{
	UObject::PostInitProperties();
	ArrayWrapper.SetDelegates(OnStructAdded, OnStructRemoved, OnArraySet, OnArrayCleared);
}

void UTkManagerStructsArray::Add_BP(const FInstancedStruct& DataStruct)
{
	ArrayWrapper.Add(DataStruct);
}

void UTkManagerStructsArray::AddMultiple_BP(const TArray<FInstancedStruct>& DataStructs)
{
	ArrayWrapper.AddMultiple(DataStructs);
}

void UTkManagerStructsArray::Remove_BP(const FInstancedStruct& DataStruct)
{
	ArrayWrapper.Remove(DataStruct);
}

void UTkManagerStructsArray::Clear_BP()
{
	ArrayWrapper.Clear();
}

TArray<FInstancedStruct> UTkManagerStructsArray::GetArray_BP() const
{
	return ArrayWrapper.Get();
}

void UTkManagerStructsArray::SetArray_BP(const TArray<FInstancedStruct>& NewStructs)
{
	ArrayWrapper.Set(NewStructs);
}

FInstancedStruct& UTkManagerStructsArray::At_BP(const int Index)
{
	return *ArrayWrapper.At(Index);
}

void UTkManagerStructsArray::SetAt(const int Index, const FInstancedStruct& NewStruct)
{
	const FInstancedStruct Prev = *ArrayWrapper.At(Index);
	if(ArrayWrapper.SetAt(Index, NewStruct))
	{
		OnStructChanged.Broadcast(Prev, NewStruct);
	}
	
}
