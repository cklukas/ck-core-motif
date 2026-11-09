---
nav_exclude: true
---
# Sidebar title replacement

`XmSidebar` owns a copy of its `XmNlabelString` title.  On an `XtSetValues`
title change, the Sidebar now creates the replacement XmString before releasing
the existing title.  If allocation fails, it restores the old pointer and
leaves the existing header label and layout unchanged.

This keeps a Sidebar usable under allocation pressure without borrowing a
caller-owned XmString or clearing its visible title.
