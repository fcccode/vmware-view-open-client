# -*- Makefile -*-
################################################################################
### Copyright 2007 VMware, Inc.  All rights reserved.
###
### This program is free software; you can redistribute it and/or modify
### it under the terms of version 2 of the GNU General Public License as
### published by the Free Software Foundation.
###
### This program is distributed in the hope that it will be useful,
### but WITHOUT ANY WARRANTY; without even the implied warranty of
### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
### GNU General Public License for more details.
###
### You should have received a copy of the GNU General Public License
### along with this program; if not, write to the Free Software
### Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
################################################################################

noinst_LIBRARIES += libUnicode.a

libUnicode_a_SOURCES =
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeCommon.c
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeInt.h
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeSimpleBase.c
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeSimpleCaseFolding.c
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeSimpleTypes.c
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeSimpleOperations.c
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeSimpleTransforms.c
libUnicode_a_SOURCES += lib/open-vm-tools/unicode/unicodeStatic.c
