#!/usr/bin/env python3
"""Check MWM command-tree construction preserves existing commands on OOM."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)

new_start = source.index("NewCommand (")
new_region = source[new_start:source.index("FindCmd(", new_start)]
assert "XtCalloc(1, sizeof(CmdTree))" in new_region
assert "if (ptr == NULL)" in new_region
assert "if (ptr->name == NULL)" in new_region
assert "if (ptr->defaultName == NULL)" in new_region

add_start = source.index("AddCommand (")
add_region = source[add_start:source.index("RemoveCommandBranch", add_start)]
assert "CmdTree *newCommand;" in add_region
assert add_region.index("newCommand = NewCommand") < add_region.index(
    "FindDuplicateName(CCI_TREE_OF_SCR(scr), name)"
)
assert "DestroyMenuSpec (&(wmGD.Screens[scr]), duplicateID)" in add_region
assert "DeleteCommand(duplicateID, &CCI_TREE_OF_SCR(scr))" in add_region
assert "DeleteCommand(commandID, &CCI_TREE_OF_SCR(scr))" not in add_region

define_start = source.index("DefineCommand (")
define_region = source[define_start:source.index("IncludeCommand", define_start)]
assert "Unable to allocate command tree root." in define_region
assert "Boolean commandSetFound = False;" in define_region
leaf_create = define_region.index("tmp = NewCommand(commandID, selection, name, defaultLabel)")
leaf_duplicate = define_region.index("FindDuplicateName(CCI_TREE(w), name)")
assert leaf_create < leaf_duplicate
assert "if (commandSetFound)" in define_region

print("PASS MWM command-tree allocation failures preserve existing entries")
