# Copied-label destructors

Avatar initials, Divider labels, and IconLabel labels are copied into widget
state during initialization and replacement. Their destructors now release
those owned `XmString` copies, matching the ownership behavior of the other
custom label widgets.
