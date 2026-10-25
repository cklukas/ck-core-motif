# ComboBox textual trait validation

ComboBox edit-box helpers now check both the access-textual trait and the
specific callback they need. A class with a partial textual trait returns no
value or skips the update safely, while complete traits retain the existing
XmString behavior.

The regression check is
`tests/automated/check-combobox-text-trait.py`.
