#!/usr/bin/env python3
"""Check Text input edge calculations widen before subtracting margins."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text()


def function_body(name: str, declaration: str) -> str:
    start = source.index(f"static {declaration}\n{name}")
    end = source.index("\nstatic ", start + 1)
    return source[start:end]


scrolling = function_body("CheckTimerScrolling", "Boolean ")
focus = function_body("DoGrabFocus", "void ")

for body in (scrolling, focus):
    assert "(int64_t)tw->core.width -" in body
    assert "(int64_t)o_data->rightmargin" in body
    assert "tw->core.width - o_data->rightmargin" not in body
    assert "tw->core.height - o_data->bottommargin" not in body

assert "ClampTextPointCoordinate(text_right" in focus
assert "ClampTextPointCoordinate(text_bottom" in focus
assert "ClampTextPointCoordinate(rtl_text_left" in focus
print("PASS Text input edge geometry widens before margin subtraction")
