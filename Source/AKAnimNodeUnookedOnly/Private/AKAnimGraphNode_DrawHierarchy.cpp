// Copyright 2026 Aaron Kemner, All Rights reserved.


#include "AKAnimGraphNode_DrawHierarchy.h"

#include "DetailLayoutBuilder.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet2/CompilerResultsLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AKAnimGraphNode_DrawHierarchy)

void UAKAnimGraphNode_DrawHierarchy::Draw(FPrimitiveDrawInterface* PDI,
                                          USkeletalMeshComponent* PreviewSkelMeshComp) const
{
	Super::Draw(PDI, PreviewSkelMeshComp);

	FAKAnimNode_DrawHierarchy* PreviewNode = GetActiveInstanceNode<FAKAnimNode_DrawHierarchy>(
		PreviewSkelMeshComp->GetAnimInstance());
	
	
	if (PreviewNode)
	{
		PreviewNode->bRequestDraw = true;
	}
}

void UAKAnimGraphNode_DrawHierarchy::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton,
	FCompilerResultsLog& MessageLog)
{
	if (ForSkeleton 
		&& ForSkeleton->GetReferenceSkeleton().FindBoneIndex(Node.StartBone.BoneName) == INDEX_NONE
		&& Node.Mode != EAKDrawHierarchyMode::WholeHierarchy)
	{
		MessageLog.Error(TEXT("@@ - You must pick a bone to observe"), this);
	}

	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

void UAKAnimGraphNode_DrawHierarchy::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName,
	int32 ArrayIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);
	
	if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_SkeletalControlBase, Alpha))
	{
		Pin->bHidden = true;
	}
}

void UAKAnimGraphNode_DrawHierarchy::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	Super::CustomizeDetails(DetailBuilder);
	
	TSharedRef<IPropertyHandle> NodeHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UAKAnimGraphNode_DrawHierarchy, Node), GetClass());
	DetailBuilder.HideProperty(NodeHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAnimNode_SkeletalControlBase, Alpha)));
	DetailBuilder.HideProperty(NodeHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAnimNode_SkeletalControlBase, AlphaScaleBias)));
}

void UAKAnimGraphNode_DrawHierarchy::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
	Super::CopyNodeDataToPreviewNode(InPreviewNode);
	
	FAKAnimNode_DrawHierarchy* Preview = static_cast<FAKAnimNode_DrawHierarchy*>(InPreviewNode);
	FBoneReference OriginalStartBone = Preview->StartBone; // Only piece of mutable data we need to keep on that node
	memcpy(InPreviewNode, &Node, sizeof(Node));
	Preview->StartBone = OriginalStartBone;
}
