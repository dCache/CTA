#!/bin/sh

if [ "x${MAJOR_CASTOR_VERSION}" = "x" ]; then
  echo "No MAJOR_CASTOR_VERSION environment variable - guessing from changelog"
  MAJOR_CASTOR_VERSION=`egrep "^castor" debian/changelog | awk '{print $2}' | head -1 | perl -ne 'if (/\((\d+)\.(\d+)/) {print "$1.$2\n";}'`
  if [ "x${MAJOR_CASTOR_VERSION}" = "x" ]; then
    echo "Unable to guess MAJOR_CASTOR_VERSION"
    exit 1
  fi
  export MAJOR_CASTOR_VERSION
  echo "... Got MAJOR_CASTOR_VERSION=${MAJOR_CASTOR_VERSION}"
fi
if [ "x${MINOR_CASTOR_VERSION}" = "x" ]; then
  echo "No MINOR_CASTOR_VERSION environment variable - guessing from changelog"
  MINOR_CASTOR_VERSION=`egrep "^castor" debian/changelog | awk '{print $2}' | head -1 | perl -ne 'if (/(\d+)\-(\d+)\)/) {print "$1.$2\n";}'`
  if [ "x${MINOR_CASTOR_VERSION}" = "x" ]; then
    echo "Unable to guess MINOR_CASTOR_VERSION"
    exit 1
  fi
  export MINOR_CASTOR_VERSION
  echo "... Got MINOR_CASTOR_VERSION=${MINOR_CASTOR_VERSION}"
fi

#
## We expect ${MAJOR_CASTOR_VERSION} in the form a.b
#
echo ${MAJOR_CASTOR_VERSION} | egrep -q '^[0-9]+\.[0-9]+$'
if [ $? -ne 0 ]; then
  echo "MAJOR_CASTOR_VERSION (${MAJOR_CASTOR_VERSION}) should be in the form a.b, example: 2.1"
  exit 1
fi

#
## We expect ${MINOR_CASTOR_VERSION} in the form c.d
#
echo ${MINOR_CASTOR_VERSION} | egrep -q '^[0-9]+\.[0-9]+$'
if [ $? -ne 0 ]; then
  echo "MINOR_CASTOR_VERSION (${MINOR_CASTOR_VERSION}) should be in the form c.d, example: 99.1"
  exit 1
fi

a=`echo ${MAJOR_CASTOR_VERSION} | sed 's/\..*//g'`
b=`echo ${MAJOR_CASTOR_VERSION} | sed 's/.*\.//g'`
c=`echo ${MINOR_CASTOR_VERSION} | sed 's/\..*//g'`
d=`echo ${MINOR_CASTOR_VERSION} | sed 's/.*\.//g'`
version=${a}.${b}.${c}
fullversion=${version}-${d}

echo "### Making build directory"

# Go to castor-${version} and do the changes in here
curdir=`pwd`
cd ..
[ -d "castor-${version}" ] && rm -rf castor-${version}
rsync -aC --exclude '.__afs*' $curdir/ castor-${version}
rm -rf castor-${version}/monitoring/castor-mon-web
cd castor-${version}

echo "### Customizing spec file"

#
# Make spec file in sync
#
perl -pi -e s/\__MAJOR_CASTOR_VERSION__/${MAJOR_CASTOR_VERSION}/g CASTOR.spec
perl -pi -e s/\__MINOR_CASTOR_VERSION__/${MINOR_CASTOR_VERSION}/g CASTOR.spec
perl -pi -e s/__A__/${a}/g CASTOR.spec
perl -pi -e s/__B__/${b}/g CASTOR.spec
perl -pi -e s/__C__/${c}/g CASTOR.spec
perl -pi -e s/__D__/${d}/g CASTOR.spec

function copyInstallPermInternal {
    package=$1
    file=$2
    if [ -s "debian/$package.conffiles" ]; then
      for f in `cat debian/$package.conffiles`; do
          g=`echo $f | sed 's/\\//\\\\\\//g'`
          sed -i "s/$g\$/%config(noreplace) $g/" $file
      done
    fi
    cat $file >> CASTOR.spec
}

