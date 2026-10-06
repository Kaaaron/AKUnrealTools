// Copyright 2026 Aaron Kemner, All Rights reserved.


#include "AKAnimNode_DrawHierarchy.h"

#include "HAL/IConsoleManager.h"
#include "BoneContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AKAnimNode_DrawHierarchy)

#if ENABLE_ANIM_DEBUG

static TAutoConsoleVariable<bool> CVarEnableDrawHierarchyAnimNode(
TEXT("a.AnimNode.DrawHierarchy"),
	true,
	TEXT("Enables debug drawing via DrawHierarchy AKAnimNodes."));

void FAKAnimNode_DrawHierarchy::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	FAnimNode_SkeletalControlBase::InitializeBoneReferences(RequiredBones);
	
	if (GetMode() == EAKDrawHierarchyMode::WholeHierarchy)
		{ return; }
	
	if(!StartBone.Initialize(RequiredBones))
	{
		AddValidationVisualWarning(FText::FromString(FString::Printf(TEXT("Invalid StartBone %s"), 
			*StartBone.BoneName.ToString())));
	}
}

bool FAKAnimNode_DrawHierarchy::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	if (GetMode() == EAKDrawHierarchyMode::WholeHierarchy)
		{ return RequiredBones.GetNumBones() > 0; }
	
	return StartBone.IsValidToEvaluate(RequiredBones);
}

void FAKAnimNode_DrawHierarchy::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output,
                                                                  TArray<FBoneTransform>& OutBoneTransforms)
{
	FAnimNode_SkeletalControlBase::EvaluateSkeletalControl_AnyThread(Output, OutBoneTransforms);
	
	if (!Output.AnimInstanceProxy)
	{
		return;
	}
	
	FAnimInstanceProxy& DrawProxy = *Output.AnimInstanceProxy;
	const FBoneContainer& RequiredBones = Output.AnimInstanceProxy->GetRequiredBones();
	
	if (!(bRequestDraw || CVarEnableDrawHierarchyAnimNode.GetValueOnAnyThread()))
	{
		return;
	}
	bRequestDraw = false;
	
	const EAKDrawHierarchyMode CurrentMode = GetMode();
	
	auto DrawBoneIndex = [&](const FCompactPoseBoneIndex& Bone, const FBoneContainer& Hierarchy) -> void
	{
		const FTransform Transform = Output.Pose.GetComponentSpaceTransform(Bone) * DrawProxy.GetComponentTransform();
		switch (GetStyle())
		{
			case EAKDrawHierarchyStyle::Axes:
				DrawProxy.AnimDrawDebugCoordinateSystem(Transform.GetLocation(), Transform.Rotator(), GetSize(), 
					false, -1.f, 1.f, SDPG_Foreground);
				break;
			case EAKDrawHierarchyStyle::Points:
				DrawProxy.AnimDrawDebugPoint(Transform.GetLocation(), GetSize(), GetColor(), 
					false, -1.f, SDPG_Foreground);
				break;
			case EAKDrawHierarchyStyle::Lines:
				const FCompactPoseBoneIndex ParentIdx = RequiredBones.GetParentBoneIndex(Bone);
				if (!ParentIdx.IsValid())
				{ break; }
				const FTransform Parent = Output.Pose.GetComponentSpaceTransform(ParentIdx)  * DrawProxy.GetComponentTransform();
				DrawProxy.AnimDrawDebugLine(Transform.GetLocation(), Parent.GetLocation(), GetColor(), 
					false, -1.f, GetSize(), SDPG_Foreground);
		}
	};
	
	if (CurrentMode == EAKDrawHierarchyMode::WholeHierarchy)
	{
		for (const FCompactPoseBoneIndex Idx : RequiredBones.ForEachCompactPoseBoneIndex())
		{
			DrawBoneIndex(Idx, RequiredBones);
		}
		return;
	}

	const int CurrentDepth = FMath::Max(GetDepth(), 1);
	const FCompactPoseBoneIndex StartBoneIdx = StartBone.GetCompactPoseIndex(RequiredBones);
	
	auto DrawParents = [&]()->void
	{
		int Running = CurrentDepth;
		FCompactPoseBoneIndex ParentIdx = StartBoneIdx;
		while (Running > 0 
			&& ParentIdx.IsValid())
		{
			DrawBoneIndex(ParentIdx, RequiredBones);
			ParentIdx = RequiredBones.GetParentBoneIndex(ParentIdx);
			Running--;
		}
	};
	
	auto DrawChildren = [&]()->void
	{
		for (const FCompactPoseBoneIndex Idx : RequiredBones.ForEachCompactPoseBoneIndex())
		{
			int DepthToParent = 0;
			FCompactPoseBoneIndex ChildIdx = Idx;
			while (DepthToParent < CurrentDepth 
				&& ChildIdx.IsValid())
			{
				if (StartBoneIdx == ChildIdx)
				{
					DrawBoneIndex(Idx, RequiredBones);
					break;
				}
				ChildIdx = RequiredBones.GetParentBoneIndex(ChildIdx);
				DepthToParent++;
			}
		}
	};
	
	switch (CurrentMode)
	{
		case EAKDrawHierarchyMode::ParentsAndChildren:
			DrawParents();
			DrawChildren();
			return;
		case EAKDrawHierarchyMode::OnlyParents:
			DrawParents();
			return;
		case EAKDrawHierarchyMode::OnlyChildren:
			DrawChildren();
			return;
		default:
			checkNoEntry()
	}
}
#endif
