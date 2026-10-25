#!/usr/bin/env python3
"""Check that MWM bitmap-cache path copies fit Xt's allocation width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmImage.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("int GetBitmapIndex")
    end = text.index("\n}", start)
    region = text[start:end]
    assert "size_t pathLength = strlen(path);" in region
    assert "if (pathLength >= UINT_MAX)" in region
    assert "XtMalloc ((Cardinal)pathLength + 1)" in region
    assert "XtMalloc ((unsigned int)(strlen (path) + 1))" not in region
    print("MWM bitmap-cache path allocation is width-checked")


if __name__ == "__main__":
    main()
