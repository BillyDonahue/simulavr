Build simulavr
==============

.. note::

  Examples in this chapter refer to a version X.Y.Z, please replace this with your
  current version, for example |release|!
  
This chapter describes, how you can build simulavr.

.. attention::

  The build scripts with cmake aren't prepared to work with Windows or Mac OS,
  even if cmake itself is able to create build configuration for this platforms!

**For more informations about special options and other targets please read our
manual!**

Prerequisites
-------------

To build simulavr you need:

* cmake (at least version 3.5 or newer)
* make
* gcc (at least a version, which supports c++11)
* git
* python (at least version 3.5 or newer)

Of course, docker have to be installed, if you want to build it with docker!

On debian (or also ubuntu) you have to install the following packages::

  > apt-get install g++ make cmake git python3
  
Do the build
------------

The conventional way on linux
+++++++++++++++++++++++++++++

At first, you have to clone the repository and change to the cloned repository::

  > git clone https://git.savannah.nongnu.org/git/simulavr.git
  > cd simulavr

If you want to produce a dedicated release, you have to change to the release tag::

  > git checkout rel-X.Y.Z

Now you are ready to build simulavr::

  > make build

After the build was successful without a error, you can find the built program in
*build/app*, shared lib is in *build/libsim*.

To check, if the program is working, you can start regression test::

  > make check

Build in docker (recommended)
+++++++++++++++++++++++++++++

.. note::

  This is also working on Windows or Mac OS! But the resulting program will only work on
  linux!

Load and create a ubuntu container in docker::

  > docker run -it -p 1212:1212 --name simulavr-build ubuntu:bionic

After the image from docker hub is loaded and the container *simulavr-build* is started, you see
the container prompt::

  root@f07fe9ded53b:/#
  
No worry if you see something other as "f07fe9ded53b", it's normal. "f07fe9ded53b" is the container
hash id. And this id is is for every container unique! Following I'll not show the prompt.::

  # first, we update the package list and do package updates, if necessary
  apt-get update
  apt-get upgrade -y
  # next we have to install the packages needed for building simulavr
  apt-get install -y g++ make cmake git python3
  # clone the repository
  cd /root
  git clone -b master https://git.savannah.nongnu.org/git/simulavr.git
  cd simulavr
  # optional, if you want to build a dedicated release (uncomment the hash)
  #git checkout rel-X.Y.Z
  # now we build the program
  make build
  # optional, just to check the build (uncomment the hash)
  #make check

Now the build is complete. For the moment we leave the docker container with the exit command::

  exit
