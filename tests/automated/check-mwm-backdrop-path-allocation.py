#!/usr/bin/env python3
"""Check that WSM backdrop request paths fit Xt's allocation width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmEvent.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("DTWM_REQ_CHANGE_BACKDROP")
    end = text.index("\n\t\t}", start)
    region = text[start:end]
    assert "size_t pathLength = strlen(pch);" in region
    assert "if (pathLength < UINT_MAX)" in region
    assert "XtMalloc((Cardinal)pathLength + 1)" in region
    assert "XtMalloc (1+strlen(pch))" not in region
    print("MWM backdrop request paths are width-checked")


if __name__ == "__main__":
    main()
