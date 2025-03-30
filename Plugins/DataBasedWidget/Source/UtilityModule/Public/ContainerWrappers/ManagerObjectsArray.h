// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TemplatedArrayWrapper.h"
#include "ManagerObjectsArray.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArrayChangedSignature, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArraySetSignature, const TArray<UObject*>&, Array);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArrayClearedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectChanged, const UObject*, Object);
UCLASS(BlueprintType, Blueprintable,DisplayName=ManagerObjectsArray)
class UTILITYMODULE_API UTkManagerObjectsArray : public UObject
{
	GENERATED_BODY()

public:
	explicit UTkManagerObjectsArray(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitProperties() override;

	// BP exposed methods to call TArrayWrapper methods
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Add)
	void Add_BP(UObject* Object);
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=AddMultiple)
	void AddMultiple_BP(const TArray<UObject*>& Multiple);

	// remove should give feedback
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Remove)
	bool Remove_BP(UObject* Object);
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Clear)
	void Clear_BP();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerObjectsArray, DisplayName=GetArray)
	TArray<UObject*> GetArray_BP() const;
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=SetArray)
	void SetArray_BP(const TArray<UObject*>& NewObjects);
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=SetAt)
	void SetAt_BP(const int Index, UObject* Object);
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=SetAt)
	UObject* At_BP(const int Index);
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Last)
	UObject* Last_BP();
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=IsEmpty)
	bool IsEmpty() const;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category=ManagerObjectsArray)
	FOnArrayChangedSignature OnObjectAdded;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category=ManagerObjectsArray)
	FOnArrayChangedSignature OnObjectRemoved;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category=ManagerObjectsArray)
	FOnArraySetSignature OnArraySet;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category=ManagerObjectsArray)
	FOnArrayClearedSignature OnArrayCleared;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category=ManagerObjectsArray)
	FOnObjectChanged OnObjectChanged;

protected:
	TArrayWrapper<UObject*, FOnArrayChangedSignature, FOnArraySetSignature, FOnArrayClearedSignature> ArrayWrapper;

};
