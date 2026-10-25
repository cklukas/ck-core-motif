#!/usr/bin/env python3
"""Check that MWM query-XID buffer growth is overflow-checked."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("wmq_bump_xids ( void )")
    end = text.index("\n}\n\n#endif /* NO_WMQUERY */", start)
    region = text[start:end]
    assert "numXids > INT_MAX - 32" in region
    assert "newCount > SIZE_MAX / sizeof(XID)" in region
    assert "newCount > UINT_MAX / sizeof(XID)" in region
    assert "XtRealloc ((char *)pXids, (Cardinal)(newCount * sizeof(XID)))" in region
    assert "(numXids + 32) * (sizeof (XID))" not in region
    print("MWM query-XID growth is overflow-checked")


if __name__ == "__main__":
    main()
