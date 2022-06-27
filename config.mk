VERSION = 1.5

EXENAME ?= shkd
BUILDROOT ?= build
INSTALLPREFIX ?= /usr/local

CXX ?= c++

CXXFLAGS ?= -Os -s -Wall -static -nostartfiles -e main -fno-PIC
LDFLAGS  ?=
