#!/usr/bin/env python3
"""Check null-client handling for MWM initial-property helpers."""

from pathlib import Path


SOURCE = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")


def function_body(name: str) -> str:
    marker = f"{name} (ClientData *pCD)"
    start = SOURCE.index(marker)
    brace = SOURCE.index("{", start)
    depth = 0
    for index in range(brace, len(SOURCE)):
        if SOURCE[index] == "{":
            depth += 1
        elif SOURCE[index] == "}":
            depth -= 1
            if depth == 0:
                return SOURCE[brace : index + 1]
    raise AssertionError(f"unterminated {name}")


for helper in ("GetInitialPropertyList", "DiscardInitialPropertyList"):
    body = function_body(helper)
    assert "if (pCD == NULL)" in body, f"{helper} lacks a null guard"
    assert "return;" in body, f"{helper} does not return safely"

has_property = SOURCE[SOURCE.index("HasProperty (") :]
assert "if (pCD == NULL)" in has_property, "HasProperty lacks a null guard"
assert "return (False);" in has_property, "HasProperty null path is not false"

print("PASS MWM initial-property helpers reject null client records")
