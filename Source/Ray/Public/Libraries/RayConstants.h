#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RayConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class RAY_API URPGConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "CRF|Constants")
	static const FName& Breakable();
	
};

inline const FName& URPGConstants::Breakable()
{
	static const FName Name{TEXT("Breakable")};
	return Name;
}
