.. _build_chapter:

Building and Installing
=======================

.. note::

  Examples in this chapter refer to a version X.Y.Z, please replace this with your
  current version, for example |release|!
  
This chapter describes, how you can build and install simulavr.

.. attention::

  The build scripts with cmake aren't prepared to work with Windows or Mac OS,
  even if cmake itself is able to create build configuration for this platforms!

Prerequisites
-------------

To build simulavr only you need:

* cmake (at least version 3.5 or newer)
* make
* gcc (at least a version, which supports c++11)
* git
* python (at least version 3.5 or newer)

If you want to build the python extension, you need also python development files
(e.g. in a debian or ubuntu platform you have to install `python3-dev` package)
and swig. (at least version 3.x or newer)

If you want to build tcl extension, you need:

* tclsh
* tcl development files
* swig (at least version 3.x or newer)

To build the verilog extension you have to install iverilog tool. (included vvp)

For running the complete regression test you need also:

* avr-gcc (on debian it's package gcc-avr)
* avr-libc
* valgrind (optional, check for overwriting memory and memory leaks)

Building the documentation suite needs also:

* gzip
* help2man
* makeinfo
* doxygen (optional, for api documentation)
* dot (optional, for api documentation)
* python module sphinx (at least version 2.2 for web site and manual)
* python module rst2pdf (at least version 0.94 for web site and manual)
* python module beautifulsoup4 (short bs4, for web site and manual)
* python module requests (for web site and manual)

Building debian packages need also:

* dpkg
* strip
* fakeroot

Build
-----
  
simulavr uses cmake and git. This means that you should be able to use the
following steps to build simulavr::

  > git clone https://git.savannah.nongnu.org/git/simulavr.git
  > cd simulavr
  > make build
  > make check

This will build ``simulavr`` and check, if the main function is ok.

In the root directory of repository you can find a ``Makefile`` as wrapper
for calling cmake and control configuration. If you call::

  > make

you'll get a help, what targets are available and what the targets will do.

Targets
+++++++

``make cfgclean``
  Removes the `build` directory. The make wrapper configures cmake for "out
  of source" build. E.g. nearly all build artifacts are below `build` directory.
  This removes also the complete cmake configuration.

``make clean``
  As usual, it removes all build artifacts but let cmake configuration untouched.

``make build``
  Build simulavr and all extensions as configured.

``make check``
  Run regression test.

``make doc``
  Build documentation, e.g. man page, info page, sphinx documentation (manual
  and web site).

``make doxygen``
  Build api documentation.

``make debian``
  Build debian packages for simulavr tool and lib and all configured extensions.

To switch on/off configuration options, you have to call make with the configuration
target::

  > make debug    # build program, libs and extensions with debug information
  > make no-debug # build without debug information
  
Other config targets are:

* python (build/do not build python module)
* tcl (build/do not build tcl module)
* verilog (build/do not build verilog extension)
* valgrind (run the gtest regression test additional with valgrind for memory check)

With target `all` and `simple` you can switch on and off all configuration options
together. There is one special target, taken over from old automake build system::

  > make keytrans

This create keytrans.h for tcl extension.

Debian packages
---------------

To support install on debian systems (e.g. debian and ubuntu and derivates) it's
possible to build debian packages. Go to root directory and call::

  > make debian
  
This will produce the following packages (PLAT represents the build platform):

``libsim_X.Y.Z_PLAT.deb``
  The simulavr lib itself.
  
``simulavr_X.Y.Z_PLAT.deb``
  The simulavr tool, depends on libsim package.
  
``simulavr-dev_X.Y.Z_PLAT.deb``
  Contains header and other files, to build applications against libsim, depends on
  libsim package.
  
``simulavr-vpi_X.Y.Z_PLAT.deb``
  Verilog extension, depends on libsim package.

``python3-simulavr_X.Y.Z_PLAT.deb``
  Python3 module, static linked, so no other dependencies needed.

``simulavr-tcl_X.Y.Z_PLAT.deb``
  Tcl extension, depends on libsim package.

On a debian system you could install the simulavr tool itself then (you need
root permission)::

  cd <root_of_repo>/build/debian
  apt install libsim_X.Y.Z_PLAT.deb # first install lib to fullfil dependencies
  apt install simulavr_X.Y.Z_PLAT.deb

After that you can check, if simulavr is ready::

  simulavr -h
  
Install
-------

If you want to make a installation on a system and not use debian packages, (maybe you
system doesn't support debian packages) then you can call the install target from cmake
itself after the normal build is finished::

  cd <root_of_repo>
  cmake --build build --target progdoc
  cmake --build build --target install

After that you'll find the install tree in the `build/install` directory. Copy this to
the destination, as you want. As example (you need root permission)::

  cd <root_of_repo>/build/install
  cp -r usr /

Build using docker
------------------

If docker is installed, then you can create docker images to build simulavr in a stable
and defined environment and independent from what's installed on your computer.

Step 1: create a docker image
+++++++++++++++++++++++++++++

There are docker scripts and a small script to create a image::

  cd <root_of_repo>/docker
  ./mkimage buildscripts/bionic.build.Dockerfile
  
This will create a docker image with name "simulavrbuild" and version "bionic". (e.g.
Ubuntu 18:04)You can check it with::

  docker images
  
There you should find the new created image in the list. **Attention:** because of the
installed packages in this image, the resulting image size is about 1G! There is also
a docker script for ubuntu 16:04, called "xenial.build.Dockerfile".

Now, after the image is ready, you can create the build container::

  docker run -it -u buildbudy --name <container_name> simulavrbuild:bionic
  buildbudy@e1694c8b9f26:/

You stay now in your new created container, the second line is the bash prompt. You have
created a container with name "<container_name>" (replace this to a name, which is useful
for you!) from the new built image and running with user "buildbudy". If you omit the "-u"
option, then you'll be root inside your container. Because this could be dangerous in some
cases and normally not needed, it's better to run with a normal user with normal privileges
also inside the container!

You can now leave the container on every time with "exit" command and come back with::

  docker start -i <container_name>
  
In this container you can now start the build::

  cd # to come to buildbudy's home directory
  git clone -b master https://git.savannah.nongnu.org/git/simulavr.git simulavr
  cd simulavr
  make all # to switch on all config options exept debug option
  make build
  
After that is finished, you've build sucessful simulavr and all extensions.

If you don't want to clone from official repository, as described before, you could also
clone from a local repository (maybe where you've written some new code). In this case the
container have to created with a extra option::

  docker run -it -u buildbudy --name <container_name> -v /local/path:/repo simulavrbuild:bionic

Replace (as before) "<container_name> with a useful name and /local/path with a path,
where your local repository is hold. For example /home/user/simulavr is the repository, then
ypu could give "-v /home/user:/repo". Inside the container you will find then a new directory
`/repo`, where you see your repository. Then the clone command could be::

  cd
  git clone -b your_branch /repo/simulavr simulavr_local
  
To get out your build artefacts, you can user "docker cp" command (and after you leaved
the container)::

  docker cp <container_name>:/home/buildbudy/simulavr/build/app/simulavr .
  docker cp <container_name>:/home/buildbudy/simulavr/build/libsim/libsim.so .
  
This copies the simulavr tool itself and the simulavr library, which is needed, simulavr to
run.
