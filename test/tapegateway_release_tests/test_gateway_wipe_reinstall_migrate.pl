#!/usr/bin/perl -w
###############################################################################
#      test/tapegateway_release_tests/test_gateway_wipe_reinstall_migrate.pl
# 
#  This file is part of the Castor project.
#  See http://castor.web.cern.ch/castor
# 
#  Copyright (C) 2003  CERN
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
# 
# 
#  @author Steven.Murray@cern.ch, Eric.Cano@cern.ch and castor tape team.
###############################################################################

# This perl script that has to be run as root on the stager host will connect to
# the DB and report wether there are any pending mogrations for this stager
# On first version, it will report to both standard output and as an exit value.

# Imports
use strict;
use POSIX;
use DBI;
use DBD::Oracle qw(:ora_types);
use CastorTapeTests;

sub goodDaySingleAndDualCopyTest ( $$ );
sub badDayTests ( $$ );
sub preparePreTransitionBacklog ( $$ );
sub managePostTransitionBacklog ( $ );
sub main ();

# Make sure at least Ctrl-C triggers a cleanup
sub sigint()
{
    warn "Interrupt caught. Exiting.";
    exit (1);
}

$SIG{INT} =  "sigint";
$SIG{TERM} = "sigint";

# Hook for main.
main();

# Main: check for leftovers in DB, print out, return exit code accordingly
sub main ()
{
    # Initialize: setup the environment
    print "t=".CastorTapeTests::elapsed_time()."s. ";
    print `date`;
    my $conffile = './tapetests-lxcastordev.conf';
    CastorTapeTests::read_config($conffile);
    CastorTapeTests::check_environment ();
    my $file_size = CastorTapeTests::get_environment('file_size');
    my $file_number = CastorTapeTests::get_environment('file_number');
    my $castor_directory = CastorTapeTests::get_environment('castor_directory');
    my $single_subdir = CastorTapeTests::get_environment('castor_single_subdirectory');
    my $dual_subdir = CastorTapeTests::get_environment('castor_dual_subdirectory');
    my $username = CastorTapeTests::get_environment('username');
    
    
    my $dbh = CastorTapeTests::open_db();
    my $ret = 0;
    if (CastorTapeTests::check_leftovers ( $dbh )) {
        my $userless; # workaround to prevent emacs from fscking the indentation.
        #CastorTapeTests::print_leftovers ( $dbh );
        $dbh->disconnect();
        die "FATAL: Leftovers found in the database. Aborting test.";
    }
    $dbh->disconnect();
    # Nuke and start clean
    my $host = `hostname -s`; chomp $host;
    if ( ($host ne 'lxcastordev03') && ($host ne 'lxcastordev04')) {
        die("ABORT: Unexpected host \"$host\"");
    }
    print "t=".CastorTapeTests::elapsed_time."s. ";
    print "Wiping the DB for a run of rtcpclientd with classic schema  =============\n";
    CastorTapeTests::reinstall_stager_db();
   
    print "t=".CastorTapeTests::elapsed_time."s\n";
    my $seed_index = CastorTapeTests::make_seed ($file_size);  
    
    # On first run, clean house
    print "Cleaning up test directories $castor_directory\{$single_subdir,$dual_subdir\}\n";
    print `su $username -c "for p in $castor_directory\{$single_subdir,$dual_subdir\}; do nsrm -r -f \\\$p; done"`;

    # Re-create the directories:
    print `su $username -c "nsmkdir $castor_directory$single_subdir"`;
    print `su $username -c "nschclass largeuser $castor_directory$single_subdir"`;
    CastorTapeTests::register_remote ( $castor_directory.$single_subdir, "directory" );
    print `su $username -c "nsmkdir $castor_directory$dual_subdir"`;
    print `su $username -c "nschclass test2 $castor_directory$dual_subdir"`;
    CastorTapeTests::register_remote ( $castor_directory.$dual_subdir, "directory" );

    # Let the dust settle
    print "t=".CastorTapeTests::elapsed_time."s\n";
    CastorTapeTests::poll_fileserver_readyness (5,60);
        
    # Migrate to the new system, with tape gateway, still running rtcpclientd
    $dbh=CastorTapeTests::open_db();
    CastorTapeTests::migrateToNewTapeGatewaySchema ();
    CastorTapeTests::stopAndSwitchToRtcpclientd ( $dbh );
    CastorTapeTests::startDaemons();
    print "t=".CastorTapeTests::elapsed_time."s. ";
    print "Switched to new schema with rtcpclientd =============\n";
    
    # First iteration of the test
    SingleAndDualCopyTest ( $dbh, $seed_index, $file_number);
    preparePreTransitionBacklog ( $seed_index, $file_number);
     
    # Switch to tape gateway
    CastorTapeTests::stopAndSwitchToTapeGatewayd ( $dbh );
    CastorTapeTests::startDaemons();
    print "t=".CastorTapeTests::elapsed_time."s. ";
    print "Switched to tapegatewayd ============================\n";

    # Second iteration of the test
    managePostTransitionBacklog( $dbh );
    SingleAndDualCopyTest ( $dbh, $seed_index, $file_number);    
    preparePreTransitionBacklog ($seed_index, $file_number);

    # Switch back to tape gateway.
    CastorTapeTests::stopAndSwitchToRtcpclientd ( $dbh );
    CastorTapeTests::startDaemons();
    print "t=".CastorTapeTests::elapsed_time."s. ";
    print "Switched back to rtcpclientd  ========================\n";

    # Fire 3rd iteration of the test
    managePostTransitionBacklog( $dbh );
    SingleAndDualCopyTest ( $dbh, $seed_index, $file_number);

    print "Cleaning up test directories $castor_directory\{$single_subdir,$dual_subdir\}\n";
    print `su $username -c "for p in $castor_directory\{$single_subdir,$dual_subdir\}; do nsrm -r -f \\\$p; done"`;
    exit 0;
}

