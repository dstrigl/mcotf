=============================================================
Implementation of the on-the-fly non-emptyness check for NBAs
=============================================================

Implementation of the algorithm of Yannakakis & al. to check whether L(A) = ∅ for a given NBA A.
It is implemented in C++ and produces nice LaTeX documents which illustrate the execution of the algorithm.

The tool is available under the `Boost Software License <http://www.boost.org/users/license.html>`_
which is also available from `www.boost.org <http://www.boost.org/>`_.

To input NBAs one has to encode the NBA directly into the source-file ``mcotf.cpp`` (examples are provided),
then compile the program via make and run it.

So, the overall procedure is as follows:

  * Download the source code of the tool.
  * Edit ``mcotf.cpp`` and encode your NBA directly inside the source file.
  * Build the executable using ``make``.
  * Run the tool ``./mcotf >output.tex`` to produce the LaTeX document.
  * Call ``pdflatex output.tex`` to create a PDF file from the LaTeX output.
  * Enjoy while reading ``output.pdf``.

