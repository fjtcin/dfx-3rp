# Copyright (C) 2022, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT
create_pblock pblock_static
add_cells_to_pblock [get_pblocks pblock_static] [get_cells -quiet [list opendfx_shell_i/static_shell opendfx_shell_i/axis_clock_converter_0]]
resize_pblock [get_pblocks pblock_static] -add {PS8_X0Y0:PS8_X0Y0}
resize_pblock [get_pblocks pblock_static] -add {VCU_X0Y0:VCU_X0Y0}
resize_pblock pblock_static -add {SLICE_X0Y0:SLICE_X16Y239}
resize_pblock pblock_static -add {BUFG_PS_X0Y0:BUFG_PS_X0Y95}
resize_pblock pblock_static -add {DSP48E2_X0Y0:DSP48E2_X3Y95}
resize_pblock pblock_static -add {IOB_X0Y0:IOB_X0Y207}
set_property CONTAIN_ROUTING 1 [get_pblocks pblock_static]
set_property IS_SOFT FALSE [get_pblocks pblock_static]
set_property SNAPPING_MODE ON [get_pblocks pblock_static]

create_pblock shim_rp_0
add_cells_to_pblock shim_rp_0 [get_cells -quiet [list opendfx_shell_i/static_shell/shim_rp0]]
resize_pblock shim_rp_0 -add {SLICE_X0Y0:SLICE_X16Y59 DSP48E2_X0Y0:DSP48E2_X3Y23 IOB_X0Y0:IOB_X0Y51}

create_pblock shim_rp_1
add_cells_to_pblock shim_rp_1 [get_cells -quiet [list opendfx_shell_i/static_shell/shim_rp1]]
resize_pblock shim_rp_1 -add {SLICE_X0Y180:SLICE_X16Y239 DSP48E2_X0Y72:DSP48E2_X3Y95 IOB_X0Y156:IOB_X0Y207}

create_pblock shim_rp_2
add_cells_to_pblock shim_rp_2 [get_cells -quiet [list opendfx_shell_i/static_shell/shim_rp2]]
resize_pblock shim_rp_2 -add {SLICE_X0Y60:SLICE_X16Y179 DSP48E2_X0Y24:DSP48E2_X3Y71 IOB_X0Y52:IOB_X0Y155}

set_property parent pblock_static [get_pblocks shim_rp_0]
set_property parent pblock_static [get_pblocks shim_rp_1]
set_property parent pblock_static [get_pblocks shim_rp_2]


create_pblock pblock_0
add_cells_to_pblock [get_pblocks pblock_0] [get_cells -quiet [list opendfx_shell_i/RP_0]]
resize_pblock pblock_0 -add {SLICE_X17Y0:SLICE_X60Y59}
resize_pblock pblock_0 -add {BUFG_GT_X0Y0:BUFG_GT_X0Y23}
resize_pblock pblock_0 -add {BUFG_GT_SYNC_X0Y0:BUFG_GT_SYNC_X0Y14}
resize_pblock pblock_0 -add {DSP48E2_X4Y0:DSP48E2_X12Y23}
resize_pblock pblock_0 -add {GTHE4_CHANNEL_X0Y0:GTHE4_CHANNEL_X0Y3 GTHE4_COMMON_X0Y0:GTHE4_COMMON_X0Y0}
resize_pblock pblock_0 -add {IOB_X1Y0:IOB_X1Y51}
resize_pblock pblock_0 -add {PCIE40E4_X0Y0:PCIE40E4_X0Y0}
resize_pblock pblock_0 -add {RAMB18_X0Y0:RAMB18_X2Y23 RAMB36_X0Y0:RAMB36_X2Y11}
resize_pblock pblock_0 -add {URAM288_X0Y0:URAM288_X0Y15}
set_property CONTAIN_ROUTING 1 [get_pblocks pblock_0]
set_property EXCLUDE_PLACEMENT 1 [get_pblocks pblock_0]
set_property IS_SOFT FALSE [get_pblocks pblock_0]
set_property SNAPPING_MODE ON [get_pblocks pblock_0]

create_pblock pblock_1
add_cells_to_pblock [get_pblocks pblock_1] [get_cells -quiet [list opendfx_shell_i/RP_1]]
resize_pblock pblock_1 -add {SLICE_X17Y180:SLICE_X60Y239}
resize_pblock pblock_1 -add {BUFG_GT_X0Y72:BUFG_GT_X0Y95}
resize_pblock pblock_1 -add {BUFG_GT_SYNC_X0Y45:BUFG_GT_SYNC_X0Y59}
resize_pblock pblock_1 -add {CFGIO_SITE_X0Y0:CFGIO_SITE_X0Y0}
resize_pblock pblock_1 -add {DSP48E2_X4Y72:DSP48E2_X12Y95}
resize_pblock pblock_1 -add {GTHE4_CHANNEL_X0Y12:GTHE4_CHANNEL_X0Y15 GTHE4_COMMON_X0Y3:GTHE4_COMMON_X0Y3}
resize_pblock pblock_1 -add {IOB_X1Y156:IOB_X1Y207}
resize_pblock pblock_1 -add {RAMB18_X0Y72:RAMB18_X2Y95 RAMB36_X0Y36:RAMB36_X2Y47}
resize_pblock pblock_1 -add {SYSMONE4_X0Y0:SYSMONE4_X0Y0}
resize_pblock pblock_1 -add {URAM288_X0Y48:URAM288_X0Y63}
set_property CONTAIN_ROUTING 1 [get_pblocks pblock_1]
set_property EXCLUDE_PLACEMENT 1 [get_pblocks pblock_1]
set_property IS_SOFT FALSE [get_pblocks pblock_1]
set_property SNAPPING_MODE ON [get_pblocks pblock_1]

create_pblock pblock_2
add_cells_to_pblock [get_pblocks pblock_2] [get_cells -quiet [list opendfx_shell_i/RP_2]]
resize_pblock pblock_2 -add {SLICE_X17Y60:SLICE_X60Y179}
resize_pblock pblock_2 -add {BUFG_GT_X0Y24:BUFG_GT_X0Y71}
resize_pblock pblock_2 -add {BUFG_GT_SYNC_X0Y15:BUFG_GT_SYNC_X0Y44}
resize_pblock pblock_2 -add {CONFIG_SITE_X0Y0:CONFIG_SITE_X0Y0}
resize_pblock pblock_2 -add {DSP48E2_X4Y24:DSP48E2_X12Y71}
resize_pblock pblock_2 -add {GTHE4_CHANNEL_X0Y4:GTHE4_CHANNEL_X0Y11 GTHE4_COMMON_X0Y1:GTHE4_COMMON_X0Y2}
resize_pblock pblock_2 -add {IOB_X1Y52:IOB_X1Y155}
resize_pblock pblock_2 -add {PCIE40E4_X0Y1:PCIE40E4_X0Y1}
resize_pblock pblock_2 -add {RAMB18_X0Y24:RAMB18_X2Y71 RAMB36_X0Y12:RAMB36_X2Y35}
resize_pblock pblock_2 -add {URAM288_X0Y16:URAM288_X0Y47}
set_property CONTAIN_ROUTING 1 [get_pblocks pblock_2]
set_property EXCLUDE_PLACEMENT 1 [get_pblocks pblock_2]
set_property IS_SOFT FALSE [get_pblocks pblock_2]
set_property SNAPPING_MODE ON [get_pblocks pblock_2]
