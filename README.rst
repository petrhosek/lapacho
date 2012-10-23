lapacho
=======

Algorithms for the linear assignment problem implemented in C as Python module.

Installing and Running
----------------------

To build and install this module, run the following::

    $ git clone git://github.com/petrh/lapacho.git
    $ cd lapacho
    $ python setup.py install
    $ python setup.py test

Source is available at http://github.com/petrh/lapacho.

Included Algorithms
-------------------

Linear assignment problem:

* Hungarian (Kuhn-Munkres)

Example Usage
-------------

    >>> import lapacho
    >>> lapacho.hungarian([[10, 10,  8],
    ...                   [ 9,  8,  1],
    ...                   [ 9,  7,  4]])
    [(0, 0), (1, 2), (2, 1)]

Authors
-------

* Petr Hosek <petrhosek@gmail.com>

License
-------

BSD license
