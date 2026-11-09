#!/usr/bin/env python3
"""Check WML syntax construction validates all raw object allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlsynbld.c").read_text(
    encoding="utf-8"
)

alloc_start = source.index("static void *\nwmlAllocateSyntaxMemory")
resize_start = source.index("static void *\nwmlResizeSyntaxMemory", alloc_start)
resize_end = source.index("/*\n * Routine to create a class descriptor", resize_start)
allocator = source[alloc_start:resize_end]
assert "if (size == 0)" in allocator
assert "result = malloc (size);" in allocator
assert "if (result == NULL)" in allocator
assert "item_size == 0 || count == 0" in allocator
assert "result = realloc (old_memory, count * item_size);" in allocator
assert allocator.count("exit (EXIT_FAILURE);") == 4

body = source[resize_end:]
assert " malloc (" not in body
assert " realloc (" not in body
assert body.count("wmlAllocateSyntaxMemory (") == 16
assert body.count("wmlResizeSyntaxMemory (") == 3

print("PASS WML syntax builder checks all object and alias allocations")
