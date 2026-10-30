---
nav_exclude: true
---
# FileSelectionBox synthetic getter guards

Directory, pattern, no-match, label, list, count, and directory-mask exports
now validate callback outputs and required children before reading state. Missing
children return null or zero outputs instead of dereferencing partial dialog
state, preserving normal Unicode compound-string exports.
