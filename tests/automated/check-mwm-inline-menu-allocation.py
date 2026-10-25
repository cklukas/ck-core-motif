#!/usr/bin/env python3
"""Check that inline MWM menu strings reject unrepresentable lengths."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("static void SearchForOtherMatches")
    end = text.index("static void InsertTreeOnClient", start)
    block = text[start:end]

    assert "(unsigned int)command_length < UINT_MAX" in block
    assert "(unsigned int)funcargs_length < UINT_MAX" in block
    assert "XtMalloc((Cardinal)command_length + 1)" in block
    assert "XtMalloc((Cardinal)funcargs_length + 1)" in block
    assert "XtMalloc((unsigned int)command_length + 1)" not in block
    assert "XtMalloc((unsigned int)funcargs_length + 1)" not in block
    print("MWM inline menu allocations are width-checked")


if __name__ == "__main__":
    main()
