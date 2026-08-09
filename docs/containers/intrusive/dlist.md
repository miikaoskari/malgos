# Intrusive Doubly Linked List

Doubly linked lists are useful when a bi-directional traversal is needed.

To help traversal through a linked list malgos provides two helper macros.

The first variant `mlg_dlist_for_each` is a non-safe variant from which you are not able to delete during insertion and the second **safe** variant `mlg_dlist_for_each_safe` that can be used to iterate and delete.
