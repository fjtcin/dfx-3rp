# dfx-3rp

## clean local repo

```bash
git clean -dfX
```

## generate RPs

```bash
vivado -mode batch -notrace -source ./opendfx_shell.tcl
```

## copy RMs

```bash
python finalize.py
```

| Matrix Size | CPU Time (ms) | FPGA time (ms) | Speedup |
| :---: | :---: | :---: | :---: |
| 12×12 | 0.015 | 0.012 | 1.3× |
| 36×36 | 0.37 | 0.052 | 7.1× |
| 60×60 | 1.7 | 0.17 | 10× |
| 96×96 | 7.3 | 0.56 | 13× |
| 120×120 | 15 | 1.0 | 15× |
| 128×128 | 32 | 1.3 | 25× |
