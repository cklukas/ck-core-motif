# Render-table property-name bounds

Render-table property conversion uses a fixed table for the supported property
names.  The converter now checks the property index before indexing that table;
oversized input follows the existing error cleanup path instead of reading past
the array.
