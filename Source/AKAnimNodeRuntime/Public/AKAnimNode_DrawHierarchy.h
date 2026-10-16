// Copyright 2026 Aaron Kemner, All Rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Animation/AnimInstanceProxy.h"
#include "AKAnimNode_DrawHierarchy.generated.h"

#define PLUGIN_API AKANIMNODERUNTIME_API

UENUM(BlueprintType)
enum class EAKDrawHierarchyMode: uint8
{
	WholeHierarchy,
	ParentsAndChildren,
	OnlyParents,
	OnlyChildren
};

UENUM(BlueprintType)
enum class EAKDrawHierarchyStyle: uint8
{
	Axes,
	Points,
	Lines
};

USTRUCT(BlueprintInternalUseOnly)
struct PLUGIN_API FAKAnimNode_DrawHierarchy: public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode != EAKDrawHierarchyMode::WholeHierarchy", EditConditionHides, Category = DrawHierarchy))
	FBoneReference StartBone;

	UPROPERTY(EditAnywhere, Category = DrawHierarchy)
	EAKDrawHierarchyMode Mode = EAKDrawHierarchyMode::WholeHierarchy;
	
	UPROPERTY(EditAnywhere, Category = DrawHierarchy)
	EAKDrawHierarchyStyle Style = EAKDrawHierarchyStyle::Axes; 
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0), Category = DrawHierarchy)
	float Size = 1.f;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition="Style != EAKDrawHierarchyStyle::Axes", EditConditionHides), Category = DrawHierarchy)
	FColor Color = FColor::Emerald;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, EditCondition="Mode != EAKDrawHierarchyMode::WholeHierarchy", EditConditionHides), Category = DrawHierarchy)
	int Depth = 0;

#if ENABLE_ANIM_DEBUG	
	bool bRequestDraw = false;
	
protected:
	// Begin FAnimNode_SkeletalControlBase Interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, 
		TArray<FBoneTransform>& OutBoneTransforms) override;
	// End FAnimNode_SkeletalControlBase Interface
#endif
};

#undef PLUGIN_API
