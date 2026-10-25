#!/usr/bin/env python3
"""Check that clipboard callback table growth is transactional."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardStartCopy(")
    end = SOURCE.index("XmClipboardCopy(", start)
    function = SOURCE[start:end]

    growth = function.index("/* Allocate replacement tables before changing the live pair. */")
    tail = function[growth:]
    assert "newCbProcTable = (XmCutPasteProc *) XtMalloc" in tail
    assert "if (newCbProcTable == NULL)" in tail
    assert "newCbIdTable = (long *) XtMalloc" in tail
    assert "if (newCbIdTable == NULL)" in tail
    assert "memcpy(newCbProcTable, cbProcTable" in tail
    assert "memcpy(newCbIdTable, cbIdTable" in tail
    assert "cbProcTable = newCbProcTable;" in tail
    assert "cbIdTable = newCbIdTable;" in tail
    assert tail.count("header->selectionTimestamp = previousTimestamp;") == 2
    assert tail.count("return ClipboardFail;") >= 2


if __name__ == "__main__":
    main()
