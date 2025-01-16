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
