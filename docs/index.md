malgos is an algorithm and data structure library for C language.

The library aims to provide zero allocation data structures that are useful in places with constrained memory e.g. embedded applications.

Currently, malgos ships intrusive data structures that are very similar to those used in Linux. Intrusive data structures use the famous `container_of` macro which is used to get the address of a containing structure from a pointer to one of its members.

Current list of implemented intrusive data structures:
- Hashtable `intrusive/hashtable.c`
- Doubly linked list `intrusive/dlist.c`
