#!/usr/bin/env python3
"""Check PushButton widget and gadget saturate default-frame geometry."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for filename, helper, rectangle in (
    ("PushB.c", "PushButtonBounded", "core"),
    ("PushBG.c", "PushButtonGadgetBounded", "rectangle"),
):
    source = (root / filename).read_text(encoding="utf-8")
    assert f"static Dimension\n{helper}Dimension" in source
    assert f"static Position\n{helper}Position" in source
    assert f"{helper}Dimension(increase, 0)" in source
    assert f"{helper}Dimension(\n\t  (long long)*margins[i] + delta[i], 0)" in source
    assert f"new_w->{rectangle}.width = {helper}Dimension(" in source
    assert f"new_w->{rectangle}.height = {helper}Dimension(" in source

print("PASS PushButton default-frame geometry saturates legacy bounds")
