#!/usr/bin/env python3
"""Keep DataField picture callback lengths representable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text()
start = source.index("\nPictureVerifyCallback(w, client_d, call_d)")
end = source.index("\n/* USE ITERATIONS OF mblen", start)
body = source[start:end]
assert "changed_length = strlen(changed);" in body
assert "changed_length > (size_t)INT_MAX" in body
assert "cbs->text->length = (int)changed_length;" in body
assert body.index("changed_length > (size_t)INT_MAX") < body.index(
    "cbs->text->length = (int)changed_length"
)
print("DataField picture callback lengths are bounded before narrowing")
