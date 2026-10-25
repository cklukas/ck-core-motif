#!/usr/bin/env python3
"""Check ClipboardDeleteFormats record and list bounds."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static void \nClipboardDeleteFormats(")
    end = SOURCE.index("static void \nClipboardDeleteItemLabel", start)
    function = SOURCE[start:end]

    assert "datalist = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "datalist->formatCount < 0" in function
    assert "datalist->formatIdList > ULONG_MAX / CONVERT_32_FACTOR" in function
    assert "format_offset" in function
    assert "(length - format_offset) / sizeof(itemId)" in function
    assert "formatdata = NULL;" in function
    assert "length < sizeof(ClipboardFormatItemRec)" in function
    assert function.index("formatdata->cutByNameFlag") > function.index(
        "length < sizeof(ClipboardFormatItemRec)"
    )


if __name__ == "__main__":
    main()
