#!/usr/bin/env python3
"""Check ClipboardDeleteItemLabel record validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static void \nClipboardDeleteItemLabel(")
    end = SOURCE.index("static unsigned long \nClipboardIsMarkedForDelete", start)
    function = SOURCE[start:end]

    assert "int status;" in function
    assert "datalist = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "length < sizeof(ClipboardDataItemRec)" in function
    assert "XtFree( (char *)datalist);" in function
    assert function.index("datalist->dataItemLabelId") > function.index(
        "length < sizeof(ClipboardDataItemRec)"
    )


if __name__ == "__main__":
    main()
