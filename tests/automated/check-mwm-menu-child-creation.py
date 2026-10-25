#!/usr/bin/env python3
"""Check that MWM separator creation cannot enqueue null children."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmMenu.c").read_text()


def main() -> None:
    assert SOURCE.count("if (children[n] == NULL)") >= 2
    assert "if (children[n - 1] == NULL)" in SOURCE
    assert "XmCreateSeparatorGadget (menuW, SEPARATOR_NAME" in SOURCE


if __name__ == "__main__":
    main()
