"""
pytrfx - Python wrapper for TRFx (Tandem Repeat Finder)

This package provides a Python interface to the TRFx tool for finding tandem repeats in DNA sequences.

Example:
    >>> import pytrfx
    >>> out, err = pytrfx.main("-a 2 -b 7 input.fasta")
    >>> print(out.decode('utf-8'))
"""

from _pytrfx import main as _main

__version__ = "1.0.0"
__all__ = ['main']


def main(cmd_string):
    """
    Run TRFx with the given command string.
    
    Args:
        cmd_string (str): Command line arguments for trfx (without the 'trfx' program name)
        
    Returns:
        tuple: (stdout, stderr) as bytes objects
        
    Example:
        >>> out, err = pytrfx.main("-a 2 -b 7 -s 30 input.fasta")
        >>> print(err.decode('utf-8'))  # Print stderr output
    """
    return _main(cmd_string)
