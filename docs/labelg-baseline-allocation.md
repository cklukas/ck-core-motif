# LabelGadget baseline allocation

`XmLabelGadgetGetBaselines` caches one extra baseline offset and returns a
caller-owned copy. Cache growth now uses a temporary pointer and rejects
missing or unrepresentable baseline data; the returned copy is also checked
before it is filled. Empty baseline sets return success with a null copy.

Allocation failure leaves the gadget cache in a safe state and returns
`False`, preserving the existing class-method contract.
