#!/bin/bash
REV=$1

if [ ! $REV ]
then
    REV='HEAD'
fi

echo "Preparing source file export for project build @revision $REV"
sleep 1

cd /usr/local/watchman-alerting

echo "Deleting old files"

if [ -f watchman-alerting.pl ]
then
    rm -rf watchman-alerting.pl
fi

if [ -d lib/ ]
then
    rm -rf lib/
fi

echo "Exporting main source file > watchman-alerting.pl"

svn cat -r $REV https://svn.fhwm.net/svn/WatchmanAlerting/trunk/watchman-alerting.pl > watchman-alerting.pl
if [ $? -eq 0 ]
then
    echo "Done"
else
    echo "Error ($?) during svn export. Fatal."
    exit
fi

echo "Exporting library source => lib/"

svn export -r $REV https://svn.fhwm.net/svn/WatchmanAlerting/trunk/lib |
while read a b c
do
    if [ $c ]
    then
        BUILD_REV=`echo $c | cut -d'.' -f1`

        if [ $BUILD_REV ]
        then
        	echo "Checking syntax"
        	`perl -wc /usr/local/watchman-alerting/watchman-alerting.pl`
        	if [ $? -ne 0 ]
        	then
        		echo "Syntax errors, compilation failed"
        		exit
        	fi
        	
            NAME="watchman-alerting-v5.$BUILD_REV"
            echo "Compiling @revision $BUILD_REV => $NAME"

            `/usr/local/bin/pp -o /usr/local/watchman-alerting/bin/$NAME /usr/local/watchman-alerting/watchman-alerting.pl`
			echo "/usr/local/bin/pp -o /usr/local/watchman-alerting/bin/$NAME /usr/local/watchman-alerting/watchman-alerting.pl"
			
            if [ $? -eq 0 ]
            then
                TAR="$NAME-bin.tar.gz"

                `tar czvf /usr/local/watchman-alerting/tar/$TAR /usr/local/watchman-alerting/bin/$NAME`
                echo "tar czvf /usr/local/watchman-alerting/tar/$TAR /usr/local/watchman-alerting/bin/$NAME"
                
                if [ $? -eq 0 ]
                then
                    if [ -f public-hostname ]
                    then
                        rm -rf public-hostname
                    fi

                    wget http://169.254.169.254/latest/meta-data/public-hostname
                    DNS=`cat public-hostname`

                    rm public-hostname

                    echo "Binary [$TAR] is packaged and ready for download" 
					echo "[sftp -i /root/.ssh/aws-fhwm-net.pem $DNS:/usr/local/watchman-alerting/tar/$TAR]"
                fi
            else
                exit
            fi
        fi
    fi
done
