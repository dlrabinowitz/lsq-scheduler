#!/bin/tcsh  -f

# Make daily report file

setenv QUESTVERBOSE -1
setenv DATATYPE TEST
setenv WRITE_TEST_PIX 1

# launch watchdog program to kill read_camera and write_fits if they are
# still running five minutes later. This prevents this script from hanging
# if the camera power is out.
#
daily_checkout_watchdog &


set ODATA2_FILE = "/scr0/quest/data/dotransfer/raid5.space"

set RECIPIENTS = nancy.ellman@yale.edu,david.rabinowitz@yale.edu,guest,anne.bauer@yale.edu,guest
#set RECIPIENTS = david.rabinowitz@yale.edu
#set RECIPIENTS = nancy.ellman@yale.edu,nan@cs.yale.edu
set TODAY = `getdate -e`
set LOGFILE = ${TODAY}_system_check.log
#
# filter log is a file updated at 3 PM local time by a cronjob 
# running on asok. After the dome opens, the filter may
# be queried by a sending a filter command to questctl
#
#set FILTER_LOG =  "/neat/obsdata/logs/filter.log"
set FILTER_LOG =  $FILTERLOG

date > $LOGFILE
echo " " >> $LOGFILE

# Get filter
echo "getting filter" >> $LOGFILE
tail -1 $FILTER_LOG >> $LOGFILE
#

# Check Disk Space

set DISK = /scr0
echo "Checking disk space:" >> $LOGFILE
set SPACE = `df $DISK | awk '{}END{print $4/1000000}'`
echo "   There are $SPACE Gb available on $DISK" >> $LOGFILE

echo "Checking disk space on odata2:" >> $LOGFILE
cat $ODATA2_FILE >> $LOGFILE


# Check Pressure
#    From /home/observer/questops.dir/questops/bin/log_pressure:
#    get pressure reading in volts (=log10(pressure)+4.0) and

echo "Checking pressure:" >> $LOGFILE
set BIAS_BOARD = 4 
set l = `echo "GET_GAUGES $BIAS_BOARD" | camctl $OBSHOST | grep -v "DONE" | grep -v socket`
#
# pressure used to be the first string returned bu call to camctl. Now it is the second string.
# This probably changed during a recent recompile of ccp and camctl
#
#set q = `echo $l[1] | cut -c 2-10`
if ( $#l < 2 ) then
  echo "ERROR reading pressure : $l" >> $LOGFILE
else
  set q = `echo $l[2] | cut -c 2-10`
  set p = `echo "scale=5;e(l(10)*($q-4))" | bc -l`
  echo "$p Torr" >> $LOGFILE
endif

# Check Temperatures

echo "Checking temperatures:" >> $LOGFILE
foreach fing (1 2 3 4) 
    echo -n "   Finger $fing  " >> $LOGFILE
    echo "READ_TEMP $fing" | tempctl $TEMPHOST  >> $LOGFILE
end

# Check Finger Positions

echo "Checking finger positions:" >> $LOGFILE
foreach fing (1 2 3 4) 
    echo -n "   Finger $fing  " >> $LOGFILE
    echo "WHERE $fing" | fingctl $FINGHOST | grep -v done | grep -v socket >> $LOGFILE
end


# Reset Camera

echo "Resetting camera:" >> $LOGFILE
echo "RESET" | camctl $OBSHOST | grep -v socket >> $LOGFILE

# Take a Stare
echo "Taking a staring dark:" >> $LOGFILE

$PALOMARDIR/bin/check_shm 112 1 
$PALOMARDIR/bin/read_camera -s &
$PALOMARDIR/bin/write_fits -n 1  &

sleep 10
echo READ $OBSHOST 2400 | camctl $OBSHOST | grep -v DONE | grep -v socket >> $LOGFILE
wait
echo STATUS | camctl $OBSHOST | grep -v DONE| grep -v socket >> $LOGFILE

echo " " >> $LOGFILE
echo "Checking connection to pointy:" >> $LOGFILE
ping -w30 -c3 pointy_local >>& $LOGFILE

echo " " >> $LOGFILE 
echo "Comparing machine times:" >> $LOGFILE
set quest7 = `rsh quest7 -l quest date -u`
set quest16 = `date -u`
echo "    quest7  -> $quest7" >> $LOGFILE
echo "    quest16 -> $quest16" >> $LOGFILE

mail -s "$TODAY system check" $RECIPIENTS < $LOGFILE
mv $LOGFILE $HOME/logs/





