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

	FMeshBoneInfo Child = FMeshBoneInfo();
	Child.Name = InChild;
	const int ParentIdx = RefSkel.GetRefBoneInfo().Find(Child);

	if (ParentIdx == INDEX_NONE)
	{
		return NAME_None;
	}

	return RefSkel.GetRefBoneInfo()[ParentIdx].Name;
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
