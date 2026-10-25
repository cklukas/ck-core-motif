# Xm widget utility null-safety

The private widget utility helpers now return safely when given a null widget:
`_XmGadgetWarning`, `_XmGetFocus`, `_XmSetValuesOnChildren`, and
`_XmUtilIsSubclassByNameQ`. Valid widget behavior and ABI remain unchanged.
This protects teardown and partially initialized widget paths from entering Xt
macros or class-chain dereferences with invalid state.
