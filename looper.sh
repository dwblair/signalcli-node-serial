#!/bin/bash

while true;
do node main.js --user $1 --port $2;
	sleep 10;
done

