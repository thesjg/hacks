#!/usr/local/bin/python

import string

from backend import sql, quota
import dir

QueryBuilder = sql.SQLQueryBuilder()
FSRootQuery = "SELECT fsroot FROM mail_config"


class mail_users(sql.SQLNotifyTable):

   DIR = None
   Quota = None

   def __name__(self):
       return 'mail_users'

   def GetCols(self):
       return ['gid', 'dba', 'username', 'quota_bytes']

   def __init__(self, db):
       sql.SQLNotifyTable.__init__(db)

       fsroot = self.DB.Query(FSRootQuery)[0][0]
       self.DIR = dir.MailDirStructure()
       self.DIR.Set(fsroot=self.fsroot)

       self.Quota = quota.Quota(fsroot)

   def INSERT(self, row_id, new_dict):

       self.DIR.Set(gid=new_dict['gid'], dba=new_dict['dba'],
                    groupname=new_dict['username'])
       self.DIR.CreateFSRoot()
       self.DIR.CreateDBA()
       self.DIR.CreateGroup()

       self.Quota.SetGroup(new_dict['gid'], quota_bytes)
       self.Quota.Apply()

   def UPDATE(self, row_id, old_dict, new_dict):

       self.DIR.Set(gid=old_dict['gid'], dba=old_dict['dba'],
                    groupname=old_dict['username'])

       if old_dict['gid'] != new_dict['gid']:
             # XXX: Should be able to change permissions
           pass

       if old_dict['dba'] != new_dict['dba']:
             # XXX: Should we allow dba name changes?
           pass

       if old_dict['username'] != new_dict['username']:
           self.DIR.RenameUser(new_dict['username'])

       if old_dict['quota_bytes'] != new_dict['quota_bytes']:
           self.Quota.SetGroup(new_dict['gid'], new_dict['quota_bytes'])
           self.Quota.Apply()

   def DELETE(self, row_id, old_dict):

       self.DIR.Set(gid=old_dict['gid'], dba=old_dict['dba'],
                    groupname=old_dict['username'])
       self.DIR.RemoveGroup()

       self.Quota.SetGroup(old_dict['gid'], 0)
       self.Quota.Apply()


class mail_accounts(sql.SQLNotifyTable):

   DIR = None
   Quota = None
   FSRoot = None

   def __name__(self):
       return 'mail_accounts'

   def GetCols(self):
       return ['uid', 'username', 'path', 'quota_bytes']

   def __init__(self, db):
       sql.SQLNotifyTable.__init__(db)

       self.FSRoot = self.DB.Query(FSRootQuery)[0][0]
       self.DIR = dir.MailDirStructure()
       self.DIR.Set(fsroot=self.FSRoot)

       self.Quota = quota.Quota(fsroot)

   def INSERT(self, row_id, new_dict):

       split_fsroot = string.split(new_dict['path'], '/')[1:]
       dba = split_fsroot[-5]
       groupname = split_fsroot[-4]
       domain = split_fsroot[-3]

       gid = self.DB.Query("""SELECT gid FROM mail_users WHERE dba='%s'
                              AND username='%s'""" % (dba, groupname))[0][0]

       self.DIR.Set(gid=gid, dba=dba, groupname=groupname, domain=domain,
                    username=new_dict['username'])
       self.DIR.CreateAll()

       self.Quota.SetUser(new_dict['uid'], new_dict['quota_bytes'])
       self.Quota.Apply()

   def UPDATE(self, row_id, old_dict, new_dict):

       split_fsroot = string.split(old_dict['path'], '/')[1:]
       old_dba = split_fsroot[-5]
       old_groupname = split_fsroot[-4]
       old_domain = split_fsroot[-3]

       split_fsroot = string.split(new_dict['path'], '/')[1:]
       new_dba = split_fsroot[-5]
       new_groupname = split_fsroot[-4]
       new_domain = split_fsroot[-3]

       gid = self.DB.Query("""SELECT gid FROM mail_users WHERE dba='%s'
                              AND username='%s'""" % (new_dba, new_groupname))[0][0]

       self.DIR.Set(gid=gid, dba=old_dba, groupname=old_groupname, domain=old_domain,
                    username=old_dict['username'])

       if old_groupname != new_groupname:
           self.DIR.RenameGroup(new_groupname)

       if old_domain != new_domain:
           self.DIR.RenameDomain(new_domain)

       if old_dict['username'] != new_dict['username']:
           self.DIR.RenameUser(new_dict['username'])

       if old_dict['quota_bytes'] != new_dict['quota_bytes']:
           self.Quota.SetUser(new_dict['uid'], new_dict['quota_bytes'])
           self.Quota.Apply()

   def DELETE(self, row_id, old_dict):

       split_fsroot = string.split(new_dict['path'], '/')[1:]
       dba = split_fsroot[-5]
       groupname = split_fsroot[-4]
       domain = split_fsroot[-3]

       gid = self.DB.Query("""SELECT gid FROM mail_users WHERE dba='%s'
                              AND username='%s'""" % (dba, groupname))[0][0]

       self.DIR.Set(gid=gid, dba=dba, groupname=groupname, domain=domain,
                    username=old_dict['username'])
       self.DIR.RemoveUser()

       # XXX: Do further checks, we may be able to prune further
