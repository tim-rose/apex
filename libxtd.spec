#
# XTD.SPEC --RPM build configuration for xtd.
#
Summary: Some xtd (i.e. suitable) extensions to the C library.
Name: xtd
Version: 0
Release: 0
License: Copyright (c) 2014 Tim Rose

%description
Xtd is a collection of C language functions that I have 
found useful over the years.

# Perform the build in the current directory
%define _sourcedir %{expand:%(pwd)}
%define _builddir %{_sourcedir}
%define _rpmdir %{_sourcedir}
%define _rpmfilename %{OS}-%{ARCH}/%{NAME}-%{VERSION}-%{RELEASE}.%{ARCH}.rpm

%install
%{__rm} -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT prefix= usr=usr install

%clean
%{__rm} -rf $RPM_BUILD_ROOT

%files -f xtd-files.txt

%changelog
* Mon Sep 10 2007 Timothy Rose <tim.rose@acm.org>
- Release 1-1:
- Initial release
