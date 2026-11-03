#!/usr/bin/env python3
"""Check WML resolver allocations cannot be dereferenced after failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlresolve.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("static void *\nwmlResolveAllocate")
helper_end = source.index("void wmlResolveDescriptors", helper_start)
helper = source[helper_start:helper_end]
assert "result = malloc (size);" in helper
assert "if ( result == NULL )" in helper
assert "exit (EXIT_FAILURE);" in helper

resolver = source[helper_end:]
assert "malloc (" not in resolver
assert resolver.count("wmlResolveAllocate (") == 21

print("PASS WML resolver allocations stop cleanly on failure")
