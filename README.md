# sdr-j-sw

Sources for the sw software

Three subdirectories
a. configurator: simple program to set the path for the components in the .sdr-j-sw.ini" file;
b, swreceiver: main program for the swreceiver itself. Note that the plugins
have to be generated separately, their sources are in "swreceiver/plugins"
c. mirics-server: very simple server for connecting the SDRplay to a remote
device and using that as input (I am using it on an Raspberry that is close
to the antenna).

By default generated programs will be placed in "./linux-bin",
the generated plugins for input handling in "./linux-bin/input-plugins",
and the generated plugins for the decoders in "./linux-bin/decoder-plugins".

