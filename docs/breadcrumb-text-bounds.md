---
nav_exclude: true
---

# Breadcrumb text-length bounds

`XmBreadcrumbBar` now bounds measured label and separator widths before adding
padding, and avoids overflowing the final text position when the cursor reaches
its limit. Normal breadcrumb placement is unchanged.

The focused regression check is
`tests/automated/check-breadcrumb-text-bounds.py`.
