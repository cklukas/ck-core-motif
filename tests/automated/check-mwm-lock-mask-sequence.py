#!/usr/bin/env python3
"""Check MWM lock-mask sequence construction rejects unsafe sizes."""

from pathlib import Path


source = Path("clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
start = source.index("static void\nBuildLockMaskSequence")
end = source.index("\n\n/******************************<->", start)
body = source[start:end]

assert "unsigned int num_masks;" in body
assert "unsigned int num_bits;" in body
assert "num_bits >= sizeof(unsigned int) * CHAR_BIT" in body
assert "(1U << num_bits) - 1U > UINT_MAX / sizeof(unsigned int) - 1U" in body
assert "num_masks = (1U << num_bits) - 1U;" in body
assert "XtCalloc ((Cardinal)(num_masks + 1U), sizeof (unsigned int))" in body
allocation = body.index("XtCalloc ((Cardinal)(num_masks + 1U)")
assert body.index("if (wmGD.pLockMaskSequence == NULL)", allocation) > allocation
assert "run = (1U << (bit-1));" in body

print("PASS MWM lock-mask sequences reject unsafe width and allocation failure")
