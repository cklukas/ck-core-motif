---
nav_exclude: true
---

# WSM window-data alignment

WSM window data carries a bitmask followed by variable-length attribute
values.  If storage for the decoded records or an attribute list fails, the
decoder now consumes the encoded values before returning.  This keeps later
records aligned in the message stream while reporting the failed data list as
empty, avoiding a cascading decode of unrelated bytes.
