#!/usr/bin/env python3
"""Check finalized clipboard data-item validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("int \nXmClipboardEndCopy(")
    end = SOURCE.index("int \nXmClipboardCancelCopy(", start)
    function = SOURCE[start:end]

    assert "itemheader = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "itemlength < sizeof(ClipboardDataItemRec)" in function
    assert "XtFree( (char *)itemheader);" in function
    assert function.index("itemheader->cutByNameWindow") > function.index(
        "itemlength < sizeof(ClipboardDataItemRec)"
    )


if __name__ == "__main__":
    main()
