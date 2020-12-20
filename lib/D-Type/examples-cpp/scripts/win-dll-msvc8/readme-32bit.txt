Requirements for building D-Type examples on Windows:
-----------------------------------------------------

- MS Windows with a working MS Visual C++ 8.0 installation


How to build D-Type examples from the command line using MS Visual C++ 8.0?
---------------------------------------------------------------------------

  Open msvc8-32bit.bat in your text editor and, if necessary, modify the paths
  to reflect the location of your MS Visual C++ 8.0 installation.

Step 1.

  Run the "01-make_dtwindow-32bit.bat" batch file (either from the command line
  or by double clicking its icon). This will build D-Type's Helper Library For
  Platform Independent Window Display and copy it to the appropriate
  sub-folder in the \other\libs folder.

Step 2.

  Run the "02-make_all_examples-32bit.bat" batch file (either from the command
  line or by double clicking its icon). This will build all the supplied D-Type
  examples.

Step 3.

  Run the "03-copy_all_examples_to_exec.bat" batch file (either from the
  command line or by double clicking its icon). This will copy the examples
  built in the previous step to the appropriate sub-folder in the exec folder.

If your Microsoft Visual Studio 8.0 has been set up correctly, at no point
during the build process should you see any compile or link errors. On a
typical machine the entire build process takes just a few seconds.

Once all these steps are completed, you may run the "00-cleanup.bat" batch
file to remove any temporary files created during the build process.

To run the examples, go to the exec folder and open one of its sub-folders.
Run the executables either from the command line or by double clicking their
icon.
