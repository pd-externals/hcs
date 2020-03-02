# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = hcs

# input source file (class name == source file basename)
class.sources = \
	canvas_name.c \
	ce_path.c \
	classpath.c \
	colorpanel.c \
	cursor.c \
	folder_list.c \
	group.c \
	helppath.c \
	passwd.c \
	screensize.c \
	setenv.c \
	split_path.c \
	sql_query.c \
	stat.c \
	sys_gui.c \
	uname.c \
	unsetenv.c \
	version.c \
	window_name.c \
	$(empty)

# list all pd objects (i.e. myobject.pd) files here, and their helpfiles will
# be included automatically
abstractions = \
	ISOdate.pd \
	ISOtime.pd \
	ascii2int.pd \
	autoscale.pd \
	blocksize_in_ms.pd \
	debounce.pd \
	debug.pd \
	embed.pd \
	file_type.pd \
	float2ascii.pd \
	get-audio-dialog.pd \
	get-midi-dialog.pd \
	gid2group_name.pd \
	group_name2gid.pd \
	hostname.pd \
	make-audio-dialog.pd \
	mouse_region.pd \
	noquit.pd \
	pi.pd \
	pointer_position.pd \
	pwm.pd \
	pwm~.pd \
	split_my_msgs.pd \
	square~.pd \
	tcl_version.pd \
	tkconsole.pd \
	tremolo~.pd \
	uid2username.pd \
	username2uid.pd \
	$(empty)

# standard supplementary files
datafiles = $(lib.name)-meta.pd README.txt LICENSE.txt

# example patches and related files, in the 'examples' subfolder
datadirs = examples

# some Tcl/Tk files
datafiles += colorpanel.tcl cursor.tcl

# For objects that only build on certain platforms, add those to class.sources
# line for the right platforms.
define forLinux
  datafiles += README-ifeel.txt
  class.sources += ifeel.c
endef

datafiles += \
	$(class.sources:%.c=%-help.pd) \
	$(empty)
datafiles += \
	$(abstractions) \
	$(abstractions:%.pd=%-help.pd) \
	$(empty)

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
