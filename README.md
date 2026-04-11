# flashmap

A CLI tool for parsing and visualizing flash memory layouts from linker map files.

## Features

- **Map Parsing** (`mapparser`): Parse GNU ld `.map` files to extract memory region data.
- **Flash Regions** (`flashregion`): Data model for individual flash/RAM memory regions.
- **Flash Layout** (`flashlayout`): Manage collections of regions as a complete memory layout.
- **Visualization** (`flashvisualizer`): Render ASCII bar charts of memory usage.
- **Reporting** (`flashreport`): Generate detailed usage reports per region.
- **Export** (`flashexport`): Export layouts to JSON or CSV formats.
- **Filtering** (`flashfilter`): Filter regions by name, type, or usage threshold.
- **Summary** (`flashsummary`): Aggregate statistics across all regions.
- **Diff** (`flashdiff`): Compare two layouts and highlight changes.
- **Alignment Check** (`flashalign`): Detect address and size alignment issues across regions.

## Building

```bash
make
```

## Running Tests

```bash
make test
```

## Usage

```bash
flashmap <mapfile> [--align <boundary>] [--export json|csv] [--filter <name>]
```

### Alignment Check Example

```bash
flashmap firmware.map --align 256
```

This will report any regions whose start address or size is not a multiple of 256 bytes.

## License

MIT
