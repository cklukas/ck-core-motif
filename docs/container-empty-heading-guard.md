---
nav_exclude: true
---

# Container empty-heading guard

Container detail headings can be absent while resources are rebuilt, and an
application may intentionally provide an empty heading list.  Icon-header
creation now leaves the header absent in that state rather than asserting.  If
allocation fails during a valid update, the follow-on management and layout
steps likewise skip the missing header instead of asserting.

Valid heading arrays still create and update the same icon header.  The public
Container resources and ABI are unchanged.  The static regression check is
`tests/automated/check-container-empty-heading.py`.
