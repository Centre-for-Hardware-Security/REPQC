# this script is used to synthesized the controller of the trojan since that is fully digital logic

#set directories path for 28nm 
set init_lib { your.lib goes here }
			 
#set design name (or) top module name
set DESIGN "trojan_64_64"

# change this line to change the trojan
set RTL_LIST { ../rtl/trojan_64_64.v }

set_db hdl_error_on_latch true
set_db lp_insert_clock_gating true

read_libs ${init_lib}

read_hdl ${RTL_LIST}

# these lines rpevent the use of latches and scan flops. naming can be different in other libraries...
set_dont_use *SDF*
set_dont_use *SEDF*
set_dont_use LH*
set_dont_use LN*

# Elaborate the top level
elaborate $DESIGN

# Read the constraint file
create_clock -name "clk" -period 0.45 [get_ports clk]
set_input_delay -clock clk 0.001 [all_inputs]
set_output_delay -clock clk 0.001 [all_outputs]

set_db syn_generic_effort high
syn_generic

set_db syn_map_effort high
syn_map

# OPT ###
set_db syn_opt_effort high
syn_opt
syn_opt -incr

# ungrouping makes it easier to handle the netlist and make the ECO insertion later
ungroup -flatten -all

#report timing > ../report/timing_28nm_${DESIGN}.rep
#report timing -lint > ../report/timing_lint_28nm_${DESIGN}.rep
#report area	  > ../report/area_28nm_${DESIGN}.rep
#report gates  > ../report/gates_28nm_${DESIGN}.rep
#report power  > ../report/power_28nm_${DESIGN}.rep
#report power -flat > ../report/power_flat_28nm_${DESIGN}.rep
#check_design > ../report/check_design_28nm_${DESIGN}.rep
#write_hdl -mapped >  ../netlist/netlist_28nm_${DESIGN}.v
#write_sdc         >  ../sdc/constraint_65nm_${DESIGN}.sdc





