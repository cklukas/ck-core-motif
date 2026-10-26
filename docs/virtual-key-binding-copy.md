---
nav_exclude: true
---
# Virtual-key binding-file assembly

The virtual-key binding loader measures each line and grows its destination
before copying. It now writes each line at the tracked byte offset and updates
the terminator directly, avoiding repeated `strcat` scans for long binding
files while retaining the original concatenated text.

The source contract is checked by:

```sh
python3 tests/automated/check-virtual-key-binding-copy.py
```
