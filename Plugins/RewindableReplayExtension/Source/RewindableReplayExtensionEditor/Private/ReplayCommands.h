#pragma once

class FReplayCommands : public TCommands<FReplayCommands>
{
public:
	FReplayCommands()
		: TCommands<FReplayCommands>(
		TEXT("RewindableReplayExtension"),
		NSLOCTEXT("Contents", "RewindableReplayEditor", "Rewindable Replay Editor"),
		NAME_None,
		FCoreStyle::Get().GetStyleSetName())
	{}

	void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> TestCommand;
};