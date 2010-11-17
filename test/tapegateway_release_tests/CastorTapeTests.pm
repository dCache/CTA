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
#  @author Steven.Murray@cern.ch, Eric.Cano@cern.ch and Castor tape-team.
###############################################################################

# CastorTapeTests package
#
# Not in a directory structure yet...
# Intended to contain all the utilities to run tape tests.

package CastorTapeTests;
my $package_name="CastorTapeTests";

# Imports
use strict;
use POSIX;
use DBI;
use DBD::Oracle qw(:ora_types);

our $VERSION = 1.00;
our @ISA = qw(Exporter);

our @export   = qw(
                   get_uid_for_username
                   get_gid_for_username
                   get_disk_servers
                   read_config
                   check_environment
                   get_environment
                   register_remote_file
                   make_seed 
                   make_localfile 
                   open_db 
                   getConfParam
                   getCastorConfParam 
                   daemonIsRunning 
                   killDaemonWithTimeout
                   startDaemons
                   genBackupFileName 
                   getNbTapeCopiesInStagerDb 
                   insertCastorFile 
                   deleteCastorFile 
                   setFileClassNbCopies 
                   overrideCheckPermission 
                   insertTapeCopy 
                   deleteTapeCopy 
                   deleteAllStreamsTapeCopiesAndCastorFiles 
                   executeSQLPlusScript 
                   executeSQLPlusScriptNoError 
                   migrateToNewTapeGatewaySchema 
                   stopAndSwitchToTapeGatewayd 
                   stopAndSwitchToRtcpclientd 
                   getLdLibraryPathFromSrcPath
                   check_leftovers
                   check_leftovers_poll_timeout
                   nullize_arrays_undefs
                   print_leftovers
                   reinstall_stager_db
                   getOrastagerconfigParam
                   cleanup
		   elapsed_time
                   ); # Symbols to autoexport (:DEFAULT tag)

# keep track of the locally created files and of the recalled and locally rfcped files.
my @local_files;

# keep track of the files migrated in castor.
my @remote_files;

# keep track of the test environment (and definition of the defaults).
my %environment = (
    allowed_stagers => [ 'lxcastordev03', 'lxcastordev04' ]
);

my $time_reference;


# Returns the numeric user ID of the user with given name
sub get_uid_for_username( $ )
{
  my $username = shift;

  my $id_result=`id $username`;
  chomp($id_result);

  if($id_result =~ m/uid=(\d+)/) {
    return $1;
  } else {
    die("Failed to get uid for username \"$username\"\n");
  }
}


# Returns the numeric group ID of the user with given name
sub get_gid_for_username( $ )
{
  my $username = shift;

  my $id_result=`id $username`;
  chomp($id_result);

  if($id_result =~ m/gid=(\d+)/) {
    return $1;
  } else {
    die("Failed to get uid for username \"$username\"\n");
  }
}


# Based on the output of bhosts, this subroutine returns the hostnames of the
# disk-servers sorted alphabetically.
sub get_disk_servers()
{
  my @servers = `bhosts | egrep -v 'HOST_NAME|closed' | awk '{print \$1;}' | sort`;
  chomp(@servers);

  return @servers;
}

# Elapsed time since first call which implicitly sets the reference moment
sub elapsed_time ()
{
    if ( defined $time_reference ) {
	return time() - $time_reference;
    } else {
	$time_reference = time();
	chomp $time_reference;
	return 0;
    }
}

# Set the user name in the environment
sub read_config ( $ )
{   
    my $config_file = shift;
    my $local_host = `hostname -s`;
    chomp $local_host;
    $environment{hostname} = $local_host;
    my @per_host_vars = ( 'username', 'checkout_location', 'file_size', 'file_number', 'castor_directory',
                          'migration_timeout', 'poll_interval', 'tapepool', 'svcclass', 
			  'local_jobmanagerd',
			  'local_mighunterd',
			  'local_rechandlerd',
			  'local_rhd',
			  'local_rmmasterd',
			  'local_rtcpclientd',
			  'local_stagerd',
			  'local_tapegatewayd' );
    my @global_vars   = ( 'dbDir' , 'tstDir', 'adminList', 'originalDbSchema',
                          'originalDropSchema', 'castor_single_subdirectory',
                          'castor_dual_subdirectory' );
    for my $i ( @per_host_vars ) {
        $environment{$i}=getConfParam('TAPETEST', $i.'_'.$local_host, $config_file );
    }
    for my $i ( @global_vars ) {
        $environment{$i}=getConfParam('TAPETEST', $i, $config_file );
    }
}

# Dies with an appropriate message if the specfied file or directory does not
# exist
sub check_file_exists ( $$ )
{
    my ($descriptive_name, $filename) = (shift, shift);

    die("$descriptive_name \"$filename\" does not exist\n") if ! -e $filename;
}

# Check the sanity of the environment, die if anything is wrong
sub check_environment ( ) 
{
    my $pass = 0;
    for my $i ( @{$environment{allowed_stagers}} ) {
        if ( $environment{hostname} eq $i ) {
            $pass = 1;
        }
    }
    if ( !$pass ) { die "Wrong host"; }

    check_file_exists("Database schema directory", $environment{checkout_location}.'/'.$environment{dbDir});
    check_file_exists("Stager schema", $environment{checkout_location}.'/'.$environment{dbDir}.'/'.$environment{originalDbSchema});
    check_file_exists("Drop stager-database script", $environment{checkout_location}.'/'.$environment{dbDir}.'/'.$environment{originalDropSchema});
    check_file_exists("Test-script directory", $environment{checkout_location}.'/'.$environment{tstDir});
}

# Extract enviroment variable (mainly for client application)
sub get_environment ( $ )
{
    my $vname = shift;
    if ( defined $environment{$vname} ) {
        return $environment{$vname}
    } else {
        die "Variable $vname not found in environment";
    }
}

# Register remote file
# keep track of a remote file that can be subsequently checked, modified, 
# sabotaged (to test robustness) and eventually dropped (also automatically)
# here, we just record the name
sub register_remote ( $$ )
{
    my ( $filename, $type ) =  ( shift, shift );
    if ( ! defined $type ) { $type = "file"; } 
    my %file_record = ( 'name' => $filename, 'type' => $type );
    my $file_index = push @remote_files,  \%file_record;
    return $file_index - 1;
}

# Check remote monitor has found anything
sub poll_rm_readyness ( $$ )
{
    my ( $interval, $timeout ) = ( shift, shift );
    my $start_time = time();
    while ( ($start_time + $timeout) >= time()) {
        my $query=`rmGetNodes`;
        if ( $query =~ /name:/ ) {
	    print "RM ready after ".(time() - $start_time)."s.\n";
            return;
        }
	sleep ( $interval );
    }
    die "Timeout in poll_rm_readyness";
 
}

# Check fileservers ready with timeout
sub poll_fileserver_readyness ( $$ )
{
    my ( $interval, $timeout ) = ( shift, shift );
    my $start_time = time();
    while ( ($start_time + $timeout) >= time()) {
        my $query=`su $environment{username} -c \"stager_qry -s\"`;
        if ( $query =~ /FILESYSTEM_PRODUCTION/ ) {
	    print "Fileservers ready after ".(time() - $start_time)."s.\n";
            return;
        }
	sleep ( $interval );
    }
    die "Timeout in poll_fileserver_readyness";
}

