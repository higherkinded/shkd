VERSION = 1.4-dev

EXENAME ?= shkd
BUILDROOT ?= build
INSTALLPREFIX ?= /usr/local

CXX ?= c++

CXXFLAGS ?= -Os -s -Wall -static -nostartfiles -e main -fno-PIC
LDFLAGS  ?=
