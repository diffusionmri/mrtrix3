.. _label2colour:

label2colour
===========

Synopsis
--------

::

    label2colour [ options ]  nodes_in colour_out

-  *nodes_in*: the input node parcellation image
-  *colour_out*: the output colour image

Description
-----------

convert a parcellated image (where values are node indices) into a colour image (many software packages handle this colouring internally within their viewer program; this binary explicitly converts a parcellation image into a colour image that should be viewable in any software)

Options
-------

-  **-lut file** Provide the relevant colour lookup table (if not provided, nodes will be coloured randomly)

Standard options
^^^^^^^^^^^^^^^^

-  **-info** display information messages.

-  **-quiet** do not display information messages or progress status.

-  **-debug** display debugging messages.

-  **-force** force overwrite of output files. Caution: Using the same file as input and output might cause unexpected behaviour.

-  **-nthreads number** use this number of threads in multi-threaded applications (set to 0 to disable multi-threading)

-  **-failonwarn** terminate program if a warning is produced

-  **-help** display this information page and exit.

-  **-version** display version information and exit.

--------------



**Author:** Robert E. Smith (robert.smith@florey.edu.au)

**Copyright:** Copyright (c) 2008-2016 the MRtrix3 contributors

This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/

MRtrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

For more details, see www.mrtrix.org
