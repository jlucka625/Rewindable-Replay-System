#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "ReplayStyle.h"
#include "ReplayManager.h"
#include "StandardSlateWidget.h"

#define LOCTEXT_NAMESPACE "ReplayTools"

void SStandardSlateWidget::Construct(const FArguments& InArgs)
{
	//OwnerHUD = InArgs._OwnerHUD;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////If the code below doesn't look like C++ to you it's because it (sort-of) isn't,
	/////Slate makes extensive use of the C++ Prerocessor(macros) and operator overloading,
	/////Epic is trying to make our lives easier, look-up the macro/operator definitions to see why.

	struct Local
	{
		static void ValueChanged(float rate)
		{
			AReplayManager::GetInstance()->Scrub();
		}
	};

	FQuat2D quatC(-PI/2.0f);
	FQuat2D quatCC(PI/2.0f);
	FSlateRenderTransform transformC(quatC);
	FSlateRenderTransform transformCC(quatCC);
	FVector2D pivot(0.5f, 0.5f);

	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Center)
			[
				SNew(SImage).Image(FReplayStyle::Get()->GetBrush("UIBorder"))
			]
			+ SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Right).AutoHeight()
				[
					SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
					.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->Stop(); return FReply::Handled(); })
					[
						SNew(SImage).Image(FReplayStyle::Get()->GetBrush("ExitButton"))
					]
				]
				+ SVerticalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->Begin(); return FReply::Handled(); })
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("BeginButton"))
						]
					]
					/*+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("PauseButton"))
						]
					]*/
					+SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(60.0f, 5.0f, 60.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->TogglePlayback(-3.0f); return FReply::Handled(); })
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("RewindButton"))
							.RenderTransformPivot(pivot)
							.RenderTransform(transformCC)
						]
					]
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(60.0f, 0.0f, 60.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->TogglePlayback(1.0f); return FReply::Handled(); })
						[
							SAssignNew(PlayButtonImage, SImage).Image(FReplayStyle::Get()->GetBrush("PlayButton"))
						]
					]
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(60.0f, 5.0f, 60.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->TogglePlayback(3.0f); return FReply::Handled(); })
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("FastForwardButton"))
							.RenderTransformPivot(pivot)
							.RenderTransform(transformCC)
						]
					]
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->End(); return FReply::Handled(); })
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("EndButton"))
						]
					]
				]
				+ SVerticalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Center)
				[
					SNew(SBox).WidthOverride(830.0f).Padding(FMargin(0.0f, 0.0f, 0.0f, 20.0f))
					[
						SAssignNew(Scrubber, SSlider).SliderBarColor(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)))
						.OnValueChanged_Static(&Local::ValueChanged)
					]
				]
			]
		];
}

TSharedPtr<SSlider> SStandardSlateWidget::GetScrubber()
{
	return Scrubber;
}

TSharedPtr<SImage> SStandardSlateWidget::GetPlayButtonImage()
{
	return PlayButtonImage;
}

#undef LOCTEXT_NAMESPACE