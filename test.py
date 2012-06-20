# -*- coding: utf-8 -*-
import csv
import sys
import unittest
import lapacho

class LapachoTestCase(unittest.TestCase):

    def test_hungarian(self):
        matrices = [
            # Square
            ([[400, 150, 400],
              [400, 450, 600],
              [300, 225, 300]],
             850 # expected cost
             ),
            # Rectangular variant
            ([[400, 150, 400, 1],
              [400, 450, 600, 2],
              [300, 225, 300, 3]],
              452 # expected cost
            ),
            # Square
            ([[10, 10,  8],
              [ 9,  8,  1],
              [ 9,  7,  4]],
              18
            ),
            # Rectangular variant
            ([[10, 10,  8, 11],
              [ 9,  8,  1, 1],
              [ 9,  7,  4, 10]],
              15
            ),
        ]

        for matrix, expected_cost in matrices:
            indexes = lapacho.hungarian(matrix)

            total_cost = 0
            for i, j in indexes:
                total_cost += matrix[i][j]

            self.assertEqual(expected_cost, total_cost)


if __name__ == '__main__':
    unittest.main()
