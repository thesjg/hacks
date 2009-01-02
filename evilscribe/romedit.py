#!/usr/local/bin/python

from config import *


class ROMEdit:
    def __init__(self):
        self.image = []
        self.image_modified = False

        self.fuel_table = []
        self.fuel_table_modified = False

        self.timing_table = []
        self.timing_table_modified = False

    def close(self):
        self.__init__()

    def set_value(self, offset, value):
        self.image[long(offset, 16)] = chr(long(value, 16))
        self.image_modified = True

    def get_value(self, offset):
        return hex(ord(self.image[long(offset, 16)]))[2:]

    def import_image(self, filename):
          # Open image file in binary read mode
        in_file = open(filename, 'rb')

          # XXX: Error checking here, open file
        self.bin_image = in_file.read()

        for i in range(in_file.tell()):
            self.image.append(self.bin_image[i])

          # Close file
        in_file.close()

        temp_array = []
        j = 0
        for i in self.image[long(config.offset_fuel[0], 16):
                            long(config.offset_fuel[1], 16)+1]:
            if j == len(config.grid_fuel_cols):
                self.fuel_table.append(temp_array)
                temp_array = []
                j = 0

            temp_array.append(str(ord(i)))
            j = j + 1
        self.fuel_table.append(temp_array)

        temp_array = []
        j = 0
        for i in self.image[long(config.offset_timing[0], 16):
                            long(config.offset_timing[1], 16)+1]:
            i = int(str(ord(i))) - config.magic_timing

            if j == len(config.grid_timing_cols):
                self.timing_table.append(temp_array)
                temp_array = []
                j = 0

            temp_array.append(str(i))
            j = j + 1
        self.timing_table.append(temp_array)

    def export_image(self, filename):
        col = 0
        row = 0
        for i in range(long(config.offset_fuel[0], 16),
                       long(config.offset_fuel[1], 16)+1):
              # XXX: Magic
            if col == len(config.grid_fuel_cols):
                col = 0
                row = row + 1

            self.image[i] = chr(int(self.fuel_table[row][col]))
            col = col + 1

        col = 0
        row = 0
        for i in range(long(config.offset_timing[0], 16),
                       long(config.offset_timing[1], 16)+1):
              # XXX: Magic
            if col == len(config.grid_timing_cols):
                col = 0
                row = row + 1

            self.image[i] = chr(int(self.timing_table[row][col]) + config.magic_timing)
            col = col + 1

          # Error checking here, open output file and write to it
        self.out_file = open(filename, 'wb')
        for i in self.image:
          self.out_file.write(str(i))
        self.out_file.close()

        self.__init__()

    def set_rev_limit(self, rpm):
          # XXX: Verify that rpm is a valid int
        new_rev_limit = hex(config.magic_rev_limit/rpm)[2:]
        rev_limit_size = len(new_rev_limit)

        rev1 = ''
        rev2 = ''

        if rev_limit_size == 1:
            rev1 = '00'
            rev2 = '0' + new_rev_limit
        elif rev_limit_size == 2:
            rev1 = '00'
            rev2 = new_rev_limit
        elif rev_limit_size == 3:
            rev1 = '0' + new_rev_limit[0]
            rev2 = new_rev_limit[1:]
        elif rev_limit_size == 4:
            rev1 = new_rev_limit[:2]
            rev2 = new_rev_limit[2:]

        self.set_value(config.offset_rev_limit[0], rev1)
        self.set_value(config.offset_rev_limit[1], rev2)

    def get_rev_limit(self):
        rev_limit = self.get_value(config.offset_rev_limit[0])
        rev_limit = rev_limit + self.get_value(config.offset_rev_limit[1])

        return config.magic_rev_limit/long(rev_limit, 16)

    def set_idle_speed(self, rpm):
          # XXX: Verify that rpm is a valid integer less than 2040 rpm
        hex_idle_speed = hex(rpm/config.magic_idle_speed)[2:]

        if len(hex_idle_speed) == 1:
            hex_idle_speed = '0' + hex_idle_speed

        for i in config.offset_idle_speed:
            self.set_value(i, hex_idle_speed)
            print "Set Idle Speed: ", str(i), str(hex_idle_speed)

    def get_idle_speed(self):
          # XXX: We should compare the idle speed's for sanity

        idle_speed = long(self.get_value(config.offset_idle_speed[0]), 16)
        return config.magic_idle_speed*idle_speed

    def fuel_cut_get_value(self):
        return self.get_value(config.offset_fuel_cut)

    def fuel_cut_set_value(self, value):
        self.set_value(config.offset_fuel_cut, hex(value))

    def fuel_cut_enable(self):
        self.fuel_cut_set_value(self.config.magic_fuel_cut_enable)

    def fuel_cut_disable(self):
        self.fuel_cut_set_value(self.config.magic_fuel_cut_disable)

    def fuel_cut_status(self):
        """ False == Enabled, True == Disabled """

        offset = config.offset_fuel_cut
        value = self.get_value(offset)

        if value == 'ff':
            return True
        else:
            return False

    def fuel_table_get(self, row, column):
        try:
            return self.fuel_table[row][column]
        except:
            print 'EXCEPTION IN fuel_table_get()'
            print 'ROW:', row, 'COL:', column

    def fuel_table_set(self, row, column, value):
        self.fuel_table[row][column] = value
        self.fuel_table_modified = True

    def timing_table_get(self, row, column):
        try:
            return self.timing_table[row][column]
        except:
            print 'EXCEPTION IN timing_table_get()'
            print 'ROW:', row, 'COL:', column

    def timing_table_set(self, row, column, value):
        self.timing_table[row][column] = value
        self.timing_table_modified = True

    def get_injector_size(self):
        offset = config.offset_injector_compensation
        inj_hex = self.get_value(offset)
        return int(450/(float(long(inj_hex, 16))/74))

    def set_injector_size(self, cc):
        inj_hex = hex(int((450/float(cc))*74))
        offset = config.offset_injector_compensation
        self.set_value(offset, inj_hex)

    def get_injector_deadtime(self, row):
        offset = config.offset_injector_deadtime[row]
        return (long(self.get_value(offset), 16)*config.magic_deadtime)

    def set_injector_deadtime(self, row, time):
        """ Passed-in time should be in usec. """

        offset = config.offset_injector_deadtime[row]
        self.set_value(offset, hex((time/config.magic_deadtime)))

    def get_maf_comp(self, row):
        offset = config.offset_maf_compensation[row]
        return long(self.get_value(offset), 16)

    def set_maf_comp(self, row, value):
        offset = config.offset_maf_compensation[row]
        self.set_value(offset, hex(value))

    def get_maf_scalar(self, row):
        offset = config.offset_maf_scalar[row]
        return long(self.get_value(offset), 16)

    def set_maf_scalar(self, row, value):
        offset = config.offset_maf_scalar[row]
        self.set_value(offset, hex(value))

    def get_maf_multiplier(self):
        offset = config.offset_maf_multiplier
        return long(self.get_value(offset), 16)

    def set_maf_multiplier(self, value):
        offset = config.offset_maf_multiplier
        self.set_value(offset, hex(value))
