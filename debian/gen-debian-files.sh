#!/bin/sh

# Copyright (C) 2015 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Michi Henning <michi.henning@canonical.com>

#
# Script to generate debian files for dual landing in Vivid (gcc 4.9 ABI)
# and Wily and later (gcc 5 ABI).
#
# This script is called from debian/rules and generates:
#
# - control
# - libunity-scopes${soversion}.install
# - libunity-scopes-dev.install
# - libunity-scopes${soversion}.scope.click-hook
# - libunity-scopes${soversion}.symbols (for Vivid only)
# - shlibs (for Wily and later only)
# - libunity-scopes-qt${qt_soversion}.install
# - libunity-scopes-qt-dev.install
# - libunity-scopes-qt${qt_soversion}.symbols (for Vivid only)
#
# For all but control, this is a straight substition and/or renaming exercise for each file.
# For control, if building on Wily or later, we also fix the "Replaces:" and "Conflicts:"
# entries, so we don't end up with two packages claiming ownership of the same places
# in the file system.
#
# Because the debian files for the different distributions are generated on the fly,
# this allows us to keep a single source tree for both distributions. See ../HACKING
# for more explanations.
#

set -e  # Fail if any command fails.

[ $# -ne 0 ] && {
    echo "usage: $(basename $0)" >&2
    exit 1
}
dir=./debian

# Set soversions depending on whether we are running on vivid or wily and later.

distro=$(lsb_release -c -s)
echo "gen-debian-files: detected distribution: $distro"

full_version=$(cat "${dir}"/VERSION)
qt_full_version=$(cat "${dir}"/QT-VERSION)

major=$(echo $full_version | cut -d'.' -f1)
minor=$(echo $full_version | cut -d'.' -f2)
major_minor="${major}.${minor}"

qt_major=$(echo $qt_full_version | cut -d'.' -f1)
qt_minor=$(echo $qt_full_version | cut -d'.' -f2)
qt_major_minor="${qt_major}.${qt_minor}"

vivid_soversion=$(expr $minor + 3)
vivid_qt_soversion=$(expr $qt_minor)

if [ "$distro" = "vivid" ]
then
    soversion=$vivid_soversion
    qt_soversion=${qt_minor}
else
    soversion="${major}.${minor}"
    qt_soversion="${qt_major}.${qt_minor}"
fi
[ -n $soversion ]
[ -n $qt_soversion ]

warning=$(mktemp -t gen-debian-files-msg.XXX)

trap "rm $warning" 0 INT TERM QUIT

warning_msg()
{
    cat >$warning <<EOF
# This file is autogenerated. DO NOT EDIT!
#
# Modifications should be made to $(basename "$1") instead.
# This file is regenerated automatically in the clean target.
#
EOF
}

# Generate debian/control from debian/control.in, substituting the soversion for both libs.
# For wily onwards, we also add an entry for the vivid versions to "Conflicts:" and "Replaces:".

infile="${dir}"/control.in
outfile="${dir}"/control
warning_msg $infile
cat $warning $infile \
    | sed -e "s/@UNITY_SCOPES_SOVERSION@/${soversion}/" \
          -e "s/@UNITY_SCOPES_QT_SOVERSION@/${qt_soversion}/" >"$outfile"

[ "$distro" != "vivid" ] && {
    sed -i -e "/Replaces: libunity-scopes0,/a\
\          libunity-scopes${vivid_soversion}," \
           -e "/Conflicts: libunity-scopes0,/a\
\           libunity-scopes${vivid_soversion}," \
           -e "/Replaces: libunity-scopes-qt0.1.0,/a\
\          libunity-scopes-qt${vivid_qt_soversion}," \
           -e "/Conflicts: libunity-scopes-qt0.1.0,/a\
\           libunity-scopes-qt${vivid_qt_soversion}," \
        "$outfile"
}

# Generate the install files, naming them according to the soversion.

# Install file for binary package
infile="${dir}"/libunity-scopes.install.in
outfile="${dir}"/libunity-scopes${soversion}.install
warning_msg "$infile"
cat $warning "$infile" >"$outfile"

# Install file for dev package
infile="${dir}"/libunity-scopes-dev.install.in
outfile="${dir}"/libunity-scopes-dev.install
warning_msg "$infile"
cat $warning "$infile" \
    | sed "s/@UNITY_SCOPES_SOVERSION@/${soversion}/" >"$outfile"

# Install file for click hook
infile="${dir}"/libunity-scopes.scope.click-hook.in
outfile="${dir}"/libunity-scopes${soversion}.scope.click-hook
warning_msg "$infile"
cat $warning "$infile" >"$outfile"

# Symbols file for vivid or shlibs file for wily and later
if [ "$distro" = "vivid" ]
then
    infile="${dir}"/libunity-scopes.symbols.in
    outfile="${dir}"/libunity-scopes${soversion}.symbols
    sed "s/@UNITY_SCOPES_SOVERSION@/${soversion}/g" "$infile" >"$outfile"

    infile="${dir}"/libunity-scopes-qt.symbols.in
    outfile="${dir}"/libunity-scopes-qt${qt_soversion}.symbols
    sed "s/@UNITY_SCOPES_QT_SOVERSION@/${qt_soversion}/g" "$infile" >"$outfile"
else
    # Single shlibs file for both libunity-scopes and libunity-scopes-qt.
    infile="${dir}"/shlibs.in
    outfile="${dir}"/shlibs
    warning_msg "$infile"
    cat $warning "$infile" \
        | sed -e "s/@UNITY_SCOPES_SOVERSION@/${soversion}/g" \
              -e "s/@UNITY_SCOPES_QT_SOVERSION@/${qt_soversion}/g" \
              -e "s/@UNITY_SCOPES_MAJOR_MINOR@/${major_minor}/g" \
              -e "s/@UNITY_SCOPES_QT_MAJOR_MINOR@/${qt_major_minor}/g" \
            >"$outfile"
fi

# Install file for qt binary package
infile="${dir}"/libunity-scopes-qt.install.in
outfile="${dir}"/libunity-scopes-qt${qt_soversion}.install
warning_msg "$infile"
cat $warning "$infile" >"$outfile"

# Install file for qt dev package
infile="${dir}"/libunity-scopes-qt-dev.install.in
outfile="${dir}"/libunity-scopes-qt-dev.install
warning_msg "$infile"
cat $warning $infile \
    | sed "s/@UNITY_SCOPES_QT_SOVERSION@/${qt_soversion}/" >"$outfile"

exit 0