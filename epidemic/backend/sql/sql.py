#!/usr/local/bin/python

import psycopg2
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
    DB = psycopg2

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


class SQLQueryBuilder:

    def __init__(self):
        pass

    def UPDATE(self, table, where, old_dict, new_dict):
        modified = {}

        for i in old_dict.keys():
            for j in new_dict.keys():
                if i == j:
                    if old_dict[i] != new_dict[j]:
                        modified[j] = new_dict[j]

        query = 'UPDATE %s SET ' % table

        for i in modified.keys():
            query = query + "%s='%s'," % (i, modified[i])

        query = query[:-1]

        if where != None:
            query = query + " WHERE"

            for i in where.keys():
                query = query + " %s='%s'" % (i, where[i])

        return query

    def INSERT(self, table, dict):
        query = "INSERT INTO %s (" % table

        for i in dict.keys():
            query = query + str(i) + ", "

        query = query[:-2]
        query = query + ') VALUES ('

        for i in dict.keys():
            query = query + "'" + str(dict[i]) + "', "

        query = query[:-2]
        query = query + ')'

        return query