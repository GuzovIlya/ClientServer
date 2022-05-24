#!/bin/bash

read File_Name
Hour=3600000 #3600000 миллисек в часу
Previous_Lines=0
Count=0
Flag=start
End=end

while [ "$Flag" != "$End" ]
do
    count=$(( $Count + 1 ))
    ./client $File_Name &
    current_lines=$(cat time.txt | wc -l)
    while [ "$Current_Lines" == "$Previous_Lines" ]
    do
        Current_Lines=$(cat time.txt | wc -l)
    done
    Current_Time=$(tail -n 1 time.txt)
    Previous_Lines=$Current_Lines
    #echo "time = $Current_Time"
if [ $Current_Time -ge $Hour ]
then
    Flag=end
    echo "count = $Count"
    echo "time = $Current_Time"
    echo "hour = $Hour"
    echo "flag = $Flag"
    echo "end = $End"
fi
done
exit 0
#netstat -atnp