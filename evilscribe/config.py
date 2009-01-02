#!/usr/local/bin/python

import string, ConfigParser

class EditorConfig(ConfigParser.SafeConfigParser):

    def parse(self, filename):

        file = open(filename)
        self.readfp(file)

        self.app_name    = self.get('app_info', 'name')
        self.app_version = self.get('app_info', 'version')
        self.app_author  = self.get('app_info', 'author')
        self.app_contact = self.get('app_info', 'contact')
        self.app_height  = int(self.get('app_info', 'height'))
        self.app_width   = int(self.get('app_info', 'width'))

        grid_fuel_rows   = self.get('grid_info', 'fuel_rows')
        grid_fuel_cols   = self.get('grid_info', 'fuel_cols')
        grid_timing_rows = self.get('grid_info', 'timing_rows')
        grid_timing_cols = self.get('grid_info', 'timing_cols')
        grid_injector_deadtime = self.get('grid_info', 'injector_deadtime')

        self.magic_timing     = int(self.get('magic_numbers', 'timing'))
        self.magic_rev_limit  = int(self.get('magic_numbers', 'rev_limit'))
        self.magic_idle_speed = int(self.get('magic_numbers', 'idle_speed'))
        self.magic_deadtime   = int(self.get('magic_numbers', 'deadtime'))
        self.magic_fuel_cut_enable = int(self.get('magic_numbers', 'fuel_cut_enable'))
        self.magic_fuel_cut_disable = int(self.get('magic_numbers', 'fuel_cut_disable'))

        self.max_idle = int(self.get('min_max', 'max_idle'))
        self.min_idle = int(self.get('min_max', 'min_idle'))
        self.max_rev = int(self.get('min_max', 'max_rev'))
        self.min_rev = int(self.get('min_max', 'min_rev'))

        offset_fuel       = self.get('offsets', 'fuel')
        offset_timing     = self.get('offsets', 'timing')
        offset_rev_limit  = self.get('offsets', 'rev_limit')
        offset_idle_speed = self.get('offsets', 'idle_speed')
        self.offset_fuel_cut   = self.get('offsets', 'fuel_cut')
        self.offset_injector_compensation = self.get('offsets', 'injector_compensation')
        offset_injector_deadtime = self.get('offsets', 'injector_deadtime')
        offset_maf_compensation = self.get('offsets', 'maf_compensation')
        offset_maf_scalar = self.get('offsets', 'maf_scalar')
        self.offset_maf_multiplier = self.get('offsets', 'maf_multiplier')

        self.grid_fuel_rows = string.split(grid_fuel_rows)
        self.grid_fuel_cols = string.split(grid_fuel_cols)
        self.grid_timing_rows = string.split(grid_timing_rows)
        self.grid_timing_cols = string.split(grid_timing_cols)
        self.grid_injector_deadtime = string.split(grid_injector_deadtime)

        self.offset_fuel = string.split(offset_fuel)
        self.offset_timing = string.split(offset_timing)
        self.offset_rev_limit = string.split(offset_rev_limit)
        self.offset_idle_speed = string.split(offset_idle_speed)
        self.offset_injector_deadtime = string.split(offset_injector_deadtime)
        self.offset_maf_compensation = string.split(offset_maf_compensation)
        self.offset_maf_scalar = string.split(offset_maf_scalar)

        inj = 0
        self.injectors = []
        while (1):
            try:
                inj_str = self.get('injectors', 'injector%s' % (str(inj)))
                self.injectors.append(string.split(inj_str, ' ', 9))
                inj = inj + 1
            except ConfigParser.NoOptionError:
                break

        maf = 0
        self.mafs = []
        while (1):
            try:
                maf_str = self.get('maf', 'maf%s' % (str(maf)))

                split_str = str.split(maf_str, ' ', 24)

                comp = split_str[0:21]
                scalar = split_str[21:23]
                multiplier = split_str[23]
                type = split_str[24]

                maf_tmp = []
                maf_tmp.append(comp)
                maf_tmp.append(scalar)
                maf_tmp.append(multiplier)
                maf_tmp.append(type)
                self.mafs.append(maf_tmp)

                maf = maf + 1
            except ConfigParser.NoOptionError:
                break

global config
config = EditorConfig()
config.parse('romedit.conf')

#for i in config.mafs:
#    print i[3]
#    for j in i:
#        print j[2]

#for i in config.offset_maf_scalar:
#    print i

#for i in range(len(config.offset_maf_scalar)):
#    print i
