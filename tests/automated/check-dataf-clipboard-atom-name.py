#!/usr/bin/env python3
"""Check that DataF handles failed clipboard atom-name lookup."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/DataF.c").read_text()


def main() -> None:
    start = SOURCE.index("atom_name = XGetAtomName")
    end = SOURCE.index("/* move the data to the clipboard */", start)
    body = SOURCE[start:end]
    assert "if (atom_name == NULL)" in body
    assert "XmClipboardCancelCopy(display, window, item_id);" in body
    assert "XtFree((char *)tmp_prop.value);" in body
    assert "return False;" in body


if __name__ == "__main__":
    main()
