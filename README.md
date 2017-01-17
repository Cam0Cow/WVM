# WVM

## What is WVM?
WVM is an emulator for a nonexistent machine, aptly named the Weird Virtual
Machine the WVM has 64k emulated RAM, with two hardware stacks, perfect for
FORTH like languages.


## Overview
As well as being dual-stack, the WVM is also dual-size oriented; each
instruction has both a byte variant and a cell (16 bit) variant.

some sample code:

pushb 6
pushb 7
mulb
logb

which prints 42