# Check that lsf has accessed the shared memory
sub poll_lsf_is_mmaped  ( $$ )
{
    my ( $interval, $timeout ) = ( shift, shift );
    my $start_time = time();
    while ( ($start_time + $timeout) >= time()) {
        my $query=`ps ax | grep mbschd | cut -c 1-5 | xargs pmap`;
        if ( $query =~ /shmid/ ) {
	    print "Lsf accessing the shared memory ready after ".(time() - $start_time)."s.\n";
            return;
        }
	sleep ( $interval );
    }
    die "Timeout in poll_lsf_is_mmaped";
}


# create a local seed file, returning the index to the file.
# Take 1 parameter: the size in 
sub make_seed ( $ )
{
    my $size = shift;
    ((defined $size)  || ($size < 0)) or die "In $package_name::make_seed: size not defined";
    my $file_name = `mktemp`;
    chomp $file_name;
    my $megs = int ($size/(1024*1024));
    my $kilos = int (($size%(1024*1024))/1024);
    my $bytes = $size % 1024;
    my $kflags="";
    my $bflags="";
    
    print "t=".elapsed_time()."s. Creating seed file $file_name\n";
    my $starttime = time();
    if ($megs != 0) {
	`dd if=/dev/urandom of=$file_name bs=1M count=$megs 2>&1`;
	$kflags=" oflag=append conv=notrunc";
	$bflags=" oflag=append conv=notrunc";
    }
    if ($kilos != 0) {
	`dd if=/dev/urandom of=$file_name bs=1K count=$kilos $kflags 2>&1`;
	$bflags=" oflag=append conv=notrunc";
    }
    if ($bytes != 0) {
	`dd if=/dev/urandom of=$file_name bs=1 count=$bytes $bflags 2>&1`;
    }
    
    # Check that everything went fine
    if (!-e $file_name) {
	die "In $package_name::make_seed: file $file_name not created";
    }
    if (! (-s $file_name) == $size) {
	die "In $package_name::make_seed: file $file_name created with wrong size";
    }
    my $endtime = time();
    print "t=".elapsed_time()."s. Seed file created in ".($endtime - $starttime)."s. Speed=".
	($size/(1024*1024)/($endtime - $starttime))."MB/s.\n";
    
    # Hand over the file to the user
    print `chown $environment{'username'} $file_name`;
    
    # We call it good enough
    my $checksum = `adler32 $file_name 2>&1`;
    chomp ($checksum);
    my %file_entry = ( 'name' => $file_name, 
		       'size' => $size,
                     'adler32' => $checksum);
    
    my $file_index = push @local_files, \%file_entry;
    $file_index --;
    return $file_index;
}


# Create a local file from seed, with its name prepended, and recompute the checksum
# return index to the file in the local files array.
#
sub make_localfile ( $$ )
{
    my ($seed_index, $file_name) = ( shift, shift );
    open NEW_FILE, "> $file_name" or die "In $package_name::make_localfile: failed to open $file_name for writing: $!";
    print NEW_FILE "$file_name";
    close NEW_FILE;
    `dd if=$local_files[$seed_index]->{'name'} of=$file_name bs=1M oflag=append conv=notrunc 2>&1`;
    my $checksum = `adler32 $file_name 2>&1`;
    chomp ($checksum);
    my $size =  ( -s $file_name );
    my %file_entry = ( 'name' => $file_name, 
		       'size' => $size,
		       'adler32' => $checksum);
    my $file_index = push @local_files, \%file_entry;
    $file_index --;
    print "t=".elapsed_time()."s. Created local file $file_name\n";
    return $file_index;
}


# Utility functions for polling states of files.

# Returns true if the file is of size zero or migrated (m bit), false otherwise
# Dies on the first problem.
sub check_migrated_in_ns ( $ )
{
    my $file_name = shift;
    my $nsls=`nsls -l $file_name`;
    if ($nsls=~ /^([\w-]+)\s+(\d+)\s+(\w+)\s+(\w+)\s+(\d+)/ ) {
        return ( ('m' eq substr ($1,0,1)) || ($5 == 0) );
    } else {
        die "Failed to find file $file_name";
    }
}

# Returns true if the file is declared as invalid in the stager
sub check_invalid ( $ )
{
    my $file_name = shift;
    my $stager_qry=`su $environment{username} -c \"stager_qry -M $file_name\"`;
    return $stager_qry=~ /INVALID/;
}

# Returns true if the file is recalled
sub check_recalled_or_fully_migrated ( $ )
{
    my $file_name = shift;
    my $stager_qry=`su $environment{username} -c \"stager_qry -M $file_name\"`;
    return $stager_qry=~ /STAGED/;
}


# remote files statuses:
# => rfcped -> partially migrated -> migrated => invalidation requested -> on tape => being recalled -> recalled
# 
# all self-transtions (->) are followed up in the same loop, with corresponding checks...
# all test-program initiated transitions (=>) are initiated from a different function...
# rfcped is created by rfcp_localfile
# invalidation requested is generated by cleanup_migrated
# being recalled is generated by stager_reget_from_tape.


# rfcp file to either single or dual copy directory and push all the
# know properties of the local file to the remote file created for the occasion
sub rfcp_localfile ( $$ )
{
    my ( $local_index, $is_dual_copy ) = ( shift, shift );
    my $dest = $environment{castor_directory};
    if ( $is_dual_copy ) { 
        $dest .= $environment{castor_dual_subdirectory};
    } else {
        $dest .= $environment{castor_single_subdirectory};
    }
    my $local = $local_files[$local_index]->{name};
    my $remote = $dest;
    if ( $local =~ /\/([^\/]*)$/ ) {
        $remote .= "/".$1;
    } else {
        die "Wrong file path in rfcp_localfile";
    }
    my $rfcp_ret=`su $environment{username} -c \"STAGE_SVCCLASS=$environment{svcclass} rfcp $local $remote\" 2>&1`;
    my %remote_entry = ( 'name' => $remote, 
                         'type' => "file",
                         'size' => $local_files[$local_index]->{size},
                         'adler32' => $local_files[$local_index]->{adler32},
                         'status' => 'rfcped' );
    push @remote_files, \%remote_entry;
    print "t=".elapsed_time()."s. rtcp'ed $local => $remote:\n";
    for ( split /^/, $rfcp_ret ) {
	if ( ! /bytes in/ ) {
	    print $_."\n";
	}
    }
}

# Remove staged files from the stager so they can be recalled.
sub cleanup_migrated ()
{
    for ( my $i =0; $i < scalar (@remote_files); $i++ ) {
	my %f = %{$remote_files[$i]};
	if ( $f{type} eq "file" && $f{status} eq "migrated" ) {
	    `su $environment{username} -c \"STAGE_SVCCLASS=$environment{svcclass} stager_rm -M $f{name}\"`;
	    $remote_files[$i]->{status} = "invalidation requested";
	    print "t=".elapsed_time()."s. Removed $f{name} (was migrated) from stager.\n";
	}
    }
}

# Stager_get file
sub stager_reget_from_tape ()
{
    for ( my $i =0; $i < scalar (@remote_files); $i++ ) {
	my %f = %{$remote_files[$i]};
	if ( $f{type} eq "file" && $f{status} eq "on tape" ) {
	    `su $environment{username} -c \"STAGE_SVCCLASS=$environment{svcclass} stager_get -M $f{name}\"`;
	    $remote_files[$i]->{status} = "being recalled";
	    print "t=".elapsed_time()."s. Initiated recall for $f{name}.\n";
	}
    }    
}