sub goodDayFileCreation ( $$ )
{
    my ( $seed_index, $file_number ) = ( shift, shift);
    my $castor_directory = CastorTapeTests::get_environment('castor_directory');
    my $single_subdir = CastorTapeTests::get_environment('castor_single_subdirectory');
    my $dual_subdir = CastorTapeTests::get_environment('castor_dual_subdirectory');
    my $username = CastorTapeTests::get_environment('username');
    my $poll = CastorTapeTests::get_environment('poll_interval');
    my $timeout = CastorTapeTests::get_environment('migration_timeout');

    for my $sd (0, 1) {
        for my $i (0 .. ($file_number - 1) ) {
            my $file_name="/tmp/".`uuidgen`;
            chomp $file_name;
            my $local_index = CastorTapeTests::make_localfile( $seed_index, $file_name );
            CastorTapeTests::rfcp_localfile ( $local_index, $sd );
        }
    } 
}

# Inject error conditions to chekc on the reactions of the tape system
sub badDayFileCreation ( $$$ )
{
    my ( $dbh, $seed_index, $file_number ) = ( shift, shift, shift);
    my $error_index = 0;
    my @error_list = ( "missing ns entry on partial migration" );
    my $castor_directory = CastorTapeTests::get_environment('castor_directory');
    my $single_subdir = CastorTapeTests::get_environment('castor_single_subdirectory');
    my $dual_subdir = CastorTapeTests::get_environment('castor_dual_subdirectory');
    my $username = CastorTapeTests::get_environment('username');
    my $poll = CastorTapeTests::get_environment('poll_interval');
    my $timeout = CastorTapeTests::get_environment('migration_timeout');

    for my $sd (0, 1) {
        for my $i (0 .. ($file_number - 1) ) {
            my $file_name="/tmp/".`uuidgen`;
            chomp $file_name;
            my $local_index = CastorTapeTests::make_localfile( $seed_index, $file_name );
            CastorTapeTests::rfcp_localfile_break ( $dbh, $local_index, $sd, $error_list[$error_index] );
	    $error_index = $error_index + 1 % scalar(@error_list);
        }
    } 
}

# Inject single and dual tape copies files, wait for them to be migrated,
# stager_rm them and then recall them. Eventually, drop them.
sub SingleAndDualCopyTest ( $$$ )
{
    my ( $dbh, $seed_index, $file_number ) = ( shift, shift, shift );
    my $poll = CastorTapeTests::get_environment('poll_interval');
    my $timeout = CastorTapeTests::get_environment('migration_timeout');
    goodDayFileCreation ( $seed_index, $file_number );
    badDayFileCreation ( $dbh,  $seed_index, $file_number );
    CastorTapeTests::poll_moving_entries ( $dbh, $poll, $timeout, "cleanup_migrated stager_reget_from_tape" );
}



# Inject file right before transition from one running configuration to the other.
sub preparePreTransitionBacklog ( $$ )
{
    my ( $seed_index, $file_number ) = ( shift, shift);
    # For the moment, reproduce the good day without the polling.
    goodDayFileCreation ( $seed_index, $file_number );
}

# Follow up on the files injected in the system after the configuration switchover.
sub managePostTransitionBacklog ( $ )
{
    my $dbh = shift;

    # Check that the tapes from the tape pool have been left in a proper state.
    my $tapepool = CastorTapeTests::get_environment('tapepool');
    my $tp_status = `vmgrlisttape -P $tapepool`;
    for ( split /^/, $tp_status) {
	if ( /BUSY/ ) {
	    chomp;
	    print "********** WARNING: ".$_."\n";
	    if ( /^([[:alnum:]]+) / ) {
		`vmgrmodifytape -V $1 --st \"\"`;
		print "Removed the busy flag from tape $1\n";
	    }
	}
    }
    my $poll = CastorTapeTests::get_environment('poll_interval');
    my $timeout = CastorTapeTests::get_environment('migration_timeout');    
    CastorTapeTests::poll_moving_entries ( $dbh, $poll, $timeout, "cleanup_migrated stager_reget_from_tape" );
}


