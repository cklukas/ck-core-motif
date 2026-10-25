#!/usr/bin/env python3
"""Check that MWM menu root creation rolls back allocation failures."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmMenu.c").read_text()


def main() -> None:
    assert "if (menuShellW == NULL)" in SOURCE
    assert "menuSpec->currentContext &= ~CR_MENU_MARK;" in SOURCE
    assert "if (menuW != NULL)" in SOURCE
    assert "if (menuW == NULL)" in SOURCE
    assert "XtDestroyWidget (menuShellW);" in SOURCE


if __name__ == "__main__":
    main()
