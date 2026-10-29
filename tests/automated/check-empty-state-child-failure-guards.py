#!/usr/bin/env python3
"""Check EmptyState internal child failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EmptyState.c").read_text()
assert "if (child == NULL)" in source
assert "if (state->empty_state.action_widget != NULL)" in source
assert "if (state->empty_state.message_widget != NULL)" in source
assert "if (state->empty_state.icon_widget != NULL)" in source
print("PASS EmptyState child-creation failure paths are null-safe")
