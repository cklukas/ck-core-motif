---
nav_exclude: true
---
# XmString malformed-state handling

The public component reader no longer uses assertions for malformed text types,
unknown states, or unreachable segment transitions. It marks the context
failed and returns `XmSTRING_COMPONENT_END`, allowing applications to handle
invalid compound strings without process termination.

The regression check is
`tests/automated/check-xmstring-component-assertions.py`.
