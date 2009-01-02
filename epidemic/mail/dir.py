#!/usr/local/bin/python

import os, string

class MailDirStructure:

    # Read = 4
    # Write = 2
    # Execute = 1

    # root:wheel (rwxr-xr-x)  /home
    # root:wheel (rwxr-xr-x)    /mail
    # root:wheel (rwxr-xr-x)      /dba
    # root:group (rwxr-x---)        /groupname
    # root:group (rwxr-x---)          /domain
    # user:group (rwx------)            /username
    # user:group (rwx------)              /Maildir
    # user:group (rwx------)                /cur
    # user:group (rwx------)                /new
    # user:group (rwx------)                /tmp

    fsroot = None
    dba = None
    groupname = None
    domain = None
    username = None
    uid = None
    gid = None

    def __init__(self):
        pass

    def Set(self, fsroot=None, dba=None, groupname=None, domain=None,
            username=None, uid=None, gid=None):
        if fsroot != None:
            self.fsroot = fsroot
        if dba != None:
            self.dba = dba
        if groupname != None:
            self.groupname = groupname
        if domain != None:
            self.domain = domain
        if username != None:
            self.username = username
        if uid != None:
            self.uid = uid
        if gid != None:
            self.gid = gid

    def CheckDir(self, dir):

          # Does it exist?
        try:
            os.stat(dir)
        except OSError:
            return False

        return True

    def CheckVars(self, vars):

        ret = True

        for i in vars:
            if i == None:
                ret = False

        return ret

    def CreateFSRoot(self):

        if self.CheckVars([self.fsroot]) == False:
            return False

        dirs = string.split(self.fsroot, '/')
        if dirs[0] == '':
            dirs = dirs[1:]

        dir = '/'
        for i in dirs:
            dir = os.path.join(dir, i)
            if self.CheckDir(dir) == False:
                os.mkdir(dir, 755)
                os.chown(dir, 0, 0)

    def CreateDBA(self):

        if self.CheckVars([self.fsroot, self.dba]) == False:
            return False

        dir = os.path.join(self.fsroot, self.dba)
        if self.CheckDir(dir) == False:
            os.mkdir(dir, 755)
            os.chown(dir, 0, 0)

    def CreateGroup(self):

        if self.CheckVars([self.fsroot, self.dba, self.groupname,
                           self.gid]) == False:
            return False

        dir = os.path.join(self.fsroot, self.dba, self.groupname)
        if self.CheckDir(dir) == False:
            os.mkdir(dir, 750)
            os.chown(dir, 0, self.gid)

    def CreateDomain(self):

        if self.CheckVars([self.fsroot, self.dba, self.groupname,
                           self.domain, self.gid]) == False:
            return False

        dir = os.path.join(self.fsroot, self.dba, self.groupname, self.domain)
        if self.CheckDir(dir) == False:
            os.mkdir(dir, 750)
            os.chown(dir, 0, self.gid)

    def CreateUser(self):

        if self.CheckVars([self.fsroot, self.dba, self.groupname,
                           self.domain, self.gid, self.uid,
                           self.username]) == False:
            return False

        dir = os.path.join(self.fsroot, self.dba, self.groupname, self.domain,
                           self.username)
        if self.CheckDir(dir) == False:
            os.mkdir(dir, 700)
            os.chown(dir, self.user, self.group)

        dir = dir + '/Maildir'
        if self.CheckDir(dir) == False:
            os.mkdir(dir, 700)
            os.chown(dir, self.user, self.group)

        for i in ['cur', 'new', 'temp']:
            tmp_dir = dir + '/' + i
            if self.CheckDir(dir) == False:
                os.mkdir(dir, 700)
                os.chown(dir, self.user, self.group)

    def CreateAll(self):
        self.CreateFSRoot()
        self.CreateDBA()
        self.CreateGroup()
        self.CreateDomain()
        self.CreateUser()

    def RemoveDir(self, dir):
        for root, dirs, files in os.walk(dir, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
        os.rmdir(dir)

    def RemoveUser(self):
        dir = os.path.join(self.fsroot, self.dba, self.groupname, self.domain,
                           self.username)
        self.RemoveDir(dir)

    def RemoveDomain(self):
        dir = os.path.join(self.fsroot, self.dba, self.groupname, self.domain)
        self.RemoveDir(dir)

    def RemoveGroup(self):
        dir = os.path.join(self.fsroot, self.dba, self.groupname)
        self.RemoveDir(dir)

    def RenameDir(self, olddir, newdir):
        os.rename(olddir, newdir)

    def RenameUser(self, newuser):
        olddir = os.path.join(self.fsroot, self.dba, self.groupname,
                              self.domain, self.username)

        newdir = os.path.join(self.fsroot, self.dba, self.groupname,
                              self.domain, newuser)
        self.RenameDir(olddir, newdir)

    def RenameDomain(self, newdomain):
        olddir = os.path.join(self.fsroot, self.dba, self.groupname,
                              self.domain)
        newdir = os.path.join(self.fsroot, self.dba, self.groupname,
                              newdomain)
        self.RenameDir(olddir, newdir)

    def RenameGroup(self, newgroup):
        olddir = os.path.join(self.fsroot, self.dba, self.groupname)
        newdir = os.path.join(self.fsroot, self.dba, newgroup)
        self.RenameDir(olddir, newdir)
