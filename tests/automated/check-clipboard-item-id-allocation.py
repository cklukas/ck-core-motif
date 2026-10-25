#!/usr/bin/env python3
"""Check clipboard next-ID and start-copy allocation guards."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    helper_start = SOURCE.index("static itemId\nClipboardGetNewItemId(")
    helper_end = SOURCE.index("static void \nClipboardSetAccess", helper_start)
    helper = SOURCE[helper_start:helper_end]
    assert "status = ClipboardFindItem" in helper
    assert "propertynumber == NULL" in helper
    assert "length < sizeof(itemId)" in helper
    assert "return 0;" in helper

    start = SOURCE.index("XmClipboardStartCopy(")
    end = SOURCE.index("XmClipboardCopy(", start)
    function = SOURCE[start:end]
    assert "if (header == NULL)" in function
    assert "if (loc_itemid == 0)" in function
    assert "dataItemLabelId == 0" in function
    assert "previousStartCopyCalled" in function


if __name__ == "__main__":
    main()
