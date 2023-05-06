# re_pqc
Reverse engineering post-quantum cryptography in order to insert hardware trojans

## Step 1: synthesize your favorite PQC core using the instructions provided in https://github.com/Centre-for-Hardware-Security/v2bench
There is a synthesis script template for Cadence Genus
`genus -files synth.tcl`

## Step 2: convert the generated netlist to a format amenable to RE analysis (i.e., bench)
Again, instructions are available from https://github.com/Centre-for-Hardware-Security/v2bench

## Step 3: execute RELIC on the .bench netlist
This will create an ordered list of registers according to the Z-score
The command
