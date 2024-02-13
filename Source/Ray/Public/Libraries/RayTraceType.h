#pragma once

#include "CoreMinimal.h"


// In a header file or namespace
namespace RayTraceTypes
{
	const EObjectTypeQuery EOT_WorldStatic = UEngineTypes::ConvertToObjectType(ECC_WorldStatic);

	const ETraceTypeQuery ETT_WorldStatic = UEngineTypes::ConvertToTraceType(ECC_WorldStatic);

	const ECollisionChannel ECC_Interactable = ECC_GameTraceChannel1;
	
	const ETraceTypeQuery ETraceTypeQuery_Mantle = UEngineTypes::ConvertToTraceType(ECC_Interactable);


	// constexpr ECollisionChannel ECC_PathTrace = ECC_GameTraceChannel1;
	// constexpr ECollisionChannel ECC_RangeTrace = ECC_GameTraceChannel2;
	// constexpr ECollisionChannel ECC_CoverTrace = ECC_GameTraceChannel3;
	// constexpr ECollisionChannel ECC_WallTrace = ECC_GameTraceChannel4;
	//
	// const ETraceTypeQuery ETQ_PathTrace = UEngineTypes::ConvertToTraceType(ECC_PathTrace);
	// const ETraceTypeQuery ETQ_RangeTrace = UEngineTypes::ConvertToTraceType(ECC_RangeTrace);
	// const ETraceTypeQuery ETQ_CoverTrace = UEngineTypes::ConvertToTraceType(ECC_CoverTrace);
	// const ETraceTypeQuery ETQ_WallTrace  = UEngineTypes::ConvertToTraceType(ECC_WallTrace);

	/*const EObjectTypeQuery PawnObjectType = UEngineTypes::ConvertToObjectType(ECC_Pawn);
	const EObjectTypeQuery WorldStaticObjectType = UEngineTypes::ConvertToObjectType(ECC_WorldStatic);*/
}
