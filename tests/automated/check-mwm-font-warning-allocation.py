#!/usr/bin/env python3
"""Check that MWM font-load diagnostics guard formatter widths."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("void \nMakeAppearanceResources")
    end = text.index("#ifndef NO_MULTIBYTE", start)
    region = text[start:end]
    assert "(unsigned int)messageLength < UINT_MAX" in region
    assert "XtMalloc((Cardinal)messageLength + 1)" in region
    assert "XtMalloc((unsigned int)messageLength + 1)" not in region
    print("mwm font-load diagnostics are width-checked")


if __name__ == "__main__":
    main()