# Check remote entries: check presence (should be always true) and then status of all files listed in remote files list
# returns true is any file has changed status, allowig a caller function to tiem out on "nothing moves anymore"
sub check_remote_entries ()
{
    my $dbh = shift;
    my $changed_entries = 0;
    for  my $i ( 0 .. scalar (@remote_files) - 1 ) {
        my %entry = %{$remote_files[$i]};
        # check presence
        my $nslsresult=`nsls -d $entry{name} 2>&1`;
        if ( $nslsresult =~ /No such file or directory$/) {
            die "Entry not found in name server: $entry{name}";
        }
	undef $nslsresult;
        # if it's a file we can do something. 
        if ( $entry{type} eq "file" ) {
	    # check the migration status and compare checksums.
            if ( $entry{status} eq "rfcped" ) {
                if ( check_migrated_in_ns ( $entry{name} ) ) {
		    # Report the newly detected migration.
		    print "t=".elapsed_time()."s. File ".$entry{name}." now partially migrated to tape (has 'm' bit).\n";
                    $remote_files[$i]->{status} = "partially migrated";
		    # Validate the checksum.
		    my $remote_checksum_string = `su $environment{username} -c \"nsls --checksum $entry{name}\"`;
		    chomp $remote_checksum_string;
		    my ( $remote_checksum, $local_checksum );
		    if ( $remote_checksum_string =~ /AD\s+([[:xdigit:]]+)\s/ ) {
			$remote_checksum = $1;
		    } else {
			print "ERROR: Failed to interpret remote checksum: $remote_checksum_string.\n";
		    }
		    if ( $entry{adler32} =~ /adler32\(.*\) = \d+\, 0x([[:xdigit:]]+)/ ) {
			$local_checksum = $1;
		    } else {
			print "ERROR: Failed to interpret locally stored checksum: $entry{adler32}.\n";
		    }
		    if ( $local_checksum ne $remote_checksum ) {
			print "ERROR: checksum mismatch beween remote and locally stored for $entry{name}: $local_checksum != $remote_checksum\n";
		    }
                    $changed_entries ++;
                }
	    # check the number of tape copies after the full migration
	    } elsif ( $entry{status} eq "partially migrated" ) {
		if ( check_recalled_or_fully_migrated ( $entry{name} ) ) {		    
		    # Report the newly detected migration.
		    print "t=".elapsed_time()."s. File ".$entry{name}." now fully migrated to tape (from stager's point of view).\n";
		    $remote_files[$i]->{status} = "migrated";
		    # Check that dual tape copies got mirgated as expected (and on different tapes)
		    $nslsresult = `su $environment{username} -c \"nsls --class $entry{name}\"`;
		    if ( $nslsresult =~ /^\s+(\d+)\s+\/castor/ ) {
			my $class = $1;
			my $nslistclass_result = `su $environment{username} -c \"nslistclass --id=$class\"`;
			if ( $nslistclass_result =~ /NBCOPIES\s+(\d+)/ ) {
			    my $expected_copynb = $1;
			    my $found_copynb = `su $environment{username} -c \"nsls -T $entry{name} 2>/dev/null | wc -l\"`;
			    if ( $expected_copynb != $found_copynb ) {
				print "ERROR: Unexpected number of copies for $entry{name}. Expected: $expected_copynb, found:$found_copynb\n";
				print "stager record: ".`su $environment{username} -c \"stager_qry -M $entry{name}\"`;
				print "TODO: handle this state as partially migrated\n";
			    }
			} else {
			    print "ERROR: Failed to extract copynb for file class $class while processing file $entry{name}.\n";
			}
		    } else {
			print "ERROR: Failed to extract class for file ".$entry{name}."\n";
		    }
		    $changed_entries ++;
		}
	    # check the invalidation status
	    } elsif ($entry{status} eq "invalidation requested" ) {
		if ( check_invalid $entry{name} )  {
		    print "t=".elapsed_time()."s. File ".$entry{name}." reported as invalid by the stager.\n";
		    $remote_files[$i]->{status} = "on tape";
		    $changed_entries ++;
		}
	    # check the recall status, rfcp in and compare checksums.
            } elsif ($entry{status} eq "being recalled" ) {
		if ( check_recalled_or_fully_migrated ( $entry{name} ) ) {
		    print "t=".elapsed_time()."s. File ".$entry{name}." now recalled to disk.\n";
		    $remote_files[$i]->{status} = "recalled";
		    # rfcp in the file, check size and checksum.
		    my $local_copy=`mktemp`;
		    chomp $local_copy;
		    # hand over the file to the user
		    `chown $environment{username} $local_copy`;
		    # rfcp the recalled copy
		    `su $environment{username} -c \"STAGE_SVCCLASS=$environment{svcclass} rfcp $entry{name} $local_copy\"`;
		    # compute checksum, get size and get rid of file
		    my $local_size = ( -s $local_copy );
		    my $local_checksum_string = `adler32 $local_copy 2>&1`;
		    my $local_checksum;
		    if ( $local_checksum_string =~ /adler32\(.*\) = \d+\, 0x([[:xdigit:]]+)/ ) {
			$local_checksum = $1;
		    }
		    my $stored_checksum;
		    if ( $entry{adler32} =~ /adler32\(.*\) = \d+\, 0x([[:xdigit:]]+)/ ) {
			$stored_checksum = $1;
		    }
		    if ( $stored_checksum ne $local_checksum ) {
			print "ERROR: checksum mismatch beween locally recalled file and stored value for $entry{name}: ".
			    "$local_checksum != $stored_checksum\n";
		    }
		    if ( $local_size != $entry{size} ) {
			print "ERROR: size mismatch beween locally recalled file and stored value for $entry{name}: ".
			    "$local_size != ".$entry{size}."\n";
		    }
		    unlink $local_copy;
		    print "t=".elapsed_time()."s. rfcped ".$entry{name}." back and checked it.\n";
		    $changed_entries ++;
		}
	    }
	}
    }
    return $changed_entries;
}

# Get the number of files for which a move is expected.
sub count_to_be_moved ()
{
    my $ret = 0;
    for ( @remote_files ) {
        if ( ($_->{type} eq "file" ) && ($_ ->{status} =~ 
					 /rfcped|partially migrated|invalidation requested|being recalled/ ) ) {
            $ret++;
        }
    }
    return $ret;
}

# Check that all the files have migrated 
sub poll_moving_entries ( $$$ )
{
    my ( $poll_interval, $timeout, $options ) = ( shift, shift, shift );
    my $starttime = time();
    while ( count_to_be_moved() > 0 && ((time() - $starttime) < $timeout) ) {
        if ( check_remote_entries () ) {
            print "t=".elapsed_time()."s. Saw at least one new migration...\n";
            $starttime = time();
        }
	if ( $options =~ /cleanup_migrated/ ) {
	    cleanup_migrated();
	}
	if ( $options =~ /stager_reget_from_tape/ ) {
	    stager_reget_from_tape();
	}
        sleep ( $poll_interval );
    }
    if (count_to_be_moved() == 0 ) {
	print "t=".elapsed_time()."s. All expected moves completed.\n";
        return;
    }
    die "Timeout with ".count_to_be_moved()." files to be migrated after $timeout s.";
}


# Collection of special functions which introduce broken structures in the system 

# rfcp file, but give it a special traetement to asses the behaviour of the migration with bad/broken files
sub rfcp_localfile_break ( $$ )
{
    my $breaking_type = shift;
    die "TODO";
    if ($breaking_type eq "missing castorfile") {
	
    }
}

sub cleanup_migrated_break ()
{
    die "TODO";
}

