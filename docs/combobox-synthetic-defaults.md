---
nav_exclude: true
---
# ComboBox synthetic accessor defaults

Before child creation, selected-item export now returns a null `XmString` and
selected-position export returns `XmINVALID_POSITION`. Selected-position
import also requires the list child. These deterministic defaults avoid
leaving caller output uninitialized during resource setup.
