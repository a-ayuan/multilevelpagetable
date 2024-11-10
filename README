
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

## Big-O Analysis:
The time complexity for translation is O(1) for LEVELS=1.
Space complexity depends on the number of allocated pages.

## Deallocation Interface:
This page table implementation supports deallocation of pages via the following function:

```
void page_deallocate(size_t va);
```

This function will deallocate the page corresponding to the virtual address va. It removes the mapping from the page tables and frees the associated physical memory. Additionally, it can clean up any page tables that become unnecessary due to the deallocation.

Feasibility Without Modifying Existing Interfaces or Internals:
- It is possible to implement page_deallocate without changing the existing page_allocate, translate, or their internal implementations. The existing code uses multi-level page tables to manage memory mappings, with each level being an array of entries that either point to the next-level page table or to the physical memory (data page).

Why we can implement page_deallocate without modifications:
- Data Structures Remain Unchanged: The existing page tables and their structures are sufficient to support deallocation. The page tables are hierarchical, and each entry contains enough information (validity bit and pointer) to navigate and modify the mappings.
- Separate Functionality: page_deallocate can be written as a separate function that mirrors the traversal logic of translate and page_allocate. It will navigate through the page tables to the target entry, remove the mapping, and free the associated memory without interfering with the existing functions.
- No Impact on page_allocate and translate: Since page_deallocate operates independently, it doesn't require any changes to the page_allocate or translate functions. It doesn't modify any shared internal mechanisms or assumptions used by these functions.
