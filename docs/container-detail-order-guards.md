# Container detail-order allocation guards

Container now checks detail-order array allocations both while initializing a
widget and while applying resource changes. A failed copy clears the array and
count, leaving the widget in a valid default-order state instead of
dereferencing a null pointer.
