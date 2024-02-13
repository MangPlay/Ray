#pragma once

#include "CoreMinimal.h"
#include "RayLog.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPath.h"
#include "AssetTableRef.generated.h"


/**
 * Asset Paths Constants
 */
namespace AssetRefPath
{
	// const FString DataTablePath = TEXT("DataTable'/Game/Game_/AssetRef/AssetRef_DataTable.AssetRef_DataTable'");
	const FString TexturesPath = TEXT("DataTable'/Game/Game_/AssetRef/AssetRef_Texture.AssetRef_Texture'");
	const FString MaterialPath = TEXT("DataTable'/Game/Game_/AssetRef/AssetRef_Material.AssetRef_Material'");
	const FString BluePrintPath = TEXT("DataTable'/Game/Game_/AssetRef/AssetRef_BluePrint.AssetRef_BluePrint'");
	// const FString WidgetPath = TEXT("DataTable'/Game/Game_/AssetRef/AssetRef_Widget.AssetRef_Widget'");
	// const FString SoundPath = TEXT("DataTable'/Game/Game_/AssetRef/AssetRef_Sounds.AssetRef_Sounds'");
	// const FString MeshesPath = TEXT("DataTable'/Game/Game_/AssetRef/DT_Mesh.DT_Mesh'");
	// const FString MaterialPath = TEXT("DataTable'/Game/Game_/AssetRef/DT_Materials.DT_Materials'");
}

USTRUCT(BlueprintType)
struct FAssetReferenceTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UObject> AssetReference;
};


/**
 * DataTable 및 자산을 사용하는 유틸리티 기능을 제공합니다.
 */
class FAssetReferenceUtility
{
public:
	// 지정된 행 이름을 사용하여 지정된 DataTable에서 자산을 로드합니다.
	template <typename T>
	static T* LoadAssetFromDataTable(const FString& DataTablePath, const FName& RowName)
	{
		FString AssetPath;
		if (!VerifyConstructorContext() || !TryGetAssetPathFromDataTable(DataTablePath, RowName, AssetPath))
		{
			return nullptr;
		}

		return LoadAsset<T>(AssetPath);
	}

	// 지정된 DataTable의 행 이름을 사용하여 T 유형의 클래스를 찾습니다.
	template <typename T>
	static TSubclassOf<T> FindClassFromDataTable(const FString& DataTablePath, const FName& RowName,
	                                             bool bBluePrint = false)
	{
		FString AssetPath;
		if (!VerifyConstructorContext() || !TryGetAssetPathFromDataTable(DataTablePath, RowName, AssetPath))
		{
			return nullptr;
		}

		if (bBluePrint)
		{
			// 블루프린트 클래스의 경우 '_C' 접미사 추가
			if (AssetPath.EndsWith("_C") == false)
			{
				AssetPath += TEXT("_C");
			}
		}

		return FindClass<T>(AssetPath);
	}

private:
	// 현재 코드 실행이 생성자 컨텍스트 내에 있는지 확인합니다.
	static bool VerifyConstructorContext()
	{
		const FUObjectThreadContext& ThreadContext = FUObjectThreadContext::Get();
		if (ThreadContext.IsInConstructor > 0)
		{
			return true;
		}

		RAY_LOG_ERROR(TEXT("어설션 실패: 생성자 외부에서 호출되었습니다."));
		return false;
	}

	// 주어진 행 이름을 사용하여 DataTable에서 자산 경로를 가져오려고 시도합니다.
	static bool TryGetAssetPathFromDataTable(const FString& DataTablePath, const FName& RowName, FString& OutAssetPath)
	{
		const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *DataTablePath);
		if (!DataTable)
		{
			RAY_LOG_ERROR(TEXT("어설션 실패: DataTable '%s'을(를) 로드하지 못했습니다."), *DataTablePath);
			return false;
		}

		const FAssetReferenceTableRow* Row = DataTable->FindRow<FAssetReferenceTableRow>(
			RowName, TEXT("LookupAssetPath"));
		if (!Row)
		{
			RAY_LOG_ERROR(TEXT("어설션 실패: DataTable '%s'에서 '%s' 행을 찾지 못했습니다."), *RowName.ToString(), *DataTablePath);
			return false;
		}

		OutAssetPath = Row->AssetReference.ToString();
		return true;
	}

	// 지정된 자산 경로에서 T 유형의 자산을 로드합니다.
	template <typename T>
	static T* LoadAsset(const FString& AssetPath)
	{
		T* Asset = LoadObject<T>(nullptr, *AssetPath);
		if (!Asset)
		{
			RAY_LOG_ERROR(TEXT("'%s' 경로에서 '%s' 유형의 개체를 로드하지 못했습니다."), *T::StaticClass()->GetName(), *AssetPath);
		}
		return Asset;
	}

	// 자산 경로를 사용하여 T 유형의 클래스를 찾습니다.
	template <typename T>
	static TSubclassOf<T> FindClass(const FString& AssetPath)
	{
		ConstructorHelpers::FClassFinder<T> ClassFinder(*AssetPath);
		if (!ClassFinder.Class)
		{
			RAY_LOG_ERROR(TEXT("'%s' 경로에서 '%s' 유형의 클래스를 찾지 못했습니다."), *T::StaticClass()->GetName(), *AssetPath);
			return nullptr;
		}

		return ClassFinder.Class;
	}
};


// 예를 들어, 어떤 자산을 로드하려면 다음과 같이 합니다.
/*
UTexture2D* MyTexture = FAssetReferenceUtility::LoadAssetFromDataTable(
	AssetPath::TexturesPath, FName("MyTextureRowName"));

// 클래스를 찾으려면 다음과 같이 합니다.
TSubclassOf<AActor> MyActorClass = FAssetReferenceUtility::FindClassFromDataTable<AActor>(
	AssetPath::BluePrintPath, FName("MyActorRowName"));
*/
