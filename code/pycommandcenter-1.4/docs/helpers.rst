Helpers
=======

There are several classes related to processing information about the game
state and the input from Starcraft. All these classes are taken more or less
directly from the original `CommandCenter`_, but we have left all
decision-making to the user of the API.

.. _CommandCenter: https://github.com/davechurchill/commandcenter

Here is a full list of all managers:

* :class:`library.BaseLocationManager`
* :class:`library.TechTree`
* :class:`library.MapTools`
* :class:`library.BuildingPlacer`

The rest of this page contains a brief description of each manager.

BaseLocationManager
-------------------

.. class:: library.BaseLocationManager

   .. autoattribute:: base_locations

   .. autoattribute:: starting_base_locations

   .. automethod:: get_occupied_base_locations

   .. automethod:: get_player_starting_base_location

   .. automethod:: get_next_expansion

BaseLocation
~~~~~~~~~~~~

.. class:: library.BaseLocation

   Closely related to BaseLocationManager. This is the datastructure used by
   the BaseLocationManager to keep track of all base locations and related
   information.

   .. autoattribute:: position

   .. autoattribute:: depot_position

   .. autoattribute:: minerals

   .. autoattribute:: geysers

   .. automethod:: get_ground_distance
      
      This function uses BFS and moves in a vertical and horizontal position. Because of this,
      the distance might overshoot compared to calculating it with Pythagoras' theorem.

   .. automethod:: is_occupied_by_player

   .. automethod:: is_player_start_location

   .. automethod:: contains_position


TechTree
--------

.. class:: library.TechTree

   This class contains all information about units and what is required to
   build a certain unit and what builds it. It only has one method, which is
   used to look-up unit types properties:

   This class has some invalid information by default, this can be corrected by
   placing the file `techtree.json` in the working directory. The
   `techtree.json` files are available here_, select the lastest version.
   A techtree for SC2 version >=4.10 is now available (not fully tested yet),
   this can be found under the folder data in this link_.
   A recent file is included in the `template repository`_.

   Instead of using TechTree, it's possible to use the functions in UnitType for
   structure, etc. In IDABot there is functions for getting data about upgrades.	

   .. method:: get_data(argument) -> library.TypeData

      Argument is either an instance of the class :class:`library.UnitType` or
      an instance of the class :class:`library.CCUpgrade`, depending on what
      information is wanted.

.. _link: https://github.com/BurnySc2/sc2-techtree
.. _here: https://github.com/noorus/sc2-gamedata
.. _`template repository`: https://gitlab.liu.se/starcraft-ai-course/sc2-python-bot

TypeData
~~~~~~~~

.. autoclass:: library.TypeData
   :members:
   :undoc-members:


.. TODO: Types for all fields would be nice, is required_units List[UnitType] or List[UNIT_TYPEID]?

MapTools
--------

.. autoclass:: library.MapTools
   :members:
   :undoc-members:

   This class contains two types of methods:

   * Methods for drawing information to the screen
   * Methods for extracting information about the map

   First, let us look at the method concerning drawing information to the 
   screen. Methods with the suffix ``_screen`` takes percentages of the 
   screens height and width, i.e. values between 0 and 1. Methods without 
   this suffix uses the same coordinate system as the game, i.e. world 
   coordinates.

   The top three methods below takes in a Point2D, but it's possible to
   send in x and y as floats instead of Point2D.

   There is also methods which are useful for extracting information about the 
   game map.

Color
~~~~~

.. autoclass:: library.Color 	
   :members:
   :undoc-members:

   .. automethod:: __init__

DistanceMap
~~~~~~~~~~~

.. autoclass:: library.DistanceMap
   :members:
   :undoc-members:

BuildingPlacer
--------------

.. autoclass:: library.BuildingPlacer 	
   :members:
   :undoc-members:

   This class is useful for placing all buildings, except refineries and town halls (Command Centers, Hacheries and Nexus).

   If you want to place a town hall, take a look at attribute `depot_location` of :class:`library.BaseLocation`.

   If you want to place a refinery, take a look at attribute `geysers` of :class:`library.BaseLocation` and the method build_target of :class:`library.Unit`.

.. toctree::