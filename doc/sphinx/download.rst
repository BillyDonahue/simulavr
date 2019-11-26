Download
========

Project homepage is available at https://savannah.nongnu.org/projects/simulavr.
There you'll find also a link to download area. If you want to download other
versions, please use the link to download area!

Secure download
---------------

.. note:: Replace X.Y.Z with the real release, this is for example |release|.

Releases are secured by gpg signatures. For every package, tarball, document,
which you can download here, you'll find a signature file too. This is a
cryptographic checksum over the released file and helps you to find out, if
this file is unchanged by somebody unauthorized.

For this, you need a ``gpg`` installation and our
`gpg keyring <https://savannah.nongnu.org/project/memberlist-gpgkeys.php?group=simulavr>`__.
Download this keyring and import it to your keyring::
  
  > gpg --import simulavr-keyring.gpg
  
You can list out, what's now in your keyring::
  
  > gpg --list-keys
  
After you have downloaded release file (tarball, document, binary package) together
with the signature file, you can verify, that your download is correct (for
example, you've downloaded ``simulavr-X.Y.Z.tar.gz`` together with
``simulavr-X.Y.Z.tar.gz.sig``)::
  
  > gpg --verify simulavr-X.Y.Z.tar.gz.sig
  
If there is no message, that file is invalid, you can use your downloaded file.
(of course, you can use it also without verifying signature, but on your own
risk!)

Release files
-------------

.. include:: download.txt

More other files and versions are available on download area on project homepage!
