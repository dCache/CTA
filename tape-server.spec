Summary: The CERN tape server project
Name: tape-server
Version: 0.0.0
Release: 1
Prefix: /usr
License: GPL
Group: Applications/File

Source: %{name}-%{version}-%{release}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root

BuildRequires: cmake >= 2.6
BuildRequires: gtest >= 1.5.0
BuildRequires: gmock >= 1.5.0
BuildRequires: gtest-devel >= 1.5.0
BuildRequires: gmock-devel  >= 1.5.0
BuildRequires: valgrind >= 3.5.0
BuildRequires: tetex-latex
BuildRequires: latex2html
BuildRequires: ImageMagick
BuildRequires: doxygen
BuildRequires: graphviz

%description
The CERN tape server project.

#######################################################################################
%package -n tape-server-utils
#######################################################################################
Summary: The CERN tape server utilities
Group: Applications/File

%description -n tape-server-utils
The CERN tape server utilities

#######################################################################################
%package -n tape-server-docs
#######################################################################################
Summary: The CERN tape server documentation
Group: Applications/File

%description -n tape-server-docs
The CERN tape server documentation

#######################################################################################
%package -n tape-server-system-tests
#######################################################################################
Summary: The CERN tape server system tests
Group: Applications/File
Requires: kmod-st-driver
Requires: kmod-mhvtl >= 1.4.4, mhvtl-utils >= 1.4.4

%description -n tape-server-system-tests
The CERN tape server system tests. Those tests are potentially destructive and should be used with care.

%prep

%setup -n %{name}-%{version}-%{release}

%build
test -e $RPM_BUILD_ROOT && rm -r $RPM_BUILD_ROOT
#%if 0%{?rhel} < 6 && %{?fedora}%{!?fedora:0} <= 1
#export CC=/usr/bin/gcc44 CXX=/usr/bin/g++44 
#%endif

mkdir -p build
cd build
cmake ../ -DRELEASE=%{release} -DCMAKE_BUILD_TYPE=Debug
%{__make} %{_smp_mflags} all 
#TapeServer_pdf

%install
cd build
%{__make} install DESTDIR=$RPM_BUILD_ROOT
echo "Installed!"

%check
cd build
test/unitTest
valgrind --leak-check=full --show-reachable=yes --error-exitcode=1 test/unitTest

%clean
rm -rf $RPM_BUILD_ROOT

%files -n tape-server-utils
%defattr(-,root,root)
/usr/local/bin/TapeDriveReadWriteTest
/usr/local/bin/unitTest

%files -n tape-server-docs
%defattr(-,root,root)
/usr/share/doc/*

%files -n tape-server-system-tests
%defattr(-,root,root)
/etc/*
%attr(755,-,-) /usr/local/bin/tape-server-system-test.sh