sub stager_reget_from_tape_break ()
{
    die "TODO";
}

# Breaking can be introduced on the occasion of a transition detection as well.
sub check_remote_entries_break ()
{
    die "TODO";
}

# Returns the value of the specified ORASTAGERCONFIG parameter.
#
# This subroutine prints an error message and aborts the entire tes script if
# it fails to get the value of the specified parameter.
#
# @param  paramName The name of the parameter whose value is to be retrieved.
# @return           The value of the parameter.
sub getOrastagerconfigParam ( $ )
{
  my $paramName = $_[0];

  my $foundParamValue = 0;
  my $paramValue = "";

  open(CONFIG, "</etc/castor/ORASTAGERCONFIG")
    or die "Failed to open /etc/castor/ORASTAGERCONFIG: $!";

  while(<CONFIG>) {
    chomp;
    if(m/^DbCnvSvc\s+$paramName\s+(\w+)/) {
      $paramValue = $1;
      $foundParamValue = 1;
    }
  }

  close CONFIG;

  die("ABORT: Failed to get ORASTAGERCONFIG parameter: paramName=$paramName\n")
    if(! $foundParamValue);

  return $paramValue;
}

# open_db : find connection parameters and open db connection
sub open_db()
{
    my ( $u, $p, $db );
    open ORACFG, "< /etc/castor/ORASTAGERCONFIG" 
      or die "Failed ot open /etc/castor/ORASTAGERCONFIG for reading: $!";
    while (<ORACFG>) {
        if (/^DbCnvSvc\W+user\W+(\w+)$/) { 
          $u = $1; 
        } elsif (/^DbCnvSvc\W+passwd\W+(\w+)$/) { 
          $p = $1; 
        } elsif (/^DbCnvSvc\W+dbName\W+(\w+)$/) { 
          $db = $1; 
        } 
    }
    close ORACFG;
    my $dbh= DBI->connect('dbi:Oracle:'.$db ,$u, $p,
      { RaiseError => 1, AutoCommit => 0}) 
      or die "Failed to connect to DB as ".$u.'\@'.$db;
    return $dbh;
}


# Returns the value of the specified parameter from the specified file.
#
# This subroutine prints an error message and aborts the entire test script if
# it fails to get the value of the specified parameter.
#
# @param  paramCategory The catagory of the parameter whose value is to be
#                       retrieved.
# @param  paramName     The name of the parameter whose value is to be
#                       retrieved.
# @param  paramFile     The configuration file's name
# @return               The value of the parameter.
sub getConfParam ( $ $ $  )
{
    my $paramCategory = $_[0];
    my $paramName     = $_[1];
    my $confFilename  = $_[2];
    
    open(CONFIG, $confFilename)
        or die "Failed to open $confFilename: $!";
    
    while(<CONFIG>) {
        chomp;
        if(m/^$paramCategory\s+$paramName\s+([^\s]+$)/) {
            close CONFIG;
            return $1;
        }
    }
    close CONFIG;
    die("ABORT: Failed to get $confFilename parameter: category=$paramCategory, name=$paramName\n");
}

# Returns the value of the specified /etc/castor/castor.conf parameter.
#
# This subroutine prints an error message and aborts the entire test script if
# it fails to get the value of the specified parameter.
#
# @param  paramCategory The catagory of the parameter whose value is to be
#                       retrieved.
# @param  paramName     The name of the parameter whose value is to be
#                       retrieved.
# @return               The value of the parameter.
sub getCastorConfParam ( $ $ )
{
    my ( $category, $parameter ) = ( shift, shift );
    return getConfParam ( $category, $parameter, "/etc/castor/castor.conf");
}


# Returns true if the daemon with specfied name is running else false.
#
# @param  daemonName The name of the daemon.
# @return            True if the daemon is runn ing, else false.
sub daemonIsRunning ( $ )
{
  my $daemonName = $_[0];

  my $psResult = `ps -e | grep $daemonName`;
  chomp($psResult);

  return($psResult =~ m/^\s*\d+\s+\?\s+\d\d:\d\d:\d\d\s+$daemonName$/);
}


# Tries to kill the specified daemon within the specified time-out period.  If
# the daemon is dead within the time-out period, then this subroutine returns
# 0, else this subroutine prints an error message and returns a value of 1.
#
# If the daemon process to be killed is already dead when this subroutine is
# called, then this method will succeed immediately.
#
# @param  processName The process name of the daemon to be killed.
# @param  timeOutSecs The time-out period in seconds.
# @return             0 on success and 1 on failure.
sub killDaemonWithTimeout ( $$ )
{
  my $processName = $_[0];
  my $timeOutSecs = $_[1];

  `/sbin/service $processName stop`;

  if (daemonIsRunning($processName)) {
      sleep (1);
  } else {
      return 0;
  }

  if (daemonIsRunning($processName)) {
      `killall $processName`;
  } else {
      return 0;
  }

  if (daemonIsRunning($processName)) {
      sleep (1);
  } else {
      return 0;
  }
  if (daemonIsRunning($processName)) {
      `killall -9 $processName`;
  }

  my $startTime  = time();
  my $timeOutTime = $startTime + $timeOutSecs;

  while(1) {
    if(!&daemonIsRunning($processName)) {
      return 0; # Success
    }

    if(time() >= $timeOutTime) {
      die ("Failed to kill $processName\n");
      return 1; # Failure
    }
    sleep(1);
  }
}

# Start demons (starts tapegatewayd from the checkout directory
sub startDaemons ()
{
    my %castor_deamons_locations =
	(
	 'jobmanagerd' => './castor/jobmanager/jobmanagerd',
	 'mighunterd'  => './castor/tape/mighunter/mighunterd',
	 'rechandlerd' => './castor/tape/rechandler/rechandlerd',
	 'rhd'         => './castor/rh/rhd',
	 'rmmasterd'   => './castor/monitoring/rmmaster/rmmasterd',
	 'rtcpclientd' => './rtcopy/rtcpclientd',
	 'stagerd'     => './castor/stager/daemon/stagerd',
	 'tapegatewayd'=> './castor/tape/tapegateway/tapegatewayd'
	 );

    # Simply start all of them, demons not needed will jsut not start.
    for ( keys %castor_deamons_locations) {
        if ($environment{"local_".$_} =~ /True/) {
	    my $local_command = $castor_deamons_locations{$_};
            my $checkout_location=$environment{checkout_location};
            `( cd $checkout_location; LD_LIBRARY_PATH=\`find ./ -name "*.so*" | perl -p -e \'s|[^/]*\$|\n|\' | sort | uniq | tr \"\n\" \":\" | perl -p -e \'s/:\$/\n/\'\` $local_command )`;
	} else {
	    `service $_ start`;
	}        
    }
}

# Returns a back-up filename based on the specified original filename.
#
# Please note that this subroutine does not create or modify any files.
#
# The back-up filename is the original plus a unique extension.
#
# @param  originalFilename The full path-name of the original file.
# @return                  The full path-name of the back-up copy.
sub genBackupFileName ( $ ) 
{
  my $originalFilename = $_[0];

  my $uuid = `uuidgen`;
  chomp($uuid);
  my $date = `date | sed 's/ /_/g;s/:/_/g'`;
  chomp($date);
  my $hostname = `hostname`;
  chomp($hostname);
  my $backupFilename = "${originalFilename}_${uuid}_${hostname}_${date}_backup";

  return($backupFilename);
}



