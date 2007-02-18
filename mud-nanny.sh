#!/bin/sh
# MUD Nanny
# Sean Middleditch / AwesomePlay Productions, Inc.
# elanthis@awemud.net
# THIS IS PUBLIC DOMAIN

ADMIN='root@localhost'

# Requires that the 'mail' command is in the PATH.

# Exit status controls MUD Nanny:
# 0 - restart MUD immediately
# 1 - shutdown MUD Nanny
# 3 - wait 30 seconds, then restart MUD
# 5 - wait 5 minutes, then restart MUD
# 6 - wait 1 hour, then restart MUD
# Any other signal will cause MUD Nanny to shutdown with an error.
# The wait codes are implemented with sleep(), so any non-fatal
# signal sent to MUD Nanny will cause it to immediately restart
# the MUD.

# Default behavior is to restart the MUD, after some wait.  An
# exit code of 0 from the MUD will indicate a desired shutdown of
# the MUD, and the script will return 1 and shutdown MUD Nanny.
# For other exit codes or signal shutdowns, the MUD will be
# restarted immediately if its uptime was at least 30 seconds;
# otherwise, the server will wait 1 hour, as it assumes that
# the MUD is always crashing on startup.

EXIT="$1"
CODE="$2"
TIME="$3"
PATH="$4"

case "$EXIT" in
	'exit')
		[ '$CODE' = 0 ] && exit 1
		if [ "$TIME" -lt 30 ] ; then
			mail -s "MUD Nanny [$PATH]" "$ADMIN" <<-EOF
				MUD Nanny
				
				$PATH
				Exit code: $CODE
				Uptime: $TIME seconds

				Restarting in one hour.
			EOF
			exit 6
		else
			mail -s "MUD Nanny [$PATH]" "$ADMIN" <<-EOF
				MUD Nanny
				
				$PATH
				Exit code: $CODE
				Uptime: $TIME seconds

				Restarting immediately.
			EOF
			exit 0
		fi
		;;
	'signal')
		if [ "$TIME" -lt 30 ] ; then
			mail -s "MUD Nanny [$PATH]" "$ADMIN" <<-EOF
				MUD Nanny
				
				$PATH
				Signal: $CODE
				Uptime: $TIME seconds

				Restarting in one hour.
			EOF
			exit 6
		else
			mail -s "MUD Nanny [$PATH]" "$ADMIN" <<-EOF
				MUD Nanny
				
				$PATH
				Signal: $CODE
				Uptime: $TIME seconds

				Restarting immediately.
			EOF
			exit 0
		fi
		;;
	*)
		mail -s "MUD Nanny [$PATH]" "$ADMIN" <<-EOF
			MUD Nanny
			
			$PATH
			Cause of death: unknown
			Uptime: $TIME seconds

			Restarting in one hour.
		EOF
		exit 6
		;;
esac
