#!/usr/bin/env python3
"""Check that clipboard inquiries fail closed on null counted results."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    assert SOURCE.count("alloc_to_free == NULL") >= 3
    assert "loc_count_len != 0" in SOURCE
    assert "loc_matchlength != 0" in SOURCE
    assert "loc_length != 0" in SOURCE
    assert SOURCE.count("return ClipboardNoData;") >= 6


if __name__ == "__main__":
    main()
