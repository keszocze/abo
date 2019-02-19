#!/bin/bash

errcho() { >&2 echo $@; }

echo "Checking C++ code quality using cppcheck"

ALLOWED_ERRS=28

cpp_check() {
	logfile=cppcheck.log
	cppcheck --enable=all src 1>/dev/null 2> $logfile
	nerr=$(wc -l < $logfile)
	#echo "found $nerr cppcheck issues"

	if [ $nerr -gt $ALLOWED_ERRS ]; then
		errcho "Too many errors: $nerr ($ALLOWED_ERRS allowed)"
		exit $(($nerr - $ALLOWED_ERRS))
	fi
	
	rm $logfile
}


which cppcheck 2>&1 > /dev/null
if [ ! $? ]; then
	success=1
	errcho "cppcheck not found!"	
	exit -1 
else
	cpp_check
fi

exit 0
