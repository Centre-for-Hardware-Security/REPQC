# this setting is needed for Innovus versions prior to 2019.

set init_design_uniquify 1

# first file is the original netlist that is trojan free. the other 3 files have trojans inserted at design time.

set init_verilog {../netlist/kali_genus.v}
#set init_verilog {../netlist/kali_with_trojan_64_16.v}
#set init_verilog {../netlist/kali_with_trojan_64_32.v}
#set init_verilog {../netlist/kali_with_trojan_64_64.v}

set init_design_netlisttype {Verilog}
set init_design_settop {1}
set init_top_cell {ComputeCoreWrapper}

#tech lef first, cell lef later
set init_lef_file " \
  your tech.lef \
  your cell.lef \
  your mem.lef 
"

set fp_core_cntl {aspect}
set fp_aspect_ratio {1.0000}
set extract_shrink_factor {1.0}
set init_assign_buffer {0}
set init_pwr_net {VDD}
set init_gnd_net {VSS}

# here starts the timing libraries
set init_cpf_file {}
set init_mmmc_file {../scripts/kali.mmmc}

init_design 

# settings begin here
# defines tech node
setDesignMode -process 28
setMultiCpuUsage -localCpu 16

setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7

#this is the VDD for the std cells
globalNetConnect VDD -type pgpin -pin VDD -inst * 

# and the VSS
globalNetConnect VSS -type pgpin -pin VSS -inst *

createBasicPathGroups -expanded 
setPathGroupOptions reg2reg -effortLevel high
setPathGroupOptions reg2cgate -effortLevel low
setPathGroupOptions in2reg -effortLevel high
setPathGroupOptions reg2out -effortLevel low
setPathGroupOptions in2out -effortLevel low

reportPathGroupOptions 

source ../scripts/floorplan.tcl

clearDrc
addHaloToBlock {1 0 1 0} -allBlock
place_opt_design
ccopt_design
routeDesign
routeDesign

setAnalysisMode -analysisType onChipVariation
optDesign -postRoute

suspend

# here are the ECO command for trojan insertion
# only one eco line can be used at a time. you have to close innovus between ECO runs otherwise it crashes. 100M dollar tool :))))
setDesignMode -process 28
setMultiCpuUsage -localCpu 16
setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7
ecoDesign ./kali_baseline_460_670.dat/ ComputeCoreWrapper ../netlist/eco_460_670_T64L16.v
set_default_switching_activity -seq_activity 0.2
checkPlace
report_power

setDesignMode -process 28
setMultiCpuUsage -localCpu 16
setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7
ecoDesign ./kali_baseline_460_670.dat/ ComputeCoreWrapper ../netlist/eco_460_670_T64L32.v
set_default_switching_activity -seq_activity 0.2
checkPlace
report_power

setDesignMode -process 28
setMultiCpuUsage -localCpu 16
setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7
ecoDesign ./kali_baseline_460_670.dat/ ComputeCoreWrapper ../netlist/eco_460_670_T64L64.v
set_default_switching_activity -seq_activity 0.2
checkPlace
report_power

setDesignMode -process 28
setMultiCpuUsage -localCpu 16
setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7
ecoDesign ./kali_baseline_540_670.dat/ ComputeCoreWrapper ../netlist/eco_540_670_T64L16.v
set_default_switching_activity -seq_activity 0.2
checkPlace
report_power

setDesignMode -process 28
setMultiCpuUsage -localCpu 16
setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7
ecoDesign ./kali_baseline_540_670.dat/ ComputeCoreWrapper ../netlist/eco_540_670_T64L32.v
set_default_switching_activity -seq_activity 0.2
checkPlace
report_power

setDesignMode -process 28
setMultiCpuUsage -localCpu 16
setNanoRouteMode -routeBottomRoutingLayer 2
setNanoRouteMode -routeTopRoutingLayer 7
ecoDesign ./kali_baseline_540_670.dat/ ComputeCoreWrapper ../netlist/eco_540_670_T64L64.v
set_default_switching_activity -seq_activity 0.2
checkPlace
report_power


