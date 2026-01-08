# pytrfx: a Python Wrapper for TRFx

A Python wrapper for calling TRFx

## Installation

```bash
# Run from the project root directory
pip install .
```

## Usage

Call trfx with command-line arguments (excluding the program name trfx)

```python
import pytrfx
out, err = pytrfx.main("-a 2 -b 7 -s 30 input.fasta")
```

`out` and `err` are strings containing the output and error messages from trfx.

### Equivalent Commands

The following Python code:
```python
out, err = pytrfx.main("-V")
```
is equivalent to running the command `trfx -V` via `os.system()` or `subprocess` from within Python.

In contrast to spawning an external process using `os.system()` or `subprocess`, `pytrfx` invokes TRFx directly, captures `stdout` and `stderr` as strings, and generally executes much faster.

## Examples

```python
import pytrfx

# Process fasta file with default parameters
out, err = pytrfx.main("test/input.fasta")

# Custom parameters
out, err = pytrfx.main("-a 2 -b 7 -g 7 -s 50 -p 2000 test/input.fasta")
```

## Technical Details

- No modifications to any original .c and .h files
- Exactly the same output as running trfx from the command line
