# flashmap

A CLI tool for parsing and visualizing flash memory layouts from linker map files.

---

## Installation

```bash
make
sudo make install
```

## Usage

Pass a linker map file to `flashmap` to generate a visual breakdown of flash memory regions:

```bash
flashmap firmware.map
```

Example output:

```
Flash Memory Layout
===================
[████████░░░░░░░░░░░░]  42% used  (168 KB / 512 KB)

.text        0x08000000   128 KB  ████████████
.rodata      0x08020000    32 KB  ███
.data        0x08028000     8 KB  █
```

Optional flags:

```
-o <file>    Write output to a file instead of stdout
-f <format>  Output format: text (default), json, csv
-v           Verbose mode
--help       Show help message
```

## Building from Source

```bash
git clone https://github.com/yourname/flashmap.git
cd flashmap
make
```

Requires a C99-compatible compiler and GNU Make.

## License

MIT © 2024 — see [LICENSE](LICENSE) for details.