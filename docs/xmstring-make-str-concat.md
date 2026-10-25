# XmString context slice concatenation

`MakeStr` now checks every component allocation and concatenation while
building a context slice.  Any failure frees the accumulated slice and
returns null instead of silently returning partial text.
