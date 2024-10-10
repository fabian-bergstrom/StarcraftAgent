Unit
====

.. class:: library.Unit

   An instance of the class Unit represents one unit in the game. The units are 
   not limited to moveable units, but every entity which is not part of the 
   background is a unit. For example, the minerals and geysers are units as 
   well as all buildings.

   For all possible types of units see the enum :class:`library.UNIT_TYPEID`. 
   Some types of objects are almost the same, for example there are many types 
   of mineral deposits, but all of them are mineable. This is one of the 
   motivations behind the :class:`library.UnitType` which aims to to make the 
   list of types more manageable. The UnitType can be accessed by the 
   :any:`Unit.unit_type` property.

   It is possible to use Unit as keys in a dictionary, which might be helpful
   for bookkeeping.

   Properties:

   .. autoattribute:: buffs
   .. autoattribute:: build_percentage
   .. autoattribute:: energy
   .. autoattribute:: facing
   .. autoattribute:: has_target
   .. autoattribute:: hit_points 
   .. autoattribute:: max_hit_points
   .. autoattribute:: max_shields
   .. autoattribute:: max_energy
   .. autoattribute:: id 
   .. autoattribute:: is_alive 
   .. autoattribute:: is_blip
   .. autoattribute:: is_being_constructed 
   .. autoattribute:: is_burrowed 
   .. autoattribute:: is_cloaked 
   .. autoattribute:: is_completed 
   .. autoattribute:: is_flying 
   .. autoattribute:: is_idle 
   .. autoattribute:: is_powered 
   .. autoattribute:: is_training 
   .. autoattribute:: is_valid 
   .. autoattribute:: player 
   .. autoattribute:: position 
   .. autoattribute:: current_ability_id
   .. autoattribute:: progress
   .. autoattribute:: progression_list
   .. autoattribute:: radius
   .. autoattribute:: shields 
   .. autoattribute:: tile_position 
   .. autoattribute:: unit_type 
   .. autoattribute:: weapon_cooldown 
   .. autoattribute:: is_carrying_minerals
   .. autoattribute:: is_carrying_gas
   .. autoattribute:: target
   .. autoattribute:: gas_left_in_refinery
   .. autoattribute:: minerals_left_in_mineralfield
   .. autoattribute:: owner

   Methods:

   .. automethod:: ability

      Call an ability directly, different abilities has different targets. Some target the unit itself (no argument), target a point (Point2D as argument) and some target a Unit (instance of Unit as argument)

   .. automethod:: is_constructing
   .. automethod:: stop
   .. automethod:: attack_unit
   .. automethod:: attack_move
   .. method:: Unit.move(self, point)

      Move the unit to the given point, the point being an instance of either 
      :class:`library.Point2D` or :class:`library.Point2DI`.

   .. automethod:: right_click
   .. automethod:: repair
   .. automethod:: build
   .. automethod:: build_target
   .. automethod:: train
   .. automethod:: research
   .. automethod:: morph
   .. automethod:: hold_position
   .. automethod:: patrol
   .. automethod:: stop_dance

      
.. toctree::