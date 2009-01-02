#!/usr/local/bin/python

import psycopg
import string
import time, re
from select import *


class SQLResult:

    Data = []

    def __init__(self, data):
        self.Data = data

    def __getitem__(self, key):
        return self.Data[key]

    def __len__(self):
        return len(self.Data)


class SQL:

    Connection = None
    DB = psycopg

    def __init__(self, database, username, password='',
                 hostname='localhost', port='5432'):

        self.Connection = self.DB.connect(database=database, user=username,
                                          password=password, host=hostname,
                                          port=port);

    def Query(self, query):
        cursor = self.Connection.cursor()

        cursor.setinputsizes(None)
        cursor.execute(query)

        if query[0] == 'S' or query[0] == 's':
            return SQLResult(cursor.fetchall())
        else:
            self.Connection.commit()
            return None

    def Commit(self):
        self.Connection.commit()


class SQLManage(SQL):

    Connections = {}

    def __init__(self):
        pass

    def Add(self, name, database, username, password='',
            hostname='localhost', port='5432'):

        self.Connections[name] = self.DB.connect(database=database, user=username,
                                                 password=password, host=hostname,
                                                 port=port);

    def Query(self, name, query):
        cursor = self.Connections[name].cursor()

        cursor.setinputsizes(None)
        cursor.execute(query)

        if query[0] == 'S' or query[0] == 's':
            return SQLResult(cursor.fetchall())
        else:
            self.Connections[name].commit()
            return None