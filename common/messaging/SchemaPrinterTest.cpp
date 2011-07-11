/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * SchemaPrinterTest.cpp
 * Test fixture for the SchemaPrinter class.
 * Copyright (C) 2011 Simon Newton
 */

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <vector>

#include "ola/messaging/Descriptor.h"
#include "ola/messaging/SchemaPrinter.h"

using std::string;
using std::vector;


using ola::messaging::BoolFieldDescriptor;
using ola::messaging::Descriptor;
using ola::messaging::FieldDescriptor;
using ola::messaging::FieldDescriptorGroup;
using ola::messaging::SchemaPrinter;
using ola::messaging::StringFieldDescriptor;
using ola::messaging::UInt16FieldDescriptor;
using ola::messaging::UInt32FieldDescriptor;
using ola::messaging::UInt8FieldDescriptor;

class SchemaPrinterTest: public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SchemaPrinterTest);
  CPPUNIT_TEST(testPrinter);
  CPPUNIT_TEST(testGroupPrinter);
  CPPUNIT_TEST(testIntervalsAndLabels);
  CPPUNIT_TEST_SUITE_END();

  public:
    SchemaPrinterTest() {}
    void testPrinter();
    void testGroupPrinter();
    void testIntervalsAndLabels();
};


CPPUNIT_TEST_SUITE_REGISTRATION(SchemaPrinterTest);


/*
 * Test the SchemaPrinter
 */
void SchemaPrinterTest::testPrinter() {
  // setup some fields
  BoolFieldDescriptor *bool_descriptor = new BoolFieldDescriptor("On/Off");
  StringFieldDescriptor *string_descriptor = new StringFieldDescriptor(
      "Name", 0, 32);
  UInt8FieldDescriptor *uint8_descriptor = new UInt8FieldDescriptor(
      "Count", false, 10);

  // try a simple print first
  vector<const FieldDescriptor*> fields;
  fields.push_back(bool_descriptor);
  fields.push_back(string_descriptor);
  fields.push_back(uint8_descriptor);

  Descriptor test_descriptor("Test Descriptor", fields);
  SchemaPrinter printer(false, false);
  test_descriptor.Accept(printer);

  string expected = "On/Off: bool\nName: string [0, 32]\nCount: uint8\n";
  CPPUNIT_ASSERT_EQUAL(expected, printer.AsString());
}


void SchemaPrinterTest::testGroupPrinter() {
  BoolFieldDescriptor *bool_descriptor = new BoolFieldDescriptor("On/Off");
  StringFieldDescriptor *string_descriptor = new StringFieldDescriptor(
      "Name", 0, 32);
  StringFieldDescriptor *string_descriptor2 = new StringFieldDescriptor(
      "Device", 0, 32);
  UInt8FieldDescriptor *uint8_descriptor = new UInt8FieldDescriptor(
      "Count", false, 10);
  UInt32FieldDescriptor *uint32_descriptor = new UInt32FieldDescriptor("Id");

  vector<const FieldDescriptor*> fields;
  fields.push_back(bool_descriptor);
  fields.push_back(string_descriptor);
  fields.push_back(uint8_descriptor);

  // now do a descriptor which contains a GroupDescriptor
  FieldDescriptorGroup *group_descriptor = new FieldDescriptorGroup(
      "Group 1", fields, 0, 2);
  vector<const FieldDescriptor*> fields2;
  fields2.push_back(string_descriptor2);
  fields2.push_back(uint32_descriptor);
  fields2.push_back(group_descriptor);
  Descriptor test_descriptor("Test Descriptor2", fields2);

  SchemaPrinter printer(false, false);
  test_descriptor.Accept(printer);

  string expected = "Device: string [0, 32]\nId: uint32\nGroup 1 {\n"
    "  On/Off: bool\n  Name: string [0, 32]\n  Count: uint8\n}\n";
  CPPUNIT_ASSERT_EQUAL(expected, printer.AsString());
}


void SchemaPrinterTest::testIntervalsAndLabels() {
  UInt16FieldDescriptor::IntervalVector intervals;
  intervals.push_back(UInt16FieldDescriptor::Interval(2, 8));
  intervals.push_back(UInt16FieldDescriptor::Interval(12, 14));

  UInt16FieldDescriptor::LabeledValues labels;
  labels["dozen"] = 12;
  labels["bakers_dozen"] = 13;

  UInt16FieldDescriptor *uint8_descriptor = new UInt16FieldDescriptor(
      "Count", intervals, labels);
  vector<const FieldDescriptor*> fields;
  fields.push_back(uint8_descriptor);
  Descriptor test_descriptor("Test Descriptor", fields);

  SchemaPrinter interval_printer(true, false);
  test_descriptor.Accept(interval_printer);
  string expected = "Count: uint16, (2, 8) (12, 14)\n";
  CPPUNIT_ASSERT_EQUAL(expected, interval_printer.AsString());

  SchemaPrinter label_printer(false, true);
  test_descriptor.Accept(label_printer);
  string expected2 = "Count: uint16\n  bakers_dozen: 13\n  dozen: 12\n";
  CPPUNIT_ASSERT_EQUAL(expected2, label_printer.AsString());

  SchemaPrinter interval_label_printer(true, true);
  test_descriptor.Accept(interval_label_printer);
  string expected3 = (
      "Count: uint16, (2, 8) (12, 14)\n  bakers_dozen: 13\n  dozen: 12\n");
  CPPUNIT_ASSERT_EQUAL(expected3, interval_label_printer.AsString());
}
