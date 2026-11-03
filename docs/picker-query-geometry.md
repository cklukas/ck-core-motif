---
nav_exclude: true
---
# Picker and popup geometry replies

Calendar, DatePicker, TimePicker, and TeachingTip retain their existing
preferred dimensions.  Their geometry query methods now delegate the reply to
`XmeReplyToQueryGeometry`, which evaluates only the width/height fields a
parent actually requested.  This makes partial and query-only Xt negotiations
standards-compliant without changing normal picker layout.
