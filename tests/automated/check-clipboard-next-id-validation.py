#!/usr/bin/env python3
"""Check validation and recovery of the clipboard next-id property."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static ClipboardHeader \nClipboardOpen(")
    end = SOURCE.index("static void \nClipboardClose(", start)
    function = SOURCE[start:end]

    validation = function.index("if (ret_value == ClipboardSuccess &&\n\t(int_ptr == NULL")
    recovery = function.index("if ( ret_value != ClipboardSuccess )", validation)
    assert "length < sizeof(long)" in function[validation:recovery]
    assert "XtFree( (char *)int_ptr);" in function[validation:recovery]
    assert "int_ptr = NULL;" in function[validation:recovery]
    assert "number = XM_FIRST_FREE_ID;" in function[recovery:]


if __name__ == "__main__":
    main()
