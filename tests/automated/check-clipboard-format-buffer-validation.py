#!/usr/bin/env python3
"""Check that XmClipboardInquireFormat validates its output buffer."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardInquireFormat(")
    end = SOURCE.index("_XmDisplayToAppContext(display);", start)
    body = SOURCE[start:end]
    assert "buffer == NULL && bufferlength != 0" in body
    assert "return ClipboardBadFormat;" in body


if __name__ == "__main__":
    main()
