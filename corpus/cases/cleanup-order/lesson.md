# Destroy dependent resources in the right order

The broken fixture records `parent=destroyed` while a child resource is still
owned. In real code this often becomes a use-after-free in a destructor, lost
cleanup context, or a final log/error call after its environment is gone.

The fixed version should release dependent child resources first, then destroy
the parent/container state.

Canonical anchors: CERT MEM12-C, lifetime/ownership cleanup guidance.
