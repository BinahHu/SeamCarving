#!/bin/sh
for((ind = 1; ind < 9; ind++)); do
	name="p$ind";
	./SeamCarving $name;
done
