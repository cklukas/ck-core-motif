#!/usr/bin/env python3
"""Check that picture state construction is transactional and size-safe."""

from pathlib import Path

source = Path("lib/Xm/Picture.c").read_text()
start = source.index("XmGetNewPictureState(XmPicture picture)")
end = source.index("\n/*", start)
region = source[start:end]

assert "if (picture == NULL || picture->num_nodes < 0)" in region
assert "state_bytes = 1U + ((size_t)picture->num_nodes / 8U);" in region
assert "if (state_bytes > (size_t)INT_MAX)" in region
assert "if (state == NULL)" in region
assert "if (state->state == NULL)" in region
assert "if (state->newstate == NULL)" in region
assert "if (state->current_string == NULL)" in region

assert region.index("if (state->state == NULL)") < region.index("state->newstate =")
assert region.index("if (state->newstate == NULL)") < region.index("state->current_capacity =")
assert region.index("if (state->current_string == NULL)") < region.index("_XmPictureSetState")

print("Picture state allocation guards present")
