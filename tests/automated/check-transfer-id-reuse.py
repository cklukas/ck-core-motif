#!/usr/bin/env python3
"""Check transfer contexts are initialized before publication and reuse."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("GetTransferID(void)")
end = source.index("static void\nFreeTransferID", start)
body = source[start:end]
init = body.index("rval -> outstanding = 0")
publish = body.index("global_tc = rval")
assert init < publish
assert "rval -> doneProcs = NULL" in body
assert "rval -> numDoneProcs = 0" in body
for token in ("rval -> widget = NULL", "rval -> selection = None",
              "rval -> callback_struct = NULL", "rval -> drag_context = NULL",
              "rval -> drop_context = NULL"):
    assert token in body, token

start = source.index("FreeTransferID(XtPointer id)")
end = source.index("static void\nCallDoneProcs", start)
body = source[start:end]
assert "tid -> doneProcs = NULL" in body
assert "tid -> numDoneProcs = 0" in body

print("Transfer contexts initialize safely before publication and reuse")
