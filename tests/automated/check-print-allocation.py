#!/usr/bin/env python3
"""Check that Motif print setup allocations fail cleanly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PrintS.c").read_text(encoding="utf-8")
start = source.index("XtEnum XmPrintToFile")
end = source.index("/*************************************************************\n **  XmPrintPopupPDM", start)
to_file = source[start:end]
assert "callback = (FileCallbackRec *) XtMalloc(sizeof(FileCallbackRec));" in to_file
assert "if (callback == NULL)" in to_file
assert "close(filedes[0]);" in to_file
pdm = source[source.index("pdm_select_data = (PDMSelectData *) XtMalloc"):]
assert "if (pdm_select_data == NULL)" in pdm
assert "return XmPDM_NOTIFY_FAIL;" in pdm
print("PASS print setup allocations are guarded")
