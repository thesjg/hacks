#!/usr/local/bin/python

import freebsd

class Quota:

    User = None
    Group = None

    FileSystem = None
    BlockSize = None

    UserQuotaBytes = None
    UserQuotaBlocks = None

    GroupQuotaBytes = None
    GroupQuotaBlocks = None

    UserGroupException = 'Either a user or group must be specified'
    BlockSizeException = 'Cannot get filesystem blocksize'


    def __init__(self, path):
        self.FileSystem = path
        self.GetBlockSize()

    def Apply(self):
        if self.User == None and self.Group == None:
            raise self.UserGroupException

        if self.User != None:
            freebsd.setquota(self.FileSystem, freebsd.USRQUOTA, self.User,
                             bhardlimit=self.UserQuotaBlocks)
            freebsd.setquota(self.FileSystem, freebsd.USRQUOTA, self.User,
                             bsoftlimit=self.UserQuotaBlocks)

        if self.Group != None:
            freebsd.setquota(self.FileSystem, freebsd.GRPQUOTA, self.Group,
                             bhardlimit=self.GroupQuotaBlocks)
            freebsd.setquota(self.FileSystem, freebsd.GRPQUOTA, self.Group,
                             bsoftlimit=self.GroupQuotaBlocks)


    def SetUser(self, user, quota):
        self.User = user
        self.UserQuotaBytes = quota
        self.UserQuotaBlocks = self.ConvertToBlocks(quota)

    def SetGroup(self, group, quota):
        self.Group = group
        self.GroupQuotaBytes = quota
        self.GroupQuotaBlocks = self.ConvertToBlocks(quota)

    def GetBlockSize(self):
        fsstat = freebsd.getfsstat(0)

        for i in fsstat:
            if i['mntonname'] == self.FileSystem:
                self.BlockSize = i['bsize']
                return

        raise BlockSizeException

    def ConvertToBlocks(self, bytes):
        if bytes == None:
            return None

        ret = bytes / self.BlockSize
        remainder = bytes % self.BlockSize

        if remainder != 0:
            ret = ret + 1

        return ret

    def ConvertToBytes(self, blocks):
        return (blocks * self.BlockSize)

if __name__ == '__main__':
    q = Quota('/usr')
    print q.ConvertToBlocks(None)
    #q.SetGroup(108, 5000000)
    #q.Apply()
