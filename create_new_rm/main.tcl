source ./hw_prj.tcl
source ../rm_tcl/axis_rm_template.tcl
cr_bd_axis_rm "" ADDSUB RP_0
cr_bd_axis_rm "" ADDSUB RP_1
cr_bd_axis_rm "" ADDSUB RP_2
export_bd_synth -force [get_files ./project_1/project_1.srcs/sources_1/bd/ADDSUB_RP_0/ADDSUB_RP_0.bd]
export_bd_synth -force [get_files ./project_1/project_1.srcs/sources_1/bd/ADDSUB_RP_1/ADDSUB_RP_1.bd]
export_bd_synth -force [get_files ./project_1/project_1.srcs/sources_1/bd/ADDSUB_RP_2/ADDSUB_RP_2.bd]
source ./generate_rm.tcl
generate_rm ADDSUB RP_0
generate_rm ADDSUB RP_1
generate_rm ADDSUB RP_2
