---
nav_exclude: true
---
# MessageBox synthetic getter guards

Message-string, symbol-pixmap, OK-label, cancel-label, and help-label exports
now validate the widget and output pointers before reading child state. This
keeps Unicode label and dialog-icon resource queries safe during partial
initialization.
