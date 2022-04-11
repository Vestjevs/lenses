###
# Этот скрипт содержит математические формулы, которые
# будут использоваться в решение задачи про Линзы.
# Состав:
# Полина
# Кирилл
# Элина
# Алексей (27.03.22)
###

import numpy as np


class Vector:
    def __init__(self, x, y, z):
        self.x, self.y, self.z = x, y, z
        self.__arr = np.array([self.x, self.y, self.z])

    @classmethod
    def __from_vector(cls, arr):
        return cls(arr[0], arr[1], arr[2])

    def __array__(self, dtype=None):
        if dtype:
            return np.array([self.x, self.y, self.z], dtype=dtype)
        else:
            return np.array([self.x, self.y, self.z])

    def __iter__(self):
        return self.__arr.__iter__()

    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y, self.z + other.z)

    def __mul__(self, other):
        return np.inner(self.__arr, other)

    def __sub__(self, other):
        return self.__arr - other

    def cross(self, other):
        return Vector.__from_vector(np.cross(self.__arr, other))

    def len(self):
        return np.sqrt(np.sum([x * x for x in self.__arr]))

    def __str__(self):
        return f'x - {self.x}, y - {self.y}, z - {self.z}'


class Point:
    def __init__(self, x, y, z):
        self.x, self.y, self.z = x, y, z
        self.__arr = np.array([self.x, self.y, self.z])

    def __iter__(self):
        return self.__arr.__iter__()
