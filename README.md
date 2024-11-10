
# Multi-Level Page Table Simulation

This project simulates multi-level page table lookup and allocation. It includes a configurable layout via `config.h`, and supports page table lookups with various levels of page tables.

## Customizing config.h

You can adjust the following constants in `config.h`:

- `LEVELS`: Number of levels in the page table (1-6).
- `POBITS`: Number of bits used for the page offset (4-18).

For example, setting `LEVELS` to 2 and `POBITS` to 12 means that each page table will have 512 entries and two levels of lookup.

## Building and Using the Library

To build the library, run:

```
make
```

This will create `libmlpt.a` which provides the page table simulation API.

To clean the build, run:

```
make clean
```

## Known Bugs

None currently identified.

## Future Expansion

- Support for memory deallocation.
- Optimization for larger page tables.
