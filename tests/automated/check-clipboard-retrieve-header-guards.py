#!/usr/bin/env python3
"""Check retrieval transaction header guards."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def check_function(name: str, end: str) -> str:
    start = SOURCE.index(name)
    return SOURCE[start : SOURCE.index(end, start)]


def main() -> None:
    start = check_function("XmClipboardStartRetrieve(", "XmClipboardEndRetrieve(")
    end = check_function("XmClipboardEndRetrieve(", "XmClipboardRetrieve(")
    retrieve = check_function("ClipboardRetrieve(Display", "XmClipboardInquireCount(")

    assert "if (header == NULL)" in start
    assert "ClipboardUnlock( display, window, 0 );" in start
    assert "if (header == NULL)" in end
    assert "return ClipboardFail;" in end
    assert "if (header == NULL)" in retrieve
    assert "ClipboardUnlock( display, window, 0 );" in retrieve
    assert retrieve.index("timestamp = header->copyFromTimestamp") > retrieve.index(
        "if (header == NULL)"
    )


if __name__ == "__main__":
    main()
