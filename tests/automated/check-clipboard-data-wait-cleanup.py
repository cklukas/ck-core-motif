#!/usr/bin/env python3
"""Check cut-by-name wait failure cleanup."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static int \nClipboardRequestDataAndWait(")
    end = SOURCE.index("/*ARGSUSED*/\nstatic void\nClipboardTimeout", start)
    function = SOURCE[start:end]

    assert "XtAppAddTimeOut(app_context" in function
    assert "if (wid != (Widget) NULL)" in function
    assert "return 0;" in function[function.index("} else {"):]
    assert function.count("XSelectInput( display") >= 5
    assert "if (! dataisready) {" in function
    assert "rootattributes.your_event_mask" in function


if __name__ == "__main__":
    main()
