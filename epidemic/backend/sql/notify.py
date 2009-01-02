from sql import *

class SQLNotify(SQL):

    Name = None
    Data = []

    def __init__(self, name, database, username, password='',
                 hostname='localhost', port='5432'):
        SQL.__init__(self, database, username, password, hostname, port)

        self.Name = name

    def Notify(self, name):
        self.Query('NOTIFY %s' % (name))

    def Register(self, table_name, class_instance):
        dict = {}
        dict['table_name'] = table_name
        dict['class'] = class_instance
        dict['class_name'] = dict['class'].__name__
        dict['cols'] = dict['class'].GetCols()
        dict['operations'] = {}

        for i in ['INSERT', 'UPDATE', 'DELETE']:
            try:
                dict['operations'][i] = True
            except:
                dict['operations'][i] = False

        self.Data.append(dict)
