Types
=====

For a full list of all unit types see the enum :class:`library.UNIT_TYPEID`.
For more information about a certain unit type, the wrapper class
:class:`library.UnitType` can be used.

For a full list of all abilities and upgrades, see :class:`library.ABILITY_ID`
, :class:`library.UPGRADE_ID` and :class:`library.EFFECT_ID` respectively. Note that these does not have any
wrapper classes for them.

If any of the ID's doesn't respond with the game. Before contacting the labassistent, check the game version
and the compatibility through Liquipedia. Some ID's might be left after a patch and is not available in
the game anymore.

UnitType
--------

As explained above, this class is a wrapper around the class
:class:`library.UNIT_TYPEID`.

.. autoclass:: library.UnitType
   :members:
   :undoc-members:
   :special-members: __init__

UNIT_TYPEID
-----------

.. autoclass:: library.UNIT_TYPEID
   :members:
   :undoc-members:

ABILITY_ID
----------

.. autoclass:: library.ABILITY_ID 	
   :members:
   :undoc-members:

UPGRADE_ID
----------

.. autoclass:: library.UPGRADE_ID 	
   :members:
   :undoc-members:

EFFECT_ID
----------
EffectID is for things like ravager bile, or fungal or even a scan.

.. autoclass:: library.EFFECT_ID 	
   :members:
   :undoc-members:

.. toctree::