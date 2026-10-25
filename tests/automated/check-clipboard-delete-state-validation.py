#!/usr/bin/env python3
"""Check ClipboardIsMarkedForDelete record validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static unsigned long \nClipboardIsMarkedForDelete(")
    end = SOURCE.index("static void \nClipboardDeleteItem(", start)
    function = SOURCE[start:end]

    assert "int status;" in function
    assert "if ( itemid == 0 )" in function
    assert "curritem = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "reclength < sizeof(ClipboardDataItemRec)" in function
    assert "XtFree( (char *)curritem);" in function
    assert function.index("curritem->deletePendingFlag") > function.index(
        "reclength < sizeof(ClipboardDataItemRec)"
    )


if __name__ == "__main__":
    main()
