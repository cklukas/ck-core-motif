#!/usr/bin/env python3
"""Check XmClipboardEndCopy handles missing clipboard headers."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("int \nXmClipboardEndCopy(")
    end = SOURCE.index("int \nXmClipboardCancelCopy(", start)
    function = SOURCE[start:end]

    assert "header = ClipboardOpen( display, sizeof( itemId ) );" in function
    assert "if (header == NULL)" in function
    assert function.index("if (header == NULL)") < function.index(
        "if (!header->startCopyCalled)"
    )
    assert "return ClipboardFail;" in function[function.index("if (header == NULL)"):]


if __name__ == "__main__":
    main()
