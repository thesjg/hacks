#!/usr/local/bin/python

from backend import sql
import string


QueryBuilder = sql.SQLQueryBuilder()


class server_db_logins(sql.SQLNotifyTable):

   def __name__(self):
       return 'server_db_logins'

   def GetCols(self):
       return ['server_id', 'username', 'password', 'database']


class user_server_quota(sql.SQLNotifyTable):

   def __name__(self):
       return 'user_server_quota'

   def GetCols(self):
       return []


class server_config(sql.SQLNotifyTable):

   def __name__(self):
       return 'server_config'

   def GetCols(self):
       return []


class users(sql.SQLNotifyTable):

   def __name__(self):
       return 'users'

   def GetCols(self):
       return []


class dns_zones(sql.SQLNotifyTable):

   def __name__(self):
       return 'dns_zones'

   def GetCols(self):
       return []


class dns_resources(sql.SQLNotifyTable):

   def __name__(self):
       return 'dns_resources'

   def GetCols(self):
       return []


class web_sites(sql.SQLNotifyTable):

   def __name__(self):
       return 'web_sites'

   def GetCols(self):
       return []


class web_aliases(sql.SQLNotifyTable):

   def __name__(self):
       return 'web_aliases'

   def GetCols(self):
       return []
