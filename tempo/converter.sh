#!/bin/bash

inputFile=train_pos.txt
output_file=pos.xml

exec<$inputFile
let "i=0"
read firsline
while read name bbox1X bbox1Y bbox2X bbox2Y centerX centerY
do
	echo "<Image name=\"$name\" bbox1X=\"$bbox1X\" bbox1Y=\"$bbox1Y\" bbox2X=\"$bbox2X\" bbox2Y=\"$bbox2Y\" centerX=\"$centerX\" centerY=\"$centerY\" />" >> $output_file;
done

