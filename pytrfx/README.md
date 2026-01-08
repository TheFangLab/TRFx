# pytrfx - Python Wrapper for TRFx

Python包装器，用于调用TRFx（Tandem Repeat Finder）工具。

## 安装

```bash
# 在项目根目录下运行
pip install .

# 或者开发模式安装
pip install -e .
```

## 使用方法

```python
import pytrfx

# 调用trfx，传入命令行参数（不包括程序名trfx）
out, err = pytrfx.main("-a 2 -b 7 -s 30 input.fasta")

# out和err是bytes对象，包含标准输出和标准错误
print("STDOUT:", out.decode('utf-8'))
print("STDERR:", err.decode('utf-8'))
```

## 等效命令

```python
out, err = pytrfx.main(cmd_string)
```

等效于在shell中运行：
```bash
trfx {cmd_string}
```

但是可以捕获stdout和stderr输出。

## 示例

```python
import pytrfx

# 使用默认参数处理fasta文件
out, err = pytrfx.main("test/input.fasta")

# 自定义参数
out, err = pytrfx.main("-a 2 -b 7 -g 7 -s 50 -p 2000 test/input.fasta")

# 输出NGS格式
out, err = pytrfx.main("-n test/input.fasta")
```

## 技术细节

- 使用Python C扩展实现
- 通过编译选项`-Dmain=trfx_main`将原始的main函数重命名为trfx_main
- 使用管道捕获stdout和stderr输出
- 不修改任何原始.c和.h文件
- 所有包装代码位于pytrfx/目录中

## 文件结构

```
pytrfx/
├── __init__.py          # Python包接口
├── pytrfx_module.c      # Python C扩展模块
├── trfx_wrapper.c       # C包装函数（捕获输出）
└── trfx_wrapper.h       # 头文件
```

## 构建说明

setup.py会自动：
1. 编译所有必要的C源文件（kthread.c, bseq.c, map.c, trfx.c）
2. 使用`-Dmain=trfx_main`重命名main函数
3. 编译包装代码（trfx_wrapper.c, pytrfx_module.c）
4. 链接所有对象文件生成Python扩展模块

## 依赖

- Python >= 3.6
- C编译器（gcc）
- zlib开发库

## License

与TRFx项目相同