# Returns the number of tape-copies in the stager-database.
#
# @param dbh The handle to the stager-database.
# @return    The number of tape-copies in the stager database.
sub getNbTapeCopiesInStagerDb ( $ ) 
{
  my $dbh = $_[0];

  my $stmt = "SELECT COUNT(*) FROM TapeCopy";
  my $rows = $dbh->selectall_arrayref($stmt);
  my $nbTapeCopies = $$rows[0][0];

  return($nbTapeCopies);
}


# Inserts a row into the CastorFile table and returns its database ID.
#
# This subroutine does not insert a row into the id2type table.
#
# @param  dbh                The handle to the stager-database.
# @param  nsHost             The name-server hostname.
# @param  fileId
# @param  fileSize
# @param  creationTime
# @param  lastUpdateTime
# @param  lastAccessTime
# @param  lastKnownFilename
# @param  svcClassName
# @param  fileClassName
# @return The database ID of the newly inserted CastorFile.
sub insertCastorFile ( $$$$$$$$$$ )
{
  my $dbh               = $_[0];
  my $nsHost            = $_[1];
  my $fileId            = $_[2];
  my $fileSize          = $_[3];
  my $creationTime      = $_[4];
  my $lastUpdateTime    = $_[5];
  my $lastAccessTime    = $_[6];
  my $lastKnownFilename = $_[7];
  my $svcClassName      = $_[8];
  my $fileClassName     = $_[9];

  my $stmt = "
    DECLARE
      varSvcClassId   NUMBER(38) := NULL;
      varFileClassId  NUMBER(38) := NULL;
      varCastorFileId NUMBER(38) := NULL;
    BEGIN
      /* Deteremine the database IDs of the service and file classes */
      SELECT id INTO varSvcClassId  FROM SvcClass  WHERE name = :SVCCLASSNAME;
      SELECT id INTO varFileClassId FROM FileClass WHERE name = :FILECLASSNAME;

      INSERT INTO CastorFile(id, nsHost, fileId, fileSize, creationTime,
                  lastUpdateTime, lastAccessTime, lastKnownFilename, svcClass,
                  fileClass)
           VALUES (ids_seq.NEXTVAL, :NSHOST, :FILEID, :FILESIZE,
                  :CREATIONTIME, :LASTUPDATETIME, :LASTACCESSTIME,
                  :LASTKNOWNFILENAME, varSvcClassId, varFileClassId)
        RETURNING id INTO varCastorFileId;

      INSERT INTO Id2Type(id, type) VALUES(varCastorFileId, 2);

      :CASTORFILEID := varCastorFileId;
    END;";

  # The castor-file database ID will be the return value
  my $castorFileId;

  my $sth = $dbh->prepare($stmt);
  $sth->bind_param_inout(":SVCCLASSNAME",\$svcClassName,2048)
    or die $sth->errstr;
  $sth->bind_param_inout(":FILECLASSNAME",\$fileClassName,2048)
    or die $sth->errstr;
  $sth->bind_param_inout(":NSHOST",\$nsHost,2048) or die $sth->errstr;
  $sth->bind_param_inout(":FILEID",\$fileId,20) or die $sth->errstr;
  $sth->bind_param_inout(":FILESIZE",\$fileSize,20) or die $sth->errstr;
  $sth->bind_param_inout(":CREATIONTIME",\$creationTime,20) or die $sth->errstr;
  $sth->bind_param_inout(":LASTUPDATETIME",\$lastUpdateTime,20)
    or die $sth->errstr;
  $sth->bind_param_inout(":LASTACCESSTIME",\$lastAccessTime,20)
    or die $sth->errstr;
  $sth->bind_param_inout(":LASTKNOWNFILENAME",\$lastKnownFilename,2048)
    or die $sth->errstr;
  $sth->bind_param_inout(":CASTORFILEID",\$castorFileId,20) or die $sth->errstr;
  $sth->execute();

  return $castorFileId;
}


# Deletes the row from the CastorFile table with the specified database ID.
#
# @param dbh          The handle to the stager-database.
# @param castorFileId The castor database ID.
sub deleteCastorFile ( $$ )
{
  my $dbh          = $_[0];
  my $castorFileId = $_[1];

  my $stmt = "
    DECLARE
      varCastorFileId NUMBER(38) := :CASTORFILEID;
    BEGIN
      DELETE FROM CastorFile WHERE id = varCastorFileId;
      DELETE FROM Id2Type    WHERE id = varCastorFileId;
    END;";

  my $sth = $dbh->prepare($stmt);
  $sth->bind_param_inout(":CASTORFILEID", \$castorFileId, 20) or die $sth->errstr;
  $sth->execute();
}


# Sets the nbCopies attribute of the specified file-class to the specified
# value.
#
# @param dbh           The handle to the stager-database.
# @param fileClassName The name of the file-class.
# @param nbCopies      The new number of tape-copies.
sub setFileClassNbCopies ( $$$ )
{
  my $dbh           = $_[0];
  my $fileClassName = $_[1];
  my $nbCopies      = $_[2];

  my $stmt = "
    BEGIN
      UPDATE FileClass
         SET nbCopies = :NBCOPIES
       WHERE name = :FILECLASSNAME;
    END;";

  my $sth = $dbh->prepare($stmt);
  $sth->bind_param_inout(":NBCOPIES", \$nbCopies, 20)
    or die $sth->errstr;
  $sth->bind_param_inout(":FILECLASSNAME", \$fileClassName, 2048)
    or die $sth->errstr;
  $sth->execute();
}


# Override checkPermission
#
# @param dbh           The handle to the stager-database.
sub overrideCheckPermission ( $ )
{
  my $dbh = $_[0];

  my $stmt = "
    CREATE OR REPLACE FUNCTION checkPermission(
      reqSvcClass IN VARCHAR2,
      reqEuid     IN NUMBER,
      reqEgid     IN NUMBER,
      reqTypeI    IN NUMBER)
    RETURN NUMBER AS
    BEGIN
      RETURN 0;
    END;";

  my $sth = $dbh->prepare($stmt);
  $sth->execute();
}


# Inserts a row into the TapeCopy table and returns the database ID.
#
# @param dbh          The handle to the stager-database.
# @param castorFileId The database ID of the associated castor-file.
# @param status       The initial status of the tape-copy.
# @return             The database ID of the newly inserted tape-copy.
sub insertTapeCopy ( $$$ )
{
  my $dbh          = $_[0];
  my $castorFileId = $_[1];
  my $status       = $_[2];

  my $stmt = "
    DECLARE
      varTapeCopyId NUMBER(38) := NULL;
    BEGIN
      INSERT INTO TapeCopy(id, copyNb, castorFile, status)
           VALUES (ids_seq.nextval, 1, :CASTORFILEID, :STATUS)
        RETURNING id INTO varTapeCopyId;

      INSERT INTO Id2Type(id, type) VALUES (varTapeCopyId, 30);

      :TAPECOPYID := varTapeCopyId;
    END;";

  # The tape-copy database ID will be the return value
  my $tapeCopyId;

  my $sth = $dbh->prepare($stmt);
  $sth->bind_param_inout(":CASTORFILEID", \$castorFileId, 20) or die $sth->errstr;
  $sth->bind_param_inout(":STATUS"      , \$status      , 20) or die $sth->errstr;
  $sth->bind_param_inout(":TAPECOPYID"  , \$tapeCopyId  , 20) or die $sth->errstr;
  $sth->execute();

  return $tapeCopyId;
}


