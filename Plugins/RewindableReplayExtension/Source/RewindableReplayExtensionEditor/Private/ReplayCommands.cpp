#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "ReplayCommands.h"

#define LOCTEXT_NAMESPACE "ReplayTools"

void FReplayCommands::RegisterCommands()
{
	UI_COMMAND(ReplayCommand, "Record", "Activates Replay Tool", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE