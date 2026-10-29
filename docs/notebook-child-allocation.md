---
nav_exclude: true
---
# Notebook scroller allocation

Notebook creates tab-scroller arrows and a default page-scroller pair as
internal children. Creation can fail under resource pressure or a custom Xt
class configuration. The notebook now checks each child before trait callbacks,
manage/unmanage operations, and layout use. A missing page-scroller child
removes the partial scroller and disables default-scroller updates; tabs and
pages remain usable without navigation arrows.

The contract is protected by
`tests/automated/check-notebook-child-allocation.py`.
