VERSION = 1.3

EXENAME ?= shkd
BUILDROOT ?= build
INSTALLPREFIX ?= /usr/local

CXX ?= c++

CXXFLAGS ?= -Os -s -Wall -static -nostartfiles -e main -fno-PIC
LDFLAGS  ?=
