#!/usr/bin/env python3
"""Check clipboard format atom-name allocation guards."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.rindex("ClipboardGetAtomFromFormat(")
    start = SOURCE.rfind("static Atom", 0, start)
    end = SOURCE.index("ClipboardGetLenFromFormat(", start)
    function = SOURCE[start:end]

    assert "if (format_name == NULL)" in function
    assert "format_name_length = strlen( format_name );" in function
    assert "prefix_length = sizeof(\"_MOTIF_CLIP_FORMAT_%s\") - 1;" in function
    assert "format_name_length > (size_t)-1 - prefix_length - 1" in function
    assert "name_bytes = format_name_length + prefix_length + 1;" in function
    assert "if (name_bytes > UINT_MAX)" in function
    assert "item = XtMalloc((Cardinal)name_bytes);" in function
    assert "if (item == NULL)" in function
    assert function.index("if (item == NULL)") < function.index("snprintf")
    assert function.count("return None;") >= 4


if __name__ == "__main__":
    main()