# Inserts the specified number of rows into the TapeCopy table all pointing to
# the specified castor-file and all with the specified status.
#
# @param dbh          The handle to the stager-database.
# @param castorFileId The database ID of the associated castor-file.
# @param status       The initial status of the tape-copies.
# @param nbTapeCopies The number of rows to insert into the TapeCopy table.
sub insertTapeCopies ( $$$$ )
{
  my $dbh          = $_[0];
  my $castorFileId = $_[1];
  my $status       = $_[2];
  my $nbTapeCopies = $_[3];

  my $stmt = "
    DECLARE
      varTapeCopyId NUMBER(38) := NULL;
    BEGIN
      FOR i IN 1 .. :NBTAPECOPIES LOOP
        INSERT INTO TapeCopy(id, copyNb, castorFile, status)
             VALUES (ids_seq.nextval, 1, :CASTORFILEID, :STATUS)
          RETURNING id INTO varTapeCopyId;

        INSERT INTO Id2Type(id, type) VALUES (varTapeCopyId, 30);
      END LOOP;
    END;";

  # The tape-copy database ID will be the return value
  my $tapeCopyId;

  my $sth = $dbh->prepare($stmt);
  $sth->bind_param_inout(":NBTAPECOPIES", \$nbTapeCopies, 20)
    or die $sth->errstr;
  $sth->bind_param_inout(":CASTORFILEID", \$castorFileId, 20)
    or die $sth->errstr;
  $sth->bind_param_inout(":STATUS"      , \$status      , 20)
    or die $sth->errstr;
  $sth->execute();

  return $tapeCopyId;
}


# Deletes the row from the TapeCopy table with the specified database ID.
#
# @param dbh        The handle to the stager-database.
# @param tapeCopyId The castor database ID.
sub deleteTapeCopy ( $$ )
{
  my $dbh        = $_[0];
  my $tapeCopyId = $_[1];

  my $stmt = "
    DECLARE
      varTapeCopyId NUMBER(38) := :TAPECOPYID;
    BEGIN
      DELETE FROM Stream2TapeCopy WHERE child = varTapeCopyId;
      DELETE FROM TapeCopy        WHERE id    = varTapeCopyId;
      DELETE FROM Id2Type         WHERE id    = varTapeCopyId;
    END;";

  my $sth = $dbh->prepare($stmt);
  $sth->bind_param_inout(":TAPECOPYID", \$tapeCopyId, 20) or die $sth->errstr;
  $sth->execute();
}


# Deletes all Stream2TapeCopy, Stream, TapeCopy and CastorFile rows from the
# stager database.
#
# TO BE USED WITH CAUTION.
#
# @param dbh The handle to the stager-database.
sub deleteAllStreamsTapeCopiesAndCastorFiles ( $ )
{
  my $dbh        = $_[0];

  my $stmt = "
    BEGIN
      DELETE FROM Stream2TapeCopy;
      DELETE FROM Stream;
      DELETE FROM TapeCopy;
      DELETE FROM CastorFile;
      DELETE FROM Id2Type WHERE type = 26; /* OBJ_STREAM     */
      DELETE FROM Id2Type WHERE type = 30; /* OBJ_TAPECOPY   */
      DELETE FROM Id2Type WHERE type =  2; /* OBJ_CASTORFILE */
    END;";

  my $sth = $dbh->prepare($stmt);
  print("Deleting all streams, tape-copies and castor-files\n");
  $sth->execute();
}


# Creates and calls an external script that calls sqlplus with the script name as a parameter
sub executeSQLPlusScript ( $$$$$ )
{
    my ( $u, $p , $db, $f, $title ) = ( shift,  shift,  shift,  shift, shift );
    if ( ! -e $f ) { die "ABORT: $f: file not found."; }
    my $tmpScript = `mktemp`;  # This creates and empty 0600 file.
    chomp $tmpScript;
    `echo "WHENEVER SQLERROR EXIT FAILURE;" > $tmpScript`;
    `echo "CONNECT $u/$p\@$db" >> $tmpScript`;
    `echo >> $tmpScript`;
    `cat $f >> $tmpScript`;
    `echo >> $tmpScript`;
    `echo "EXIT;" >> $tmpScript`;
    print ("$title\n");
    my $result = `'sqlplus' /NOLOG \@$tmpScript`;
    unlink $tmpScript;
    print("\n");
    print("$title RESULT\n");
    print("===================\n");
    my @result_array = split(/\n/, $result);
    $result = "";
    for (@result_array) {
        # Pass on the boring "everything's fine" messages.
        if (! /^((Package(| body)|Function|Table|Index|Trigger|Procedure|View|\d row(|s)) (created|altered|updated)\.|)$/) {
            $result .= $_."\n";
        }
    }
    if ( $result =~/ERROR/ ) {
        print $result;
        die "Error encountered in SQL script";
    } else {
        print "SUCCESS\n";
    }
}


# Creates and calls an external script that calls sqlplus with the script name as a parameter
sub executeSQLPlusScriptNoError ( $$$$$ )
{
    my ( $u, $p , $db, $f, $title ) = ( shift,  shift,  shift,  shift, shift );
    if ( ! -e $f ) { die "ABORT: $f: file not found."; }
    my $tmpScript = `mktemp`;  # This creates and empty 0600 file.
    chomp $tmpScript;
    `echo "WHENEVER SQLERROR EXIT FAILURE;" > $tmpScript`;
    `echo "CONNECT $u/$p\@$db" >> $tmpScript`;
    `echo >> $tmpScript`;
    `cat $f >> $tmpScript`;
    `echo >> $tmpScript`;
    `echo "EXIT;" >> $tmpScript`;
    print ("$title\n");
    my $result = `'sqlplus' /NOLOG \@$tmpScript`;
    unlink $tmpScript;
    print("\n");
    print("$title RESULT\n");
    print("===================\n");
    my @result_array = split(/\n/, $result);
    $result = "";
    for (@result_array) {
        # Pass on the boring "everything's fine" messages.
        if (! /^((Package(| body)|Function|Table|Index|Trigger|Procedure|View|\d row(|s)) (created|altered|updated)\.|)$/) {
            $result .= $_."\n";
        }
    }
    if ( $result =~/ERROR/ ) {
        print $result;
    } else {
        print "SUCCESS\n";
    }
}

# check_leftovers : find wether there are any leftover unmigrated data in the stager
sub check_leftovers ( $ )
{
    my $dbh = shift;
    my $sth = $dbh -> prepare ("SELECT count (*) FROM TABS");
    $sth -> execute();
    my @row = $sth->fetchrow_array();
    if ($row[0] == 0) {
        return 0;
    }
    $sth = $dbh -> prepare("SELECT count (*) from ( 
                              SELECT dc.id from diskcopy dc where
                                dc.status NOT IN ( 0, 7 )
                              UNION ALL
                              SELECT tc.id from tapecopy tc)"); # Discopy_staged diskcopy_invalid
    $sth -> execute ();
    @row = $sth->fetchrow_array();
    return $row[0];
}

# check_leftovers_poll_timeout: loop poll
sub check_leftovers_poll_timeout ( $$$ )
{
    my ( $dbh, $interval, $timeout ) = ( shift, shift, shift );
    my $start_time = time();
    while (check_leftovers($dbh)) {
        if (time()> $start_time + $timeout) {
            return 1;
        }
    }
    return 0;
}

# Replace undef members of an array by the string NULL
sub nullize_arrays_undefs ( $ )
{
    my $row=shift;
    for my $i ( 0 .. ( scalar(@{$row}) -1) ) {
        if ( ! defined ($row->[$i]) ) {
            $row->[$i] = 'NULL';
        }
    }
}

