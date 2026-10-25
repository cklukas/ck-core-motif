#!/usr/bin/env python3
"""Check that MWM catalog strings use one width-checked allocator."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("static char *\nAllocateNlsString")
    end = text.index("} /* InitNlsStrings", start)
    region = text[start:end]
    assert "if (value == NULL)" in region
    assert "if (length >= UINT_MAX)" in region
    assert "XtMalloc((Cardinal)length + 1)" in region
    assert region.count("AllocateNlsString(tmpString)") == 6
    assert "XtMalloc ((unsigned int) (strlen(tmpString) + 1))" not in region
    print("MWM catalog strings use width-checked allocation")


if __name__ == "__main__":
    main()
