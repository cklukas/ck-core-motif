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

setup_start = source.index("static void\nSetupLockingModifierMask")
setup_end = source.index("\n\n/******************************<->", setup_start)
setup = source[setup_start:setup_end]
mapping = setup.index("modifier_map = XGetModifierMapping(dpy);")
free_previous = setup.index("XtFree((char *)wmGD.pLockMaskSequence);")
assert free_previous < mapping
assert "wmGD.pLockMaskSequence = NULL;" in setup[free_previous:mapping]
null_guard = setup.index("if (modifier_map == NULL)", mapping)
first_dereference = setup.index("modifier_map->max_keypermod", mapping)
assert mapping < null_guard < first_dereference
assert "BuildLockMaskSequence();" in setup[null_guard:first_dereference]

handler_start = source.index("static void\nMappingEventHandler")
handler_end = source.index("\n\f\n/******************************<->", handler_start)
handler = source[handler_start:handler_end]
refresh = handler.index("XRefreshKeyboardMapping(&event->xmapping);")
modifier = handler.index("if(event->xmapping.request == MappingModifier)")
assert refresh < modifier

print("PASS MWM lock-mask sequences reject unsafe width and allocation failure")
