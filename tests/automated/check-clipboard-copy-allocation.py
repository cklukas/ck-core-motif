#!/usr/bin/env python3
"""Check that XmClipboardCopy handles allocation and retrieval failures."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardCopy(")
    end = SOURCE.index("XmClipboardEndCopy(", start)
    copy = SOURCE[start:end]

    assert "formatptr     = (ClipboardFormatItem)XtMalloc" in copy
    assert "if (formatptr == NULL)" in copy
    assert "formatdataptr = XtMalloc" in copy
    assert "if (formatdataptr == NULL && formatdatalength != 0)" in copy
    assert "status = ClipboardRetrieveItem(display," in copy
    assert "formatdataptr = NULL;" in copy
    assert "formatdatalength < length" in copy
    assert "return (status == ClipboardSuccess) ? ClipboardFail : status;" in copy


if __name__ == "__main__":
    main()
