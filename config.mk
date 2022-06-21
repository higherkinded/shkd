VERSION = 1.2

EXENAME ?= shkd
BUILDROOT ?= build
INSTALLPREFIX ?= /usr/local

CXX ?= c++

CXXFLAGS ?= -Os -s -Wall -static -nostartfiles -e main -fno-PIC
LDFLAGS  ?=
