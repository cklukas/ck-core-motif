#!/usr/bin/env python3
"""Check that clipboard copy APIs validate format and data buffers."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    copy = SOURCE.index("XmClipboardCopy(")
    by_name = SOURCE.index("XmClipboardCopyByName(")
    assert "if (format == NULL || (buffer == NULL && length != 0))" in SOURCE[copy:by_name]
    assert "if (buffer == NULL && length != 0)" in SOURCE[by_name:]
    assert SOURCE[copy:].count("return ClipboardBadFormat;") >= 2


if __name__ == "__main__":
    main()
