// Copyright 2026 Aaron Kemner, All Rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AKAnimNode_DrawHierarchy.h"
#include "Editor/AnimGraph/Public/AnimGraphNode_SkeletalControlBase.h"
#include "AKAnimGraphNode_DrawHierarchy.generated.h"

#define PLUGIN_API AKANIMNODEUNOOKEDONLY_API

UCLASS()
class PLUGIN_API UAKAnimGraphNode_DrawHierarchy : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAKAnimNode_DrawHierarchy Node;
	
	// Begin UAnimGraphNode_Base Interface
	virtual void Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* PreviewSkelMeshComp) const override;
	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	virtual void CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode) override;
	// End UAnimGraphNode_Base Interface
protected:
	// Begin UAnimGraphNode_SkeletalControlBase Interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End UAnimGraphNode_SkeletalControlBase Interface
};

#undef PLUGIN_API
