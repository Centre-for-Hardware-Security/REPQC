#setPlanDesignMode -congAware true 
setPlanDesignMode -legalize true 
setPlanDesignMode -macroSpacing 10
setPlanDesignMode -minMacroToCoreSpace 5
setPlanDesignMode -util 0.75

# you can change core_x and core_y, the others should remain the same. the considered values in the paper are 470x670 and 540x670.
set FP_RING_OFFSET 1
set FP_RING_WIDTH 2
set FP_RING_SPACE 1
set FP_CORE_X 540
set FP_CORE_Y 670
set FP_RING_SIZE [expr {$FP_RING_SPACE + 2*$FP_RING_WIDTH + $FP_RING_OFFSET}]

floorPlan -site core -s $FP_CORE_X $FP_CORE_Y $FP_RING_SIZE $FP_RING_SIZE $FP_RING_SIZE $FP_RING_SIZE -noSnap

set pinlist1 {}
foreach_in_collection input [all_inputs] {lappend pinlist1 [get_property $input hierarchical_name]}

set pinlist2 {}
foreach_in_collection output [all_outputs] {lappend pinlist2 [get_property $output hierarchical_name]}

setPinAssignMode -pinEditInBatch true
editPin -fixOverlap 1 -unit MICRON -spreadDirection clockwise -side Left -layer 3 -spreadType center -spacing 2.016 -pin [join $pinlist1]
editPin -fixOverlap 1 -unit MICRON -spreadDirection clockwise -side Right -layer 3 -spreadType center -spacing 2.016 -pin [join $pinlist2]
editPin -snap TRACK -pin *
setPinAssignMode -pinEditInBatch false
legalizePin

planDesign -constraints template.txt

win

setLayerPreference node_blockage -isVisible 0

dehighlight
deselectAll
selectInst *CORE_BR_*BR1
selectInst *CORE_BR_*BR2
selectInst *CORE_BR_*BR3
selectInst *CORE_BR_*BR4
selectInst *CORE_BR_*BR5
highlight -index 56
deselectAll

selectInst *isc_INST_RAM*
highlight -index 56
deselectAll

selectInst *keccak_state_reg*
highlight -index 49
deselectAll

selectInst *3_doutb_reg*
highlight -index 2
deselectAll


return

