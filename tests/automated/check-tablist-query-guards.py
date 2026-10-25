#!/usr/bin/env python3
"""Check TabList query APIs validate output and invalid-list inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
query_start = source.index("XmTabbedStackListQuery(XmTabbedStackList tab_list")
query_end = source.index("\n}\n", query_start) + 2
query = source[query_start:query_end]
assert "attributes == NULL" in query
assert "tab_list->tabs == NULL" in query
assert query.index("attributes == NULL") < query.index("attributes->")

simple_start = source.index("XmTabbedStackListSimpleQuery(XmTabbedStackList tab_list")
simple_end = source.index("\n}\n", simple_start) + 2
simple = source[simple_start:simple_end]
assert "if (tab_list == NULL || tab_list->tabs == NULL" in simple
assert "return NULL;" in simple
assert simple.index("return NULL;") < simple.index("XmTabbedStackListQuery")
print("PASS TabList query guards")
