# DTSI Generation

[Tutorial](https://xilinx.github.io/kria-apps-docs/creating_applications/2022.1/build/html/docs/dtsi_dtbo_generation.html#example)

```xsct
hsi open_hw_design opendfx_shell_wrapper.xsa
hsi set_repo_path /home/fjtcin/tools/device-tree-xlnx/
hsi create_sw_design device-tree -os device_tree -proc psu_cortexa53_0
hsi set_property CONFIG.dt_overlay true [hsi get_os]
hsi set_property CONFIG.dt_zocl true [hsi get_os]
hsi generate_target -dir main
hsi close_hw_design [hsi current_hw_design]
```

```bash
git clone https://github.com/Xilinx/device-tree-xlnx
cd device-tree-xlnx
git checkout xlnx_rel_v2021.2
```
