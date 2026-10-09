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

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (FoldProperty), Category = DrawHierarchy)
	EAKDrawHierarchyMode Mode = EAKDrawHierarchyMode::WholeHierarchy;
	
	UPROPERTY(EditAnywhere, meta = (FoldProperty), Category = DrawHierarchy)
	EAKDrawHierarchyStyle Style = EAKDrawHierarchyStyle::Axes; 
	
	UPROPERTY(EditAnywhere, meta = (FoldProperty, ClampMin = 0), Category = DrawHierarchy)
	float Size = 1.f;
	
	UPROPERTY(EditAnywhere, meta = (FoldProperty, EditCondition="Style != EAKDrawHierarchyStyle::Axes", EditConditionHides), Category = DrawHierarchy)
	FColor Color = FColor::Emerald;
	
	UPROPERTY(EditAnywhere, meta = (FoldProperty, ClampMin = 0, EditCondition="Mode != EAKDrawHierarchyMode::WholeHierarchy", EditConditionHides), Category = DrawHierarchy)
	int Depth = 0;
#endif

#if ENABLE_ANIM_DEBUG
	// Begin FoldProperty Getters
	[[nodiscard]] EAKDrawHierarchyMode GetMode() const
		{ return GET_ANIM_NODE_DATA(EAKDrawHierarchyMode, Mode); }
	[[nodiscard]] EAKDrawHierarchyStyle GetStyle() const
		{ return GET_ANIM_NODE_DATA(EAKDrawHierarchyStyle, Style); }
	[[nodiscard]] float GetSize() const
		{ return GET_ANIM_NODE_DATA(float, Size); }
	[[nodiscard]] FColor GetColor() const
		{ return GET_ANIM_NODE_DATA(FColor, Color); }
	[[nodiscard]] int GetDepth() const
		{ return GET_ANIM_NODE_DATA(int, Depth); }
	// End FoldProperty Getters
	
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
