from setuptools import setup, Extension
import os
import sys
import subprocess

# 获取项目根目录
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

# 源文件列表（不包括trfx.c，因为会重新编译）
source_files = [
    'pytrfx/pytrfx_module.c',
    'pytrfx/trfx_wrapper.c',
    'kthread.c',
    'bseq.c',
    'map.c',
    'trfx.c',  # 这个会通过-Dmain=trfx_main重命名main函数
]

# 转换为绝对路径
source_files = [os.path.join(ROOT_DIR, f) for f in source_files]

# 编译参数
shim_path = os.path.join(ROOT_DIR, 'pytrfx', 'exit_shim.h')

extra_compile_args = [
    '-g',
    '-Wall',
    '-O2',
    '-Wno-unused-function',
    '-Dmain=trfx_main',  # 关键：将main重命名为trfx_main
    '-DNO_GPU',  # 默认使用CPU版本
    '-include', shim_path,  # 强制包含shim头文件，将exit替换为return
]

# 链接参数
extra_link_args = [
    '-lm',
    '-lz',
    '-lpthread',
]

# 包含目录
include_dirs = [
    ROOT_DIR,
    'pytrfx',
]

# 定义扩展模块
ext_modules = [
    Extension(
        '_pytrfx',
        sources=source_files,
        include_dirs=include_dirs,
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    )
]

# 读取README（如果存在）
long_description = ""
readme_path = os.path.join(ROOT_DIR, 'README.md')
if os.path.exists(readme_path):
    with open(readme_path, 'r', encoding='utf-8') as f:
        long_description = f.read()

setup(
    name='pytrfx',
    version='1.0.0',
    description='Python wrapper for TRFx - Tandem Repeat Finder',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='TRFx Team',
    packages=['pytrfx'],
    ext_modules=ext_modules,
    python_requires='>=3.6',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Bio-Informatics',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: C',
    ],
)
