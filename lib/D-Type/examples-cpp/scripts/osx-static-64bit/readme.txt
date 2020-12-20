Requirements for building D-Type examples on Mac OS X (Cocoa):
--------------------------------------------------------------

- Mac OS X 10.8 or newer
- Xcode Developer Tools with clang


How to build D-Type examples from the command line using the clang compiler?
----------------------------------------------------------------------------

First, open Terminal and change your current working directory to the directory
containing this readme.txt file.

Step 1.

  From the command line, type

  ./01-make_dtwindow

  This will build D-Type's Helper Library For Platform Independent Window Display
  and copy it to the appropriate sub-directory in the /other/libs directory.

Step 2.

  From the command line, type

  ./02-make_all_examples

  This will build all the supplied D-Type examples.

Step 3.

  From the command line, type

  ./03-copy_all_examples_to_exec

  This will copy the examples built in the previous step to the appropriate
  sub-directory in the exec directory.

If your compiler and linker have been set up correctly, at no point during the build
process should you see any compile or link errors. On a typical machine the entire
build process takes just a few seconds.

Once all these steps are completed, you may run the "00-cleanup" script to remove
any temporary files created during the build process.

To run the examples, go to the exec directory and open one of its sub-directories.
Run the executables either from the command line or by double clicking their icon.

Example:

cd ../../../exec/a-rasterizer
./example_color_rasterizer_osx
