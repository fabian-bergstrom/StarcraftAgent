import unittest
import sys

sys.path.append('build/python-api-src')

from library import UnitTypeID, UNIT_TYPEID


class TestUnitType(unittest.TestCase):

    def test_equality(self):
        self.assertTrue(UNIT_TYPEID.TERRAN_ARMORY == UNIT_TYPEID.TERRAN_ARMORY)
    
    def test_inequality(self):
        self.assertFalse(UNIT_TYPEID.TERRAN_ARMORY != UNIT_TYPEID.TERRAN_ARMORY)

    def test_convert_equality(self):
        unit_typeid = UNIT_TYPEID.TERRAN_ARMORY
        self.assertTrue(unit_typeid == UnitTypeID(unit_typeid))
        self.assertTrue(UnitTypeID(unit_typeid) == unit_typeid)
        self.assertFalse(UnitTypeID(unit_typeid) != unit_typeid)

if __name__ == '__main__':
    unittest.main()

