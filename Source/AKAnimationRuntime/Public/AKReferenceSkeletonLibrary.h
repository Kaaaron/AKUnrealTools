// Copyright 2026 Aaron Kemner, All Rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReferenceSkeleton.h"
#include "Engine/SkeletalMesh.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AKReferenceSkeletonLibrary.generated.h"

#define PLUGIN_API AKANIMATIONRUNTIME_API
USTRUCT(BlueprintType)
struct PLUGIN_API FBlueprintReferenceSkeleton
{
	GENERATED_BODY()
	
	FBlueprintReferenceSkeleton() = default;
	FBlueprintReferenceSkeleton(USkeletalMesh* InMesh)
		: ReferenceMesh(InMesh)
	{}
	
	[[nodiscard]] bool IsValid() const
		{ return  ::IsValid(ReferenceMesh); }
	
	[[nodiscard]] const FReferenceSkeleton& Get() const
	{
		check(ReferenceMesh)
		return ReferenceMesh->GetRefSkeleton();
	}
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ReferenceSkeleton)
	TObjectPtr<USkeletalMesh> ReferenceMesh;
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EBlueprintBoneReferenceDiffFlags: uint8
{
	None			= 0 UMETA(Hidden),
	MissingInSource = 1 << 1,
	MissingInTarget = 1 << 2,
	DifferentParent = 1 << 3
};
ENUM_CLASS_FLAGS(EBlueprintBoneReferenceDiffFlags);

UCLASS(BlueprintInternalUseOnly)
class PLUGIN_API UAKReferenceSkeletonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category = "Animation|ReferenceSkeleton")
	[[nodiscard]] static FName GetParentBone(const FBlueprintReferenceSkeleton& InReferenceSkeleton, const FName InChild);
	
	UFUNCTION(BlueprintPure, Category = "Animation|ReferenceSkeleton")
	[[nodiscard]] static int GetNumBones(const FBlueprintReferenceSkeleton& InReferenceSkeleton);
	
	UFUNCTION(BlueprintCallable, Category = "Animation|ReferenceSkeleton")
	static void GetBones(const FBlueprintReferenceSkeleton& InReferenceSkeleton, TArray<FName>& OutBones);
	
	UFUNCTION(BlueprintCallable, Category = "Animation|ReferenceSkeleton")
	static void DiffHierarchies(const FBlueprintReferenceSkeleton& InSourceReferenceSkeleton, 
		const FBlueprintReferenceSkeleton& InTargetReferenceSkeleton,
		UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/AKAnimationRuntime.EBlueprintBoneReferenceDiffFlags")) const int32 DiffFlags,
		TArray<FName>& OutDiffResult); 
};

#undef PLUGIN_API