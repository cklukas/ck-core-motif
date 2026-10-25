#!/usr/bin/env python3
"""Check WSM command window-list allocations fit Xt's Cardinal width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    assert text.count("(size_t) count > UINT_MAX / sizeof(Window)") == 5
    assert text.count("XtMalloc((Cardinal)(sizeof(Window) * (size_t) count))") == 5
    assert "XtMalloc(sizeof(Window) * (size_t) count)" not in text
    print("MWM command window arrays are Cardinal-width checked")


if __name__ == "__main__":
    main()
