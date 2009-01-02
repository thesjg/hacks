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


global config
config = EditorConfig()
config.parse('romedit.conf')
