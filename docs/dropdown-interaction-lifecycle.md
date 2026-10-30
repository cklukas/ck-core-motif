---
nav_exclude: true
---
# DropDown secondary interaction lifecycle

DropDown’s text-modification, value-change, list-selection, shell-event, and
focus-loss callbacks now reject incomplete callback data and missing list/text
children before reading fields or dispatching actions.  This keeps keyboard,
focus, and popup interactions safe after partial construction or destruction.

Normal interactions on a complete DropDown retain their existing traversal,
selection, and popup behavior.
