# Workspace-info serialization guards

`SetWorkspaceInfo` now rejects a missing data buffer for nonzero record counts
and checks the record-size multiplication before publishing the property.
Zero-record updates remain valid and publish an empty value.
