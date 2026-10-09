// Copyright 2026 Aaron Kemner, All Rights reserved.


#include "AKReferenceSkeletonLibrary.h"

#include "Algo/Compare.h"
#include "Algo/Copy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AKReferenceSkeletonLibrary)

FName UAKReferenceSkeletonLibrary::GetParentBone(const FBlueprintReferenceSkeleton& InReferenceSkeleton,
                                                 const FName InChild)
{
	if (!InReferenceSkeleton.IsValid() || InChild.IsNone())
	{
		return NAME_None;
	}

	const FReferenceSkeleton& RefSkel = InReferenceSkeleton.Get();

	const int32 ChildIdx = RefSkel.FindBoneIndex(InChild);
	if (ChildIdx == INDEX_NONE)
	{
		return NAME_None;
	}

	const int32 ParentIdx = RefSkel.GetParentIndex(ChildIdx);
	if (ParentIdx == INDEX_NONE)
	{
		return NAME_None;
	}
	
	return RefSkel.GetBoneName(ParentIdx);
}

void UAKReferenceSkeletonLibrary::GetChildBones(const FBlueprintReferenceSkeleton& InReferenceSkeleton,
                                                const FName InParent, TArray<FName>& OutChildren)
{
	OutChildren.Empty();
	if (!InReferenceSkeleton.IsValid())
	{
		return;
	}

	const FReferenceSkeleton& RefSkel = InReferenceSkeleton.Get();
	const int32 ParentIdx = RefSkel.FindBoneIndex(InParent);
	if (ParentIdx == INDEX_NONE)
	{
		return;
	}
	TArray<int32> ChildIndices;
	InReferenceSkeleton.Get().GetDirectChildBones(ParentIdx, ChildIndices);

	Algo::Transform(ChildIndices, OutChildren, [&RefSkel](const int32 Elem) -> FName
	{
		return RefSkel.GetBoneName(Elem);
	});
}

int UAKReferenceSkeletonLibrary::GetNumBones(const FBlueprintReferenceSkeleton& InReferenceSkeleton)
{
	if (!InReferenceSkeleton.IsValid())
	{
		return 0;
	}

	return InReferenceSkeleton.Get().GetRawBoneNum();
}

void UAKReferenceSkeletonLibrary::GetBones(const FBlueprintReferenceSkeleton& InReferenceSkeleton,
                                           TArray<FName>& OutBones)
{
	if (!InReferenceSkeleton.IsValid())
	{
		OutBones.Empty();
		return;
	}

	const FReferenceSkeleton& RefSkel = InReferenceSkeleton.Get();
	OutBones = RefSkel.GetRawRefBoneNames();
}

void UAKReferenceSkeletonLibrary::DiffHierarchies(const FBlueprintReferenceSkeleton& InSourceReferenceSkeleton,
                                                  const FBlueprintReferenceSkeleton& InTargetReferenceSkeleton,
                                                  const int32 DiffFlags,
                                                  TArray<FName>& OutDiffResult)
{
	OutDiffResult.Empty();
	if (!(InSourceReferenceSkeleton.IsValid() && InTargetReferenceSkeleton.IsValid()))
	{
		return;
	}

	const FReferenceSkeleton& Source = InSourceReferenceSkeleton.Get();
	const FReferenceSkeleton& Target = InTargetReferenceSkeleton.Get();

	const TArray<FName> SourceBoneNames = Source.GetRawRefBoneNames();
	const TArray<FName> TargetBoneNames = Target.GetRawRefBoneNames();

	if (DiffFlags & static_cast<int32>(EBlueprintBoneReferenceDiffFlags::MissingInSource))
	{
		Algo::CopyIf(TargetBoneNames, OutDiffResult,
		             [&SourceBoneNames](const FName& Elem)-> bool
		             {
			             return !SourceBoneNames.Contains(Elem);
		             });
	}

	if (DiffFlags & static_cast<int32>(EBlueprintBoneReferenceDiffFlags::MissingInTarget))
	{
		Algo::CopyIf(SourceBoneNames, OutDiffResult,
		             [&TargetBoneNames, &OutDiffResult](const FName& Elem)-> bool
		             {
			             return !TargetBoneNames.Contains(Elem)
					             && !OutDiffResult.Contains(Elem);
		             });
	}

	if (DiffFlags & static_cast<int32>(EBlueprintBoneReferenceDiffFlags::DifferentParent))
	{
		TArray<FName> Combined;
		Algo::CopyIf(SourceBoneNames, Combined,
		             [&TargetBoneNames](const FName& Elem)-> bool
		             {
			             return TargetBoneNames.Contains(Elem);
		             });

		for (const FName& Bone : Combined)
		{
			const int32 SourceIdx = Source.GetRawNameToIndexMap()[Bone];
			const int32 TargetIdx = Target.GetRawNameToIndexMap()[Bone];
			const int32 SourceParentIdx = Source.GetParentIndex(SourceIdx);
			const int32 TargetParentIdx = Target.GetParentIndex(TargetIdx);
			const FName SourceParentName = Source.GetBoneName(SourceParentIdx);
			const FName TargetParentName = Target.GetBoneName(TargetParentIdx);
			
			if (SourceParentName != TargetParentName)
			{
				OutDiffResult.AddUnique(Bone);
			}
		}
	}
}
