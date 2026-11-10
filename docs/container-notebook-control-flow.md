---
nav_exclude: true
---
# Container and Notebook control-flow clarity

Container placement, icon-header access, and traversal now make the scope of
their guarded statements explicit.  Notebook tab selection likewise brackets
the active major and minor tab switches.  These changes preserve established
layout and traversal behavior while ensuring that maintenance edits cannot
silently move coordinate updates, constraint reads, or page-loop advancement
into the wrong conditional branch.

The focused regression check verifies the relevant source scopes.  The native
library compilation is also run with `-Werror=misleading-indentation` for the
two affected translation units.

The adjacent Container paths also avoid retaining locals whose values are
never observed.  The geometry-manager request is still issued and explicitly
discarded, while managed-child and traversal code no longer suggests state that
does not exist.  A focused check and native compilation with unused-local
diagnostics promoted to errors guard that contract.