# print_leftovers
sub print_leftovers ( $ )
{
    my $dbh = shift;
    # Print by castofile with corresponding tapecopies
    my $sth = $dbh -> prepare ("SELECT cf.lastknownfilename, dc.id, dc.status, tc.id, tc.status 
                                  FROM castorfile cf
                                  LEFT OUTER JOIN diskcopy dc ON dc.castorfile = cf.id
                                  LEFT OUTER JOIN tapecopy tc ON tc.castorfile = cf.id
                                 WHERE dc.status NOT IN ( 0, 7)"); # Discopy_staged diskcopy_invalid
    $sth -> execute();
    while ( my @row = $sth->fetchrow_array() ) {
        nullize_arrays_undefs ( \@row );
        print( "Remaining catorfile for $row[0]\n\twith diskcopy (id=$row[1], ".
               "status=$row[2]) and tapecopy (id=$row[3], status=$row[4])\n" );
    }
    # print any other tapecopy not covered previously
    $sth = $dbh -> prepare ("SELECT cf.lastknownfilename, dc.id, dc.status, tc.id, tc.status 
                                  FROM castorfile cf
                                 RIGHT OUTER JOIN diskcopy dc ON dc.castorfile = cf.id
                                 RIGHT OUTER JOIN tapecopy tc ON tc.castorfile = cf.id
                                 WHERE dc.status IS NULL OR dc.status IN ( 0 )");
    $sth -> execute();
    while ( my @row = $sth->fetchrow_array() ) {
        nullize_arrays_undefs ( \@row );
        print( "Remaining tapecopy for $row[0]\n\twith diskcopy (id=$row[1], ".
               "status=$row[2]) and tapecopy (id=$row[3], status=$row[4])\n" );
    }
}

sub reinstall_stager_db()
{
    my $stager_host = getCastorConfParam('STAGER', 'HOST');

    # Print error message and abort if the user is not root
    my $uid = POSIX::getuid;
    my $gid = POSIX::getgid;
    if($uid != 0 || $gid !=0) {
        print("ABORT: This script must be ran as root\n");
        exit(-1);
    }

    my $checkout_location   = $environment{checkout_location};
    my $dbDir               = $environment{dbDir};
    my $originalDropSchema  = $environment{originalDropSchema};
    my $originalDbSchema    = $environment{originalDbSchema};

    my $originalDropSchemaFullpath=$checkout_location.'/'.$dbDir.'/'.$originalDropSchema;
    my $originalDbSchemaFullpath=$checkout_location.'/'.$dbDir.'/'.$originalDbSchema;

    die "ABORT: $originalDropSchema does not exist\n"
       if ! -e $originalDropSchemaFullpath;
    
    die "ABORT: $originalDbSchema does not exist\n"
        if ! -e $originalDbSchemaFullpath;
    
    # Make sure we're running on the proper machine.
    my $host = `uname -n`;
    die('ABORT: This script is only made to be run on host $stager_host\n')
      if ( ! $host =~ /^$stager_host($|\.)/i );

    my @diskServers = get_disk_servers();
    my $nbDiskServers = @diskServers;

    print("Found the following disk-servers: ");
    foreach(@diskServers) {
      print("$_ ");
    }
    print("\n");

    die("ABORT: Reinstall requires at least 2 disk-servers")
      if($nbDiskServers < 2);

    # Ensure all of the daemons accessing the stager-database are dead
    killDaemonWithTimeout('jobmanagerd' , 2);
    killDaemonWithTimeout('mighunterd'  , 2);
    killDaemonWithTimeout('rechandlerd' , 2);
    killDaemonWithTimeout('rhd'         , 2);
    killDaemonWithTimeout('rmmasterd'   , 2);
    killDaemonWithTimeout('rtcpclientd' , 2);
    killDaemonWithTimeout('stagerd'     , 2);
    killDaemonWithTimeout('tapegatewayd', 2);

    # Stop lsf
    print "Stopping lsf\n";
    `service lsf stop`;

    # Destroy the shared memory segment
    print "Removing shared memory segment\n";
    `ipcrm -M 0x00000946`;

    # Print shared memory usage
    print "Current shared memory usage:\n";
    {
	my $pm = `ps axh | cut -c 1-5 | egrep '[0-9]*' | xargs -i pmap -d {}`;
	my  $current_proc;
	foreach ( split /^/, $pm) { 
	    if ( /^\d+:\s+/ ) { $current_proc = $_ } 
	    elsif ( /\[\s+shmid=/ ) { 
		print $current_proc; 
		print $_; 
	    } 
	}
    }

    # Restart lsf to allow the restart of rmmasterd to recreate the shared memory, then re-stop all
    # Not really. lsf will catch the shared memory when it appears. Not worries...
    `service lsf start`;
    #`service rmmasterd start`;
    #`service lsf restart`;
    
    #poll_lsf_is_mmaped( 1, 60 );

    #`service rmmasterd stop`;

    # Re-print memory mapping. lsf should use the shmem segment
    #print "Current shared memory usage (we now expect /usr/lsf/etc/mbschd):\n";
    #{
    #	my $pm = `ps axh | cut -c 1-5 | egrep '[0-9]*' | xargs -i pmap -d {}`;
    #	my  $current_proc;
    #	foreach ( split /^/, $pm) { 
    #	    if ( /^\d+:\s+/ ) { $current_proc = $_ } 
    #	    elsif ( /\[\s+shmid=/ ) { 
    #		print $current_proc; 
    #		print $_; 
    #	    } 
    #	}
    #}
    

    # Re-create mighunterd daemon scripts
    print("Re-creating mighunterd daemon scripts\n");
    `echo "DAEMON_COREFILE_LIMIT=unlimited" >  /etc/sysconfig/mighunterd`;
    `echo 'SVCCLASSES="default dev"'        >> /etc/sysconfig/mighunterd`;
    `echo 'MIGHUNTERD_OPTIONS="-t 5"'       >  /etc/sysconfig/mighunterd.default`;
    `echo 'MIGHUNTERD_OPTIONS="-t 5"'       >  /etc/sysconfig/mighunterd.dev`;
  
    # Ensure there is no leftover in the DB
    my $dbh = open_db ();
    if ( check_leftovers ( $dbh ) ) {
        print_leftovers ( $dbh );
        $dbh->disconnect();
        die ("Found leftovers in the stager's DB. Stopping here.");
    }
    $dbh->disconnect();
    
    my $dbUser   = &getOrastagerconfigParam("user");
    my $dbPasswd = &getOrastagerconfigParam("passwd");
    my $dbName   = &getOrastagerconfigParam("dbName");
    
    executeSQLPlusScript ( $dbUser, $dbPasswd, $dbName, 
                           $originalDropSchemaFullpath,
                           "Dropping schema");
    
    my $stageUid = &get_uid_for_username('stage');
    my $stageGid = &get_gid_for_username('stage');
    my $adminList = $environment{adminList};
    
    my $hacked_creation= `mktemp`;
    chomp $hacked_creation;
    `cat $originalDbSchemaFullpath > $hacked_creation`;
    `sed -i s/^ACCEPT/--ACCEPT/ $hacked_creation`;
    `sed -i s/^PROMPT/--PROMPT/ $hacked_creation`;
    `sed -i s/^UNDEF/--UNDEF/ $hacked_creation`;
    `sed -i s/\\&stageGid/$stageGid/g $hacked_creation`;
    `sed -i s/\\&stageUid/$stageUid/g $hacked_creation`;
    `sed -i s/\\&adminList/$adminList/g $hacked_creation`;
    executeSQLPlusScript ( $dbUser, $dbPasswd, $dbName, 
                           $hacked_creation, "Re-creating schema");
    unlink $hacked_creation;
    
    # Restart the demons
    `/etc/init.d/jobmanagerd start`;
    #`/etc/init.d/mighunterd start`;
    `/etc/init.d/rechandlerd start`;
    `/etc/init.d/rhd start`;
    `/etc/init.d/rmmasterd start`;
    #`/etc/init.d/rtcpclientd start`;
    `/etc/init.d/stagerd start`;
    #`/etc/init.d/tapegatewayd start`;
    
    # Restart lsf
    print "Restarting lsf\n";
    `service lsf restart`;

    poll_rm_readyness ( 1, 15 );
    print "Sleeping extra 15 seconds\n";
    sleep (15);
    
    my $rmGetNodesResult = `rmGetNodes | egrep 'name:'`;
    print("\n");
    print("rmGetNodes RESULTS\n");
    print("==================\n");
    print($rmGetNodesResult);
    
    # Fill database with the standard set-up for a dev-box
    `nslistclass | grep NAME | awk '{print \$2}' | xargs -i enterFileClass --Name {} --GetFromCns`;
    
    `enterSvcClass --Name default --DiskPools default --DefaultFileSize 10485760 --FailJobsWhenNoSpace yes --NbDrives 1 --TapePool stager_dev03 --MigratorPolicy defaultMigrationPolicy --StreamPolicy defaultStreamPolicy`;
    `enterSvcClass --Name dev --DiskPools extra --DefaultFileSize 10485760 --FailJobsWhenNoSpace yes`;
    `enterSvcClass --Name diskonly --DiskPools extra --ForcedFileClass temp --DefaultFileSize 10485760 --Disk1Behavior yes --FailJobsWhenNoSpace yes`;
    
    `moveDiskServer default $diskServers[0]`;
    `moveDiskServer extra $diskServers[1]`;
    `rmAdminNode -r -R -n $diskServers[0]`;
    `rmAdminNode -r -R -n $diskServers[1]`;
    
    # Add a tape-pool to $environment{svcclass} service-class
    my $tapePool = get_environment('tapepool');
    `modifySvcClass --Name $environment{svcclass} --AddTapePool $tapePool --MigratorPolicy defaultMigrationPolicy --StreamPolicy defaultStreamPolicy`;
    
    # Set the number of drives on the default and dev service-classes to desired number for each
    `modifySvcClass --Name default --NbDrives 1`;
    `modifySvcClass --Name dev     --NbDrives 2`;
}


# Migrate the stager's db to the new tapegateway schema. This includes a switch to the tapegatewayd (in DB)
# stops but does not start the demons
sub migrateToNewTapeGatewaySchema ()
{
    # Stop the mighunter, rechandler, tapegateway, rtcpclientd
    for my $i ('mighunterd', 'rechandlerd', 'tapegatewayd', 'rtcpclientd') {
        if (killDaemonWithTimeout ($i, 5)) {
            die "Failed to stop a daemon. Call an exorcist.";
        }
    }
    my $full_test_dir=$environment{checkout_location}.'/'.
        $environment{tstDir}.'/';
    for my $i ('switchToTapegatewayd-no-triggers.sql',
               'tape_gateway_refactor_to_drop_tgsubrequest_table.sql',
               'tape_gateway_refactor_to_drop_tgrequest_table_and_triggers.sql',
               'tape_gateway_testing_round_001_tradeoffs.sql') {
        my $dbUser   = getOrastagerconfigParam("user");
        my $dbPasswd = getOrastagerconfigParam("passwd");
        my $dbName   = getOrastagerconfigParam("dbName");
        if ($i =~ /tgsubrequest/) {
            executeSQLPlusScriptNoError ( $dbUser, $dbPasswd, $dbName,
                               $full_test_dir.$i,
                               "Executing $i");
        } else {
            executeSQLPlusScript ( $dbUser, $dbPasswd, $dbName,
                                   $full_test_dir.$i,
                                   "Executing $i");
        }
    }
}


# Switch to tapegatewayd (in DB, stop demons before, massages the DB as required)
sub stopAndSwitchToTapeGatewayd ( $ )
{
    my $dbh = shift;

    # Stop the mighunter, rechandler, tapegateway, rtcpclientd
    for my $i ('mighunterd', 'rechandlerd', 'tapegatewayd', 'rtcpclientd') {
        if (killDaemonWithTimeout ($i, 5)) {
            die "Failed to stop a daemon. Call an exorcist.";
        }
    }    
    $dbh->do("UPDATE castorconfig SET value='tapegatewayd' WHERE class='tape' AND key='interfaceDaemon'");
    $dbh->commit();
}

# Switch from rtcpclientd (in DB, stops demons before, massages the DB as required)
sub stopAndSwitchToRtcpclientd ( $ )
{
    my $dbh = shift;

    # Stop the mighunter, rechandler, tapegateway, rtcpclientd
    for my $i ('mighunterd', 'rechandlerd', 'tapegatewayd', 'rtcpclientd') {
        if (killDaemonWithTimeout ($i, 5)) {
            die "Failed to stop a daemon. Call an exorcist.";
        }
    }
    
    $dbh->do("UPDATE castorconfig SET value='rtcpclientd' WHERE class='tape' AND key='interfaceDaemon'");
    $dbh->commit();
}


# Returns the LD_LIBRARY_PATH locating all of the *.so* files found in the
# specified root CASTOR source path
#
# @param  srcPath The root CASTOR source path containing the *.so* files
# @return         The LD_LIBRARY_PATH
sub getLdLibraryPathFromSrcPath ( $ ) {
  my $srcPath = $_[0];

  my $ldLibraryPath = `find $srcPath -follow -name '*.so*' | sed 's|/[^/]*\$||' | sort | uniq | xargs | sed 's/ /:/g'`;
  chomp($ldLibraryPath);

  return $ldLibraryPath;
}

sub cleanup () {
    print "t=".elapsed_time()."s. In CastorTapeTests::cleanup: cleaning up files\n";
    for (@local_files) {
        print "t=".elapsed_time()."s. Cleanup: removing ".$_->{name}."\n";
        unlink $_->{name};
    }
    #use Data::Dumper;
    #print Dumper \@remote_files;
    for ( my $i =scalar ( @remote_files ) - 1 ; $i >= 0; $i-- ) {
        #reverse order remotees removal to removes directories in the end.
        print "t=".elapsed_time()."s. Cleanup: removing ".$remote_files[$i]->{name}."\n";
	if ($remote_files[$i]->{type} eq "file") {
            `su $environment{username} -c \"STAGE_SVCCLASS=$environment{svcclass} stager_rm -M $remote_files[$i]->{name}\"`;
        }
        `su $environment{username} -c \"nsrm $remote_files[$i]->{name}\"`;
    }
}

# Final cleanup of the library.
# So far, get rid of leftover local files
END {
    print "t=".elapsed_time()."s. In CastorTapeTests::END: calling cleanup\n";
    cleanup();
    print "t=".elapsed_time()."s. Cleanup complete. Printing leftovers.\n";
    my $dbh = open_db();
    print_leftovers ($dbh);
    $dbh->disconnect();
    print "t=".elapsed_time()."s. End of leftovers\n";
}
# # create a local 
1;                            # this should be your last line