function copyInstallPerm {
    package=$1
    cp -f debian/$package.install.perm debian/$package.install.perm.tmp
    # Add a missing '/'
    perl -pi -e 's/\) /\) \//g' debian/$package.install.perm.tmp
    # compute 64 bits version :
    #   - replace usr/lib by usr/lib64
    #   - except for /usr/lib/perl
    #   - except for /usr/lib/log
    #   - replace gcc32dbg by gcc64dbg (gridFTP specific)
    perl -p -e 's/usr\/lib/usr\/lib64/g;s/usr\/lib64\/perl/usr\/lib\/perl/g;s/usr\/lib64\/python/usr\/lib\/python/g;s/usr\/lib64\/log/usr\/lib\/log/g;s/gcc32dbg/gcc64dbg/g' debian/$package.install.perm.tmp > debian/$package.install.perm.64.tmp
    # check whether to bother with 64 bits specific code
    diff -q debian/$package.install.perm.tmp debian/$package.install.perm.64.tmp > /dev/null
    if [ $? -eq 0 ]; then
      copyInstallPermInternal $package debian/$package.install.perm.tmp
    else
      echo "%ifarch x86_64" >> CASTOR.spec
      copyInstallPermInternal $package debian/$package.install.perm.64.tmp
      echo "%else" >> CASTOR.spec
      copyInstallPermInternal $package debian/$package.install.perm.tmp
      echo "%endif" >> CASTOR.spec
    fi
    #rm -f debian/$package.install.perm.tmp debian/$package.install.perm.64.tmp
}

