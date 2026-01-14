from setuptools import setup, Extension
import os

# Get project root directory
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

# Source file list
source_files = [
    'pytrfx/pytrfx_module.c',
    'pytrfx/trfx_wrapper.c',
    'kthread.c',
    'bseq.c',
    'map.c',
    'trfx.c',
]

# Convert to absolute paths
source_files = [os.path.join(ROOT_DIR, f) for f in source_files]

# Compile arguments
shim_path = os.path.join(ROOT_DIR, 'pytrfx', 'exit_shim.h')

extra_compile_args = [
    '-g',
    '-Wall',
    '-O2',
    '-Wno-unused-function',
    '-Dmain=trfx_main',  # Important: rename main to trfx_main
    '-DNO_GPU',  # Default to CPU version
    '-include', shim_path,  # Force-include shim header to replace exit with return
]

# Link arguments
extra_link_args = [
    '-lm',
    '-lz',
    '-lpthread',
]

# Include directories
include_dirs = [
    ROOT_DIR,
    'pytrfx',
]

# Define extension module(s)
ext_modules = [
    Extension(
        '_pytrfx',
        sources=source_files,
        include_dirs=include_dirs,
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    )
]

# Read README (if present)
long_description = ""
readme_path = os.path.join(ROOT_DIR, 'README.md')
if os.path.exists(readme_path):
    with open(readme_path, 'r', encoding='utf-8') as f:
        long_description = f.read()

setup(
    name='pytrfx',
    version='1.0.0',
    description='Python wrapper for TRFx - a multi-threaded Tandem Repeats Finder',
    long_description=long_description,
    long_description_content_type='text/markdown',
    packages=['pytrfx'],
    ext_modules=ext_modules,
    python_requires='>=3.9',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Bio-Informatics',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: Python :: 3.13',
        'Programming Language :: C',
    ],
)
