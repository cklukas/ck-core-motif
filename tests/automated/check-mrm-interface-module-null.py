#!/usr/bin/env python3
"""Check Mrm interface-module readers guard null context buffers."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_MRMMODULE_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmmodule.c"))
text = source.read_text(encoding="utf-8")
macro = text.index("#define\t_validate_context")
macro_end = text.index("\n\n", macro)
macro_body = text[macro:macro_end]
if "recptr == NULL" not in macro_body:
    raise SystemExit("shared module context binding lacks a null-record guard")
getter = text.index("UrmIFMGetModule (IDBFile")
getter_end = text.index("\n}", getter)
getter_body = text[getter:getter_end]
if "ifmodptr == NULL" not in getter_body:
    raise SystemExit("direct module getter lacks a null-record guard")
print("Mrm interface-module readers guard null buffers")
