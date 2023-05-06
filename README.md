# re_pqc
Reverse engineering post-quantum cryptography in order to insert hardware trojans

## Step 1: synthesize your favorite PQC core
Instructions are provided in using the instructions provided in https://github.com/Centre-for-Hardware-Security/v2bench There is a synthesis script template for Cadence Genus. Run it by executing: `genus -files synth.tcl`

## Step 2: convert the generated netlist to a format amenable to RE analysis (i.e., bench)
Again, instructions are available from https://github.com/Centre-for-Hardware-Security/v2bench
`v2bench in.v out.bench`

## Step 3: execute RELIC on the .bench netlist
This will create an ordered list of registers according to the Z-score
The command to be used is `cd work; step3.sh`

## Step 4: execute RELIC_PCA on the .bench netlist
This will create groups of registers based on their similarity. To guide the tool, we provide the hint that we are looking for 64-bit registers
The command to be used is `cd work; step4.sh`

## (Optional) Step 5: verification
If everything went fine so far, we should be able to find all 1600 register of the Keccak in our Z-score file and they should have very low scores because they are strictly data oriented. Important: the adversary cannot run this verification step because it does a search by register name. We can do it because we know the name a priori.

`cd work; step5.sh`
It should print on the screen what is the lowest, average, and highest z-score of any flip-flop related to keccak output.

## Step 6: run our algorithm pqc_re
`cd work; step6.sh`

## (Optional) Steps 7 and 8: verification with redpen
If our analysis is correct, there must be a path between the target register and the Keccak module. We are going to use a tool named redpen that creates a list of register pairs following the format "target --> destination". 
