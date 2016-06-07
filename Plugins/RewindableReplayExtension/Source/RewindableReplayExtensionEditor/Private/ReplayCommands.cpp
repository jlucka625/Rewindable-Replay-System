#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "ReplayCommands.h"

#define LOCTEXT_NAMESPACE "ReplayTools"

void FReplayCommands::RegisterCommands()
{
	UI_COMMAND(TestCommand, "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE