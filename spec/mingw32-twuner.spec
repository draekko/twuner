#
# spec file for package twuner
#


%define __strip %{_mingw32_strip}
%define __objdump %{_mingw32_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw32_findrequires}
%define __find_provides %{_mingw32_findprovides}
%define __os_install_post %{_mingw32_debug_install_post} \
			%{_mingw32_install_post}
%define appname twuner

Name:           mingw32-twuner
Version:        0.9.8.8
Release:        beta4
Summary:        A lightweight, flexible microblogging client
License:        LGPL-3.0
Group:          Productivity/Networking/Instant Messenger
Url:            https://draekko.org
Source:         twuner-%{version}.tar.xz
BuildRequires:  ccache
BuildRequires:  cmake
BuildRequires:  mingw32-cross-gcc
BuildRequires:	mingw32-cross-gcc-c++
BuildRequires:	mingw32-cross-binutils
BuildRequires:	mingw32-cross-pkg-config
BuildRequires:  intltool
BuildRequires:  update-desktop-files
BuildRequires:  xz
BuildRequires:  mingw32-libqtwebkit
BuildRequires:  mingw32-libqt4-devel
BuildArch:      noarch


%description
Twuner is a multi-column microblogging client written in HTML5 through Webkit, thus can run on any device supports webkit.

%prep
%setup -q -n %{appname}-%{version}
# Already Fix Upstream. Will be droped next major release.
sed -i "s/Categories=Qt;Network;/Categories=Qt;Network;InstantMessaging;/" misc/twuner-qt.desktop.in

%build
mkdir winbuild
cd winbuild
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-windows-mingw32-openSUSE.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=%{buildroot}%{_mingw32_prefix} \
      -DLIB_INSTALL_DIR=%{buildroot}%{_mingw32_libdir} \
      -DWITH_GIR=OFF \
      -DWITH_GTK=OFF \
      -DWITH_QT=ON \
      -DWITH_KDE=OFF \
	..
%{_mingw32_make} %{?_smp_mflags}

%install
cd winbuild
%{_mingw32_makeinstall}
cd ..

# Rename Binaries
mv %{buildroot}%{_mingw32_bindir}/%{appname}-qt.exe %{buildroot}%{_mingw32_bindir}/%{appname}.exe

# Rename Desktopfiles
mv %{buildroot}%{_mingw32_datadir}/applications/%{appname}-qt.desktop %{buildroot}%{_mingw32_datadir}/applications/%{appname}.desktop

%suse_update_desktop_file %{appname} Network InstantMessaging

# Install Documents
mkdir -p %{buildroot}%{_mingw32_docdir}/%{appname}/
cp -r ChangeLog %{buildroot}%{_mingw32_docdir}/%{appname}/
cp -r LGPL-license.txt %{buildroot}%{_mingw32_docdir}/%{appname}/

%find_lang %{appname}

%files -f %{appname}.lang
%defattr(-,root,root)
%{_mingw32_bindir}/%{appname}.exe
%dir %{_mingw32_datadir}/applications
%{_mingw32_datadir}/applications/%{appname}.desktop
%{_mingw32_datadir}/%{appname}/
%{_mingw32_datadir}/icons/
%{_mingw32_datadir}/locale/
%{_mingw32_docdir}/%{appname}/

%changelog
