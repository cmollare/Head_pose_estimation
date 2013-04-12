#!/bin/bash

inputFile=train_neg.txt
output_file=neg.xml

exec<$inputFile
let "i=0"
read firsline
while read name bbox1X bbox1Y bbox2X bbox2Y 
do
	echo "<Image name=\"$name\" />" >> $output_file;
done

