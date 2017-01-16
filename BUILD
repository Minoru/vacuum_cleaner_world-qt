How to build the project
========================

Linux
-----

First way. Install whole Qt SDK:

1. Type in terminal:

	$ sudo apt-get install qt-sdk

2. Run QtCreator and start coding.


Second way. Install separate components:

1. Install Qt5. Make sure you have qmake (it might be in a separate package).

2. If you want some IDE, install QtCreator:

     $ sudo aptitude install qtcreator

   After that you can just open Agent.pro and start hacking.

3. If you want to use your favourite text editor or IDE, you'll need to
   generate Makefile. Just run the following command:

     $ qmake -o Makefile Agent.pro

   and then use make to build the executable binary:

     $ make

   You can clean working directory using
    
     $ make clean

   If your machine happen to have more than 1 core, you can use -j option to
   speed up compilation.

Windows
-------

1. Install libraries. There are two options:

   a) install libraries only. In that case you'll have to use some IDE
      (probably Microsoft Visual Studio) for development. Libraries pack is
      about 275MB in size.

   b) install Qt SDK. It would provide not only libraries but also QtCreator, a
      nice IDE. Note, though, that SDK is about 1.3GB in size.

   Both libraries and SDK can be downloaded from http://qt.nokia.com/downloads

2. If you installed SDK, just fire up QtCreator, open the project (Agent.pro)
   and start hacking.

3. If you decided to stick to MSVS, you'll need to generate project file. To do
   that, go to Start - All Programs - Qt by Nokia v4.8.0 (VS2010 OpenSource) -
   Qt 4.8.0 Command Prompt (versions may differ), wait till initialization is
   finished (">" sign should appear), go to the directory with project (e.g. cd
   C:\Agent) and run the following command:

     > qmake -tp vc

4. Open generated project in Visual Studio and create some nice bots.