#
## Append all sub-packages to CASTOR.spec
#
for this in `grep Package: debian/control | awk '{print $NF}' | grep -v castor-tape-server` castor-tape-server castor-tape-server-nostk; do
    package=$this
    actualPackage=$package

    # handle nostk vs normal compilation of castor-tape-server
    if [ "$package" = "castor-tape-server" ]; then
	echo "%if ! %compiling_nostk" >> CASTOR.spec
    fi
    if [ "$package" = "castor-tape-server-nostk" ]; then
        echo "%else" >> CASTOR.spec
        package="castor-tape-server"
    fi

    #
    ## get package group if any
    #
    group=`cat debian/control | perl -e '
      $package=shift;
      $what=shift;
      $this = do { local $/; <> };
      $this =~ s/.*Package: $package[^\w\-]//sg;
      $this =~ s/Package:.*//sg;
      map {if (/([^:]+):(.+)/) {$this{$1}=$2};} split("\n",$this);
      if (defined($this{$what})) {
        print "$this{$what}\n";
      }' $package XBS-Group |
      sed 's/ //g' | sed 's/\${[^{},]*}//g' | sed 's/^,*//g' | sed 's/,,*/,/g'`
    if [ "${group}" != "Client" ]; then
        echo "%if ! %compiling_client" >> CASTOR.spec # no a client package
    fi
    echo "%package -n $actualPackage" >> CASTOR.spec
    echo "Summary: Cern Advanced mass STORage" >> CASTOR.spec
    echo "Group: Application/Castor" >> CASTOR.spec
    #
    ## Except for Description that is truely a multi-line thing even within debian/control, the other
    ## fields are always on a single line:
    ## The following perl script isolates the Package: definition of a package, then split it using
    ## newlines, then look which line matches the keyword.
    ## Then we remove spaces
    #
    ## Get requires
    #
    ## Requires might give ${shlibs:Depends},${misc:Depends} in the output: we will drop that
    ## This is why there are extra sed's after the original that remove the spaces
    #
    #
    requires=`cat debian/control | perl -e '
      $package=shift;
      $what=shift;
      $this = do { local $/; <> };
      $this =~ s/.*Package: $package[^\w\-]//sg;
      $this =~ s/Package:.*//sg;
      map {if (/([^:]+):(.+)/) {$this{$1}=$2};} split("\n",$this);
      if (defined($this{$what})) {
        print "$this{$what}\n";
      }' $package Depends |
      sed 's/ //g' | sed 's/\${[^{},]*}//g' | sed 's/^,*//g' | sed 's/,,*/,/g' | sed 's/cx-Oracle/cx_Oracle/g' | sed 's/vdt-globus-essentials/vdt_globus_essentials/g' | sed 's/vdt-globus-data-server/vdt_globus_data_server/g'`
    if [ -n "${requires}" ]; then
        echo "Requires: ${requires}" >> CASTOR.spec
    fi
    #
    ## Get provides
    #
    provides=`cat debian/control | perl -e '
      $package=shift;
      $what=shift;
      $this = do { local $/; <> };
      $this =~ s/.*Package: $package[^\w\-]//sg;
      $this =~ s/Package:.*//sg;
      map {if (/([^:]+):(.+)/) {$this{$1}=$2};} split("\n",$this);
      if (defined($this{$what})) {
        print "$this{$what}\n";
      }' $package Provides |
      sed 's/ //g'`
    if [ -n "${provides}" ]; then
        echo "Provides: ${provides}" >> CASTOR.spec
    fi
    #
    ## Get conflicts
    #
    conflicts=`cat debian/control | perl -e '
      $package=shift;
      $what=shift;
      $this = do { local $/; <> };
      $this =~ s/.*Package: $package[^\w\-]//sg;
      $this =~ s/Package:.*//sg;
      map {if (/([^:]+):(.+)/) {$this{$1}=$2};} split("\n",$this);
      if (defined($this{$what})) {
        print "$this{$what}\n";
      }' $package Conflicts |
      sed 's/ //g'`
    if [ -n "${conflicts}" ]; then
        echo "Conflicts: ${conflicts}" >> CASTOR.spec
    fi
    #
    ## Get Obsoletes
    #
    obsoletes=`cat debian/control | perl -e '
      $package=shift;
      $what=shift;
      $this = do { local $/; <> };
      $this =~ s/.*Package: $package[^\w\-]//sg;
      $this =~ s/Package:.*//sg;
      map {if (/([^:]+):(.+)/) {$this{$1}=$2};} split("\n",$this);
      if (defined($this{$what})) {
        print "$this{$what}\n";
      }' $package XBS-Obsoletes |
      sed 's/ //g'`
    if [ -n "${obsoletes}" ]; then
        echo "Obsoletes: ${obsoletes}" >> CASTOR.spec
    fi
    #
    ## Get description
    #
    echo "%description -n $actualPackage" >> CASTOR.spec
    cat debian/control | perl -e '
      $package=shift;
      $what=shift;
      $this = do { local $/; <> };
      $this =~ s/.*Package: $package[^\w\-]//sg;
      $this =~ s/Package:.*//sg;
      $this =~ s/.*Description: +//sg;
      $this =~ s/\n*$//sg;
      $this =~ s/\n/ \- /sg;
      $this =~ s/  */ /sg;
      print "$this\n";' $package >> CASTOR.spec
    ## Get file list
    echo "%files -n $actualPackage" >> CASTOR.spec
    echo "%defattr(-,root,root)" >> CASTOR.spec
    ## deal with manpages
    if [ -s "debian/$package.manpages" ]; then
        for man in `cat debian/$package.manpages | sed 's/debian\/castor\///g'`; do
            echo "%attr(0644,root,bin) %doc /$man" >> CASTOR.spec
        done
    fi
    ## deal with directories
    if [ -s "debian/$package.dirs" ]; then
        cat debian/$package.dirs | while read dir; do
            if [ `echo $dir | grep -c %attr` -lt 1 ]; then
                echo "%attr(-,root,bin) %dir /$dir" >> CASTOR.spec
            else
                echo $dir | sed -e "s/) /) %dir \//" >> CASTOR.spec
            fi
        done
    fi
    ## deal with files
    if [ -s "debian/$package.install.perm" ]; then
        copyInstallPerm $package
    fi
    ## deal with scripts
    if [ -s "debian/$package.postinst" ]; then
        echo "%post -n $actualPackage" >> CASTOR.spec
        cat debian/$package.postinst >> CASTOR.spec
    fi
    if [ -s "debian/$package.preun" ]; then
        echo "%preun -n $actualPackage" >> CASTOR.spec
        cat debian/$package.preun >> CASTOR.spec
    fi
    if [ -s "debian/$package.pre" ]; then
        echo "%pre -n $actualPackage" >> CASTOR.spec
        cat debian/$package.pre >> CASTOR.spec
    fi
    if [ -s "debian/$package.postun" ]; then
        echo "%postun -n $actualPackage" >> CASTOR.spec
        cat debian/$package.postun >> CASTOR.spec
    fi
    if [ "${group}" != "Client" ]; then
        echo "%endif" >> CASTOR.spec # end of client compilation if
    fi
    echo >> CASTOR.spec
done
echo "%endif" >> CASTOR.spec # end of compiling_notstk if

cd ..

echo "### Creating tarball"

tar -zcf castor-${fullversion}.tar.gz --exclude '.__afs*' castor-${version}
rm -rf castor-${version}
