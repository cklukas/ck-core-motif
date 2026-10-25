#!/usr/bin/env python3
"""Check ClipboardDataIsReady event and record validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static int \nClipboardDataIsReady(")
    end = SOURCE.index("#if 0", start)
    function = SOURCE[start:end]

    assert "if (event == NULL || private_info == NULL)" in function
    assert "formatitem = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "formatlength < sizeof(ClipboardFormatItemRec)" in function
    assert "XtFree( (char *)formatitem);" in function
    assert function.index("event->type") > function.index("event == NULL")


if __name__ == "__main__":
    main()
