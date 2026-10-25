#!/usr/bin/env python3
"""Check that the clipboard next-item property update validates retrieval."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static void \nClipboardSetNextItemId(")
    end = SOURCE.index("WeOwnSelection(", start)
    function = SOURCE[start:end]

    assert "int status;" in function
    assert "int_ptr = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "int_ptr == NULL" in function
    assert "length < sizeof(long)" in function
    assert "XtFree( (char *)int_ptr);" in function
    assert function.index("if (status != ClipboardSuccess") < function.index("*(long *) int_ptr")


if __name__ == "__main__":
    main()
