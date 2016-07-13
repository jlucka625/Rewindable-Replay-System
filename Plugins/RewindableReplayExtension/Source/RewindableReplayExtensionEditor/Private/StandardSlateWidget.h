#pragma once
#include "SlateBasics.h"

class SStandardSlateWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SStandardSlateWidget){}

	/*See private declaration of OwnerHUD below.*/
	//SLATE_ARGUMENT(TWeakObjectPtr<class AReplayManager>, ReplayManager)

	SLATE_END_ARGS()

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Needed for every widget
	/////Builds this widget and any of it's children
	void Construct(const FArguments& InArgs);

	TSharedPtr<SSlider> GetScrubber();
	TSharedPtr<SImage> GetPlayButtonImage();

private:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Pointer to our parent HUD
	/////To make sure HUD's lifetime is controlled elsewhere, use "weak" ptr.
	/////HUD has "strong" pointer to Widget,
	/////circular ownership would prevent/break self-destruction of hud/widget (cause memory leak).
	//TWeakObjectPtr<class AStandardHUD> OwnerHUD;
	TSharedPtr<SSlider> Scrubber;
	TSharedPtr<SImage> PlayButtonImage;
};