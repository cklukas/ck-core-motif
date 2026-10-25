---
nav_exclude: true
---
# LabelG secondary-object allocation

LabelG creates secondary and request extension objects before fetching cache
resources. The setup now checks both objects and the extension-data record,
releasing partial allocations before returning. Cache pointers are only updated
after all required storage exists.
