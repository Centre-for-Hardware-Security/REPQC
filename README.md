# REPQC
Reverse engineering post-quantum cryptography in order to insert hardware trojans!

If you want to use the bench files provided with this repository, skip to step 3. If you want to generate your own, follow steps 1 and 2.

## Step 1: synthesize your favorite PQC core
Instructions are provided in https://github.com/Centre-for-Hardware-Security/v2bench 
There is a synthesis script template for Cadence Genus. Run it by executing: `genus -files synth.tcl`. Notice that some changes might be required to reflect the cell naming of your standard cell library. Look for the `dont_use` statements in the synthesis script.

## Step 2: convert the generated netlist to a format amenable to RE analysis (i.e., bench)
Again, instructions are available from https://github.com/Centre-for-Hardware-Security/v2bench
You must execute `v2bench in.v out.bench`. The same standard cell terminology that you used in Step 1 has to be repeated here. The source code of v2bench has to be recompiled once to make these changes. A compile script for g++ is provided: `./compile.sh`.

## Step 3: execute RELIC on the .bench netlist
This will create an ordered list of registers according to the Z-score. The Z-score is calculated based on the features given [/aux/features.zscore.txt](/aux/features.zscore.txt)
The command to be used is `cd work; step3.sh`

## Step 4: execute RELIC_PCA on the .bench netlist
This will create registers (groups of flip-flops) based on similarity. It utilizes the features given in [/aux/features.groups.txt](/aux/features.groups.txt)
The command to be used is `cd work; step4.sh`

## Step 5: execute REDPEN on the .bench netlist
We are going to use a tool named REDPEN that creates a list of flip-flop dependencies following the format "target --> destination". This will create a list of all dependencies in the design, i.e., which flip-flop feeds which flip-flop. The command to be used is `cd work; step5.sh`

## (Optional) Step 6: verification
If everything went fine so far, we should be able to find all 1600 registers of the Keccak state in our Z-score file and they should have low scores because they are data oriented. Important: the adversary cannot run this verification step because it does a search by register name. We can do it because we know the name a priori.

`cd work; step6.sh`
It should print on the screen what is the lowest, average, and highest Z-score of any flip-flop related to the Keccak state. It will also output this information as a CSV file.

## Step 7: Run the REPQC algorithm
You can start by trying this configuration:
`../bin/RE_PQC ../results/kali_v1.zscore ../results/kali_v1.grouping ../results/kali_v1.depends 64 -1`
which means 
- groups with less than 64 members are ignored
- all flip-flops will be considered during the search (-1). any other number can be used. in some circuits, as litle as 300 flip-flops are needed to get a good enough result. One can also use the hints from step6 to decide what number to use here.

*All results reported in the paper use 64, -1*

### What is REPQC actually doing? ###
Here is a pseudo code:

```
Input: Lff, a list of flops ordered by their Z-score
Input: Lgrp, a list of registers
Input: target, number of flops to consider

var regs := 0
var winner := nil

for each ff in Lff {
	if (ff.fanin and ff.fanout are IN_RANGE) then continue
	for each grp in Lgrp {
		hit := false
		for each member in grp {
			if (member == ff) then break
			if (PATH(member,ff)) then hit := true
		}
		if (hit == false) then DELETE(grp, Lgrp)
		if (Lgrp.size == 1) break
	}
	
	regs := regs + 1
	if (regs == target) break
}

winner := ELEMENT(Lgrp, 0)
SORT(winner)
PICK_LOW_64(winner)

```

## Step 8: Inspect results
The tool will report the "winning" group as well as 64 registers from it with the lowest Z-score. The tool will also report the properties of these registers. It becomes evident that all Keccak input flops have the same properties: fanin, fanout, hits, etc... 

## (Optional) Exploration
You can play with the parameters passed to `RE_PQC`. There are also compile-time parameters defined in params.h. It works surprisingly well for a very large range of inputs
