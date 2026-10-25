#!/usr/bin/env python3
"""Check workspace-list parsing validates its initial pointer array."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmResource.c"
text = source.read_text(encoding="utf-8")

needle = "ppchWsNames = (char **) XtMalloc (wsNamesAlloced * sizeof (char *));"
assert needle in text
guard = text[text.index(needle):text.index(needle) + 300]
assert "if (ppchWsNames == NULL)" in guard
assert "ExitWM (WM_ERROR_EXIT_VALUE);" in guard
print("workspace-list parsing checks its initial allocation")
