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
| 16×16 | 0.034 | 0.019 | 1.8× |
| 32×32 | 0.26 | 0.047 | 5.5× |
| 36×36 | 0.37 | 0.052 | 7.1× |
| 60×60 | 1.7 | 0.17 | 10× |
| 64×64 | 2.0 | 0.24 | 8.3× |
| 96×96 | 7.3 | 0.56 | 13× |
| 120×120 | 15 | 1.0 | 15× |
| 127×127 | 17 | 1.3 | 13× |
| 128×128 | 32 | 1.3 | 25× |
