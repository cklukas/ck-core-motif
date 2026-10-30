#!/usr/bin/env python3
"""Check PushButtonGadget label-area validation precedes field access."""

from pathlib import Path


source = Path("lib/Xm/PushBG.c").read_text(encoding="utf-8")
start = source.rindex("ComputePBLabelArea(")
end = source.index("\n}", start)
body = source[start:end]
guard = body.index("if (pb == NULL || box == NULL)")
assert guard < body.index("PBG_ArmColor(pb)")
assert "return False;" in body[guard:]

print("PushButtonGadget label-area guard precedes field access")
