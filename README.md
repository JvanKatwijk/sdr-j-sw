# sdr-j-sw

Sources for the sw software

Three subdirectories
a. configurator: simple program to set the path for the components in the .sdr-j-sw.ini" file;
b, swreceiver: main program for the swreceiver itself. Note that the plugins
have to be generated separately, their sources are in "swreceiver/plugins"
c. mirics-server: very simple server for connecting the SDRplay to a remote
device and using that as input (I am using it on an Raspberry that is close
to the antenna).

While the sdr-j-sw software is powerful software, it might be complicated to generate
an executable from the sources. This is mainly caused by the confusion  there might be on
the Qt installation and the qwt library, both needed to generate the software.

Many Linux systems spport both Qt4 and Qt5, some versions also provide in their repositories
a version of qwt compiled against Qt4 and a version of qwt compiled against Qt5.
However, some other Linux systems support both Qt4 and Qt5, providing only one version of qwt.
While it is perfectly possible to compile the system against a Qt4 system and with a qwt library that
is compiled against Qt5, running such a compiled system crashes within seconds.

As an example, the qwt library on my Raspbian system is compiled against Qt4,  while both Qt4 and Qt5
libraries are available. Compiling the system is therefore - in that case - the obvious way.

One of the practical issues then is that the configuration files, i.e. the ".pro" files need to be
adaoted to indicate the searchpaths to the required included files and the required libraries.

Plugins and pathnames
The structure of the system is that the swreceiver software is the kernel of the system, with the
device libraries and the decoders as plugins, loaded when selected.
The software, however, has to know where to find the plugins. The pathnames to the directories where the
plugins for the decoders resp. the device handlers are stored, are kept in an ".ini" file, a file where
some configuration parameters are stored. The ".ini" file is named ".jsdr-sw.ini" and stored in the home
directory of the user.
While it is certainly possible to manually fill in these pathnamens, the program "configure-sw"
can be used to create the ".ini" file if needed and add the path names.



By default generated programs will be placed in "./linux-bin",
the generated plugins for input handling in "./linux-bin/input-plugins",
and the generated plugins for the decoders in "./linux-bin/decoder-plugins".

