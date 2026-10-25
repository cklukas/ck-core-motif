#!/usr/bin/env python3
"""Check that transient-window list growth propagates allocation failure."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmManage.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("ListTransientSubtree (")
    end = text.index("} /* END OF FUNCTION ListTransientTree", start)
    region = text[start:end]
    assert "static Boolean" in region
    assert "XtRealloc" in region
    assert "if (*ppWins == NULL)" in region
    assert "if (!ListTransientSubtree" in region
    assert "if (!ListTransientSubtree (pCD, &pWins" in region
    assert "XtMalloc((Cardinal)(iSize * sizeof(Window)))" in region
    print("MWM transient list growth propagates allocation failure")


if __name__ == "__main__":
    main()
