---
nav_exclude: true
---

# DropTransfer initialization

`XmDropTransfer` initialization now treats its transfer-list allocation and
entry-copy allocation as one transaction. Overflow or allocation failure
leaves the object with an empty transfer list instead of dereferencing a null
list or entering later transfer processing with partial state. Successful
initialization retains the existing transfer ownership and behavior.

The focused regression check is
`tests/automated/check-drop-transfer-initialization.py`.
