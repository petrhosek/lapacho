# Copyright 2012 Petr Hosek
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2,
# as published by the Free Software Foundation.
#
# In addition to the permissions in the GNU General Public License,
# the authors give you unlimited permission to link the compiled
# version of this file into combinations with other programs,
# and to distribute those combinations without any restriction
# coming from the use of this file.  (The General Public License
# restrictions do apply in other respects; for example, they cover
# modification of the file, and distribution when not linked into
# a combined executable.)
#
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

def pad_matrix(matrix, pad_value=0):
    """
    Pad a possibly non-square matrix to make it square.

    :Parameters:
        matrix : list of lists
            matrix to pad

        pad_value : int
            value to use to pad the matrix

    :rtype: list of lists
    :return: a new, possibly padded, matrix
    """
    max_columns = 0
    total_rows = len(matrix)

    for row in matrix:
        max_columns = max(max_columns, len(row))

    total_rows = max(max_columns, total_rows)

    new_matrix = []
    for row in matrix:
        row_len = len(row)
        new_row = row[:]
        if total_rows > row_len:
            # Row too short. Pad it.
            new_row += [0] * (total_rows - row_len)
        new_matrix += [new_row]

    while len(new_matrix) < total_rows:
        new_matrix += [[0] * total_rows]

    return new_matrix
