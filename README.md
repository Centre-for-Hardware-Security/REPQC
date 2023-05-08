# re_pqc
Reverse engineering post-quantum cryptography in order to insert hardware trojans

## Step 1: synthesize your favorite PQC core
Instructions are provided in https://github.com/Centre-for-Hardware-Security/v2bench 
There is a synthesis script template for Cadence Genus. Run it by executing: `genus -files synth.tcl`. Notice that some changes might be required to reflect the cell naming of your standard cell library. Look for the `dont_use` statements.

## Step 2: convert the generated netlist to a format amenable to RE analysis (i.e., bench)
Again, instructions are available from https://github.com/Centre-for-Hardware-Security/v2bench
You must execute `v2bench in.v out.bench`. The same standard cell terminology that you used in step 1 has to be repeated here. The source code of v2bench has to be recompiled once to make these changes. A compile script for g++ is provided: `./compile.sh`.

## Step 3: execute RELIC on the .bench netlist
This will create an ordered list of registers according to the Z-score. The Z-score is calculated based on the features given [/aux/features.txt](/aux/features.txt)
The command to be used is `cd work; step3.sh`

## Step 4: execute RELIC_PCA on the .bench netlist
This will create groups of registers based on their similarity. To guide the tool, we provide the hint that we are looking for 64-bit registers
The command to be used is `cd work; step4.sh`

## Step 5: execute redpen on the .bench netlist
We are going to use a tool named redpen that creates a list of register pairs following the format "target --> destination". This will create a list of all register dependencies in the design, i.e., which register feeds which register. The command to be used is `cd work; step5.sh`

## (Optional) Step 6: verification
If everything went fine so far, we should be able to find all 1600 register of the Keccak in our Z-score file and they should have very low scores because they are strictly data oriented. Important: the adversary cannot run this verification step because it does a search by register name. We can do it because we know the name a priori.

`cd work; step6.sh`
It should print on the screen what is the lowest, average, and highest z-score of any flip-flop related to the Keccak output.

## Step 7: run our algorithm pqc_re
You can start by trying this configuration:
`../bin/pqc_re ../results/kali.zscore ../results/kali.grouping ../results/kali.depends 64 0.3 100`
which means 
- groups with less than 64 members are ignored
- within a group, there is a tolerance of 30%. if a register z-score is more than 30% off from the dominant group score, this register is discarded as an outlier. the 30% margin works both ways, scores that are 30% higher and 30% lower get discarded.
- number of low-ranked registers to consider during the search. 100 works well because 94 out of the 100 lowest z-scores are related to Keccak. thsi information can be validated from Step 6.

## Step 8: Inspect results
The tool will report the "winning" group as well as all register in it which were not discarded and have a 1-to-1 path to a Keccak register. The results look like this: [/results/result.txt](/results/result.txt)

## (Optional) Exploration
You can play with the parameters passed to `pqc_re`. It works surprisingly well for a very large range of inputs.
