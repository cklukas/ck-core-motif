#!/usr/bin/env python3
"""Check that Mrm decommit-all preserves failed dirty buffers."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("Idb__BM_DecommitAll (")
end = text.index("\n}", start)
body = text[start:end]
failure = body.index("if ( result != MrmSUCCESS )")
first_error = body.index("first_error = result", failure)
next_success = body.index("curbuf->cur_file = NULL", first_error)
failure_block = body[failure:next_success]
if "continue;" not in failure_block:
    raise SystemExit("decommit-all failure does not skip release")
if "curbuf->activity = 0" in failure_block or "curbuf->modified = FALSE" in failure_block:
    raise SystemExit("decommit-all failure still clears dirty state")
if "curbuf->cur_file = NULL" not in body[next_success:]:
    raise SystemExit("successful decommit does not detach the buffer")
print("PASS Mrm decommit-all preserves failed dirty buffers")
