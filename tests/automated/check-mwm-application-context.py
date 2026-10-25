#!/usr/bin/env python3
"""Check that MWM validates application-context creation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmInitWs.c").read_text()


def main() -> None:
    assert "wmGD.mwmAppContext = XtCreateApplicationContext();" in SOURCE
    assert "if (wmGD.mwmAppContext == NULL)" in SOURCE
    assert 'Warning ("Cannot create the MWM application context")' in SOURCE
    assert "ExitWM (WM_ERROR_EXIT_VALUE);" in SOURCE


if __name__ == "__main__":
    main()
