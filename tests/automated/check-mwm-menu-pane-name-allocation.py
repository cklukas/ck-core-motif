#!/usr/bin/env python3
"""Check that MWM menu-pane names fit Xt's allocation width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("static void ParseMenuSet (WmScreenData")
    end = text.index("} /* END OF FUNCTION ParseMenuSet */", start)
    region = text[start:end]
    assert "size_t menuNameLength;" in region
    assert "menuNameLength = strlen((char *)string);" in region
    assert "menuNameLength >= UINT_MAX" in region
    assert "XtMalloc((Cardinal)menuNameLength + 1)" in region
    assert "XtMalloc ((unsigned int) (strlen ((char *)string) + 1))" not in region
    print("MWM menu-pane names are width-checked")


if __name__ == "__main__":
    main()
