---
nav_exclude: true
---
# ComboBox selected-position export guards

The synthetic `XmNselectedPosition` export now returns before querying the
list child when invoked before ComboBox initialization has created children.
This matches the existing import guard and prevents pre-initialization resource
access from dereferencing incomplete widget state.
