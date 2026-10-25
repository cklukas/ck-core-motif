#!/usr/bin/env python3
"""Check that WSM public helpers validate widget inputs before Xt use."""

from pathlib import Path


SEND = (Path(__file__).resolve().parents[2] / "clients/mwm/WmWsmLib/send.c").read_text()
RECV = (Path(__file__).resolve().parents[2] / "clients/mwm/WmWsmLib/recv.c").read_text()


def main() -> None:
    assert "if (w == NULL || request == NULL)" in SEND
    assert "widget and request are required" in SEND
    assert "if (w == NULL)" in RECV
    assert "widget is required" in RECV
    assert SEND.index("if (w == NULL || request == NULL)") < SEND.index("XtScreen(w)")
    assert RECV.index("if (w == NULL)") < RECV.index("XtDisplay(w)")
    assert "if (w == NULL)" in RECV
    assert "if (w == NULL || len_ret == NULL)" in RECV
    target_list = RECV.index("WSMGetTargetList")
    assert RECV.index("if (w == NULL || len_ret == NULL)", target_list) < RECV.index("_WSMGetDispInfo(XtDisplay(w))", target_list)
    assert "output == NULL" in RECV
    assert "(input == NULL && input_len != 0)" in RECV


if __name__ == "__main__":
    main()
