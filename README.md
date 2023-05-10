# re_pqc
Reverse engineering post-quantum cryptography in order to insert hardware trojans

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
This will create groups of registers based on their similarity. It groups registers based on features given in [/aux/features.groups.txt](/aux/features.groups.txt)
The command to be used is `cd work; step4.sh`

## Step 5: execute REDPEN on the .bench netlist
We are going to use a tool named redpen that creates a list of register pairs following the format "target --> destination". This will create a list of all register dependencies in the design, i.e., which register feeds which register. The command to be used is `cd work; step5.sh`

## (Optional) Step 6: verification
If everything went fine so far, we should be able to find all 1600 registers of the Keccak state in our Z-score file and they should have very low scores because they are data oriented. Important: the adversary cannot run this verification step because it does a search by register name. We can do it because we know the name a priori.

`cd work; step6.sh`
It should print on the screen what is the lowest, average, and highest Z-score of any flip-flop related to the Keccak state.

## Step 7: Run the pqc_re algorithm
You can start by trying this configuration:
`../bin/pqc_re ../results/kali.tool1.zscore ../results/kali.tool1.grouping ../results/kali.tool1.depends 64 300`
which means 
- groups with less than 64 members are ignored
- number of low-ranked registers to consider during the search. 300 works well because 50 out of the 100 lowest z-scores are related to Keccak (this information can be validated from Step 6). 

### What is pqc_re actually doing? ###
Here is a pseudo code:

```
Input: Lreg, a list of registers ordered by their Z-score
Input: Lgrp, a list of groups of registers
Input: targetRegs, number of regs to consider

var regs := 0
var winner := nil

for each reg in Lreg {
	if (reg.fanin < 64) then continue
	for each grp in Lgrp {
		hit := false
		for each member in grp {
			if (member == reg) then break
			if (PATH(member,reg)) then hit := true
		}
		if (hit == false) then DELETE(grp, Lgrp)
		if (Lgrp.size == 1) break
	}
	
	regs := regs + 1
	if (regs == targetRegs) break
}

winner := ELEMENT(Lgrp, 0)
SORT(winner)
PICK_LOW_64(winner)

```

## Step 8: Inspect results
The tool will report the "winning" group as well as 64 registers from it with the lowest Z-score. These registers are guaranteed to have a 1-to-1 path to a Keccak state register. 

## (Optional) Exploration
You can play with the parameters passed to `pqc_re`. It works surprisingly well for a very large range of inputs. Anything between 300 and 600 works well for kali.tool1.
