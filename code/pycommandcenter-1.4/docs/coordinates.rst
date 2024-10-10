Coordinates
===========

The library uses 2 types of coordinate classes. One for integers and one for 
floats, these are called :class:`library.Point2DI` and 
:class:`library.Point2D`. Conversion between the two types is possible by
sending the other type as argument in the constructor. In the case of 
Point2D to Point2DI the floating point values will just be cast into
integer values.

Both of the types are hashable so they can be used in sets and dictionaries. 
Note that it is the tuple (x_val, y_val) that will be hashed so don't mix 
those sets or dictionaries with normal tuples.

Point2DI
--------


.. class:: library.Point2DI

   These points are used for defining the location of tiles, which are used when 
   placing buildings in Starcraft. 

   Not many operators are defined for this class, only equality, inequality and
   converting to strings are defined.


Point2D
-------

.. class:: library.Point2D

   Instances of this class is used to represent the location of Units and
   support many operations for general purpose calculation.

Example
~~~~~~~

.. code:: python

   from library import Point2D, Point2DI
   
   # Creating two points

   p1 = Point2D()  # Defaults to (0, 0)
   p2 = Point2D(1, 2)
   
   # Adding two points
   p1 += p2

   # Multiplying a point with a scalar
   p2 *= 3
   
   # Creating a new point, defined as the sum of p1 and p2
   p3 = p1 + p2
   
   print(p1)
   print(p2)
   print(p3)
   
   # The same works for Point2DI
   pi1 = Point2DI(1, 2)
   pi2 = Point2DI(1, 2)
   
   # However, we can only compare them. There are no operators for addition,
   # subtraction or multiplication.
   print(pi1 == pi2)  # prints: True


It is also possible to define custom operators for these points, as shown
below:

.. code:: python

   from math import sqrt
   from library import Point2D

   p1 = Point2D(3, 4)
   p2 = Point2D(1, 2)
   
   # Defining a custom @ operator, like this
   Point2D.__matmul__ = lambda self, other: self.x * other.x + self.y * other.y

   print(p1 @ p2)  # prints: 11.0

   # Or maybe a distance operator:
   Point2D.distance = lambda self, other: sqrt((self.x - other.x)**2 + (self.y - other.y)**2)

   print(p1.distance(p2))  # prints: 2.8284...

.. toctree::