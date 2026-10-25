#!/usr/bin/env python3
"""Check that picture state construction is transactional and size-safe."""

from pathlib import Path

source = Path("lib/Xm/Picture.c").read_text()
parse_start = source.index("XmParsePicture(char *input)")
parse_end = source.index("\n/*", parse_start)
parse_region = source[parse_start:parse_end]
assert "if (picture == NULL)" in parse_region
assert "if (picture->source == NULL)" in parse_region
assert "if (picture->nodes == NULL)" in parse_region
assert parse_region.index("if (picture->source == NULL)") < parse_region.index("picture->num_nodes")
assert parse_region.index("if (picture->nodes == NULL)") < parse_region.index("_XmPictureParseNode")

public_region = source[source.index("XmPictureProcessCharacter("):]
assert "if (state == NULL || is_finished == NULL)" in public_region
assert "if (p == NULL)" in public_region
assert "if (s == NULL)" in public_region
assert "if (state == NULL)" in public_region
autofill_start = source.index("XmPictureDoAutoFill(XmPictureState state)")
autofill_end = source.index("\n}\n", autofill_start) + 2
autofill_region = source[autofill_start:autofill_end]
assert "processed = XmPictureProcessCharacter" in autofill_region
assert "if (processed == NULL && !finished)" in autofill_region
assert autofill_region.index("if (processed == NULL && !finished)") < autofill_region.index("if(finished)")

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
