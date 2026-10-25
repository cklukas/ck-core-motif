---
nav_exclude: true
---
# XmString parse-table allocation failures

The lazy parse table shared by `XmStringGenerate` and `_XmStringUngenerate`
now validates its output pointer and checks the table allocation before
publishing it. Allocation failure returns a zero table size with a null output
table, allowing the caller's normal conversion failure path to run without a
null dereference.
