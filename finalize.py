import shutil
from pathlib import Path

ACCEL_JSON = """{
  "AccelHandshakeType": "streamDataFromTrg",
  "accel_devices": [
    {
      "dev_name": "$ADDR1.AccelConfig",
      "reg_base": "",
      "reg_size": "65536"
    },
    {
      "dev_name": "$ADDR2.rm_comm_box",
      "reg_base": "",
      "reg_size": ""
    }
  ],
  "accel_metadata": {},
  "accel_type": "SIHA_PL_DFX"
}
"""
RM_DTSI = """/dts-v1/;
/plugin/;
/ {
	fragment@0 {
		target = <&fpga_PR$RPNUM>;
		__overlay__ {
			firmware-name = "opendfx_shell_i_RP_$RPNUM_$RMNAME_inst_$RMNUM_partial.bit.bin";
			partial-fpga-config;
		};
	};
};
"""

accel_json_slot0 = ACCEL_JSON.replace("$ADDR1", "80000000").replace("$ADDR2", "81000000")
accel_json_slot1 = ACCEL_JSON.replace("$ADDR1", "82000000").replace("$ADDR2", "83000000")
accel_json_slot2 = ACCEL_JSON.replace("$ADDR1", "84000000").replace("$ADDR2", "85000000")
accel_json_dict = {
    0: accel_json_slot0,
    1: accel_json_slot1,
    2: accel_json_slot2
}

shutil.copy("./project_1/project_1.runs/impl_1/opendfx_shell_wrapper.bit", "./configs/")

modules = [
    ("TWICE0", "impl_1", (True, False, False)),
    ("TWICE1", "impl_1", (False, True, False)),
    ("TWICE2", "impl_1", (False, False, True)),
    ("GEMM", "child_1_impl_1", (False, False, True)),
    ("COOdec", "child_1_impl_1", (True, False, False)),
    ("COOenc", "child_1_impl_1", (False, True, False)),
    ("CSRdec", "child_2_impl_1", (True, False, False)),
    ("CSRenc", "child_2_impl_1", (False, True, False)),
    ("CSCdec", "child_3_impl_1", (True, False, False)),
    ("CSCenc", "child_3_impl_1", (False, True, False))
]

for module in modules:
    name, impl, mask = module
    Path(f"./configs/{name}/").mkdir(exist_ok=True)

    rm_num = 0
    for i in range(3):
        if not mask[i]: continue
        p = Path(f"./configs/{name}/{name}_slot{i}")
        p.mkdir(exist_ok=True)
        shutil.copy(f"./project_1/project_1.runs/{impl}/opendfx_shell_i_RP_{i}_{name}_inst_{rm_num}_partial.bit", p)
        with open(p / "accel.json", "w") as f: f.write(accel_json_dict.get(i))
        with open(p / f"opendfx_shell_i_RP_{i}_{name}_inst_{rm_num}_partial.dtsi", "w") as f: f.write(RM_DTSI.replace("$RPNUM", str(i)).replace("$RMNAME", name).replace("$RMNUM", str(rm_num)))
        rm_num += 1
