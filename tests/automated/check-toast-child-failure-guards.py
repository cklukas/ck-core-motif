#!/usr/bin/env python3
"""Check Toast internal child failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toast.c").read_text()
assert "if (child == NULL)" in source
assert "if (toast->toast.action_widget != NULL)" in source
assert "if (toast->toast.message_widget != NULL)" in source
assert "XtAddCallback(toast->toast.action_widget" in source
print("PASS Toast child-creation failure paths are null-safe")
