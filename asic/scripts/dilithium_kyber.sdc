# ####################################################################

#  Created by RTL Compiler (RC) 05.20-p002 on Wed Feb 22 10:43:54 AM CST 2006

# ####################################################################

set sdc_version 1.4

# *********************************************************************
# Set the current design
# *********************************************************************

current_design ComputeCoreWrapper


create_clock -name "clk" -period 0.455 [get_ports clk]
#create_clock -name "clk" -period 0.50 [get_ports clk]
set_clock_transition 0.0 [get_clocks clk]
set_input_delay -clock clk 0.001 [all_inputs]
set_output_delay -clock clk 0.001 [all_outputs]
create_generated_clock -divide_by 2 CLKDIV/out_clk -source clk


