#!/usr/local/bin/python

from backend import sql
import string

QueryBuilder = sql.SQLQueryBuilder()

class server_db_logins(sql.SQLNotifyTable):
    """ XXX: Implement this later """

    def __name__(self):
        return 'server_db_logins'

    def GetCols(self):
        return ['server_id', 'username', 'password', 'database']


class user_server_quota(sql.SQLNotifyTable):
    """ XXX: Implement me """

    def __name__(self):
        return 'user_server_quota'

    def GetCols(self):
        return ['user_id', 'service_type_id', 'quota_bytes']


class server_config(sql.SQLNotifyTable):

    def __name__(self):
        return 'server_config'

    def GetCols(self):
        return ['server_id', 'fsroot', 'min_uid', 'max_uid', 'min_gid', 'max_gid']

    def INSERT(self, row_id, new_dict):

          # Drop new fsroot into mail_config on target server
        query = "INSERT INTO mail_config (fsroot) VALUES ('%s')" % (new_dict['fsroot'])
        self.Query(query, new_dict['server_id'])

    def UPDATE(self, row_id, old_dict, new_dict):

          # Check if there is anything to do
        if old_dict['fsroot'] == new_dict['fsroot']:
            return

          # Update fsroot entry in mail_config on target server
        query = "UPDATE mail_config SET fsroot='%s'" % (new_dict['fsroot'])
        self.Query(query, new_dict['server_id'])

          # XXX: We will have to change all of the user's fsroot entries too


class users(sql.SQLNotifyTable):

    ServerIDQuery = """SELECT user_servers.server_id FROM user_servers, servers,
                       server_types WHERE user_servers.server_id=servers.id AND
                       servers.server_type_id=server_types.id AND
                       server_types.type='mail' AND
                       user_servers.user_id='%s'"""

    def __name__(self):
        return 'users'

    def GetCols(self):
        return ['dba_id', 'name', 'username', 'password']

    def INSERT(self, row_id, new_dict):

         # Figure out which mail server to attach user too
       query = """SELECT servers.id, COUNT(*) AS count FROM servers, user_servers,
                  server_types WHERE servers.server_type_id=server_types.id AND
                  user_servers.server_id=servers.id AND server_types.type='mail'
                  GROUP BY servers.id ORDER BY count LIMIT 1"""
       server_id = self.Query(self.ServerIDQuery)[0][0]

         # Get max gid for server
       max_gid = self.Query("""SELECT max_gid FROM server_config WHERE
                               server_id='%d'""" % (server_id))[0][0]

         # Get the current max gid on that server
       current_max_gid = self.Query("""SELECT max(server_gid) FROM mail_gid_trans
                                       WHERE server_id='%d'""" % (server_id))[0][0]

       new_gid = 0

         # No users, get min. gid
       if current_max_gid == '':
           new_gid = self.Query("""SELECT min_gid FROM server_config WHERE
                                   server_id='%d'""" % (server_id))[0][0]

         # Cannot put that user on selected server, its full
       elif int(current_max_gid) >= int(max_gid):
             # XXX
           return

         # Calculate new gid
       else:
           new_gid = int(current_max_gid) + 1


         # Update gid translation table
       dict = {}
       dict['gid'] = row_id
       dict['server_gid'] = new_gid
       dict['server_id'] = server_id
       query = QueryBuilder.INSERT('mail_gid_trans', dict)
       self.Query(query)


        # Snag user quota
       query = """SELECT user_server_quota.quota_bytes FROM
                  user_server_quota, service_types WHERE
                  user_server_quota.service_type_id=service_types.id
                  AND service_types.name='mail' AND
                  user_server_quota.user_id='%d'""" % (row_id)
       quota = self.Query(query)[0][0]

        # Snag DBA
       dba = self.Query("SELECT name FROM dba WHERE id='%d'" % new_dict['dba_id'])[0][0]


         # Put new user where they go
       query = """INSERT INTO mail_users (gid, dba, username, quota_bytes) VALUES
                  ('%s', '%s', '%s', '%s')""" % (new_gid, dba, new_dict['username'],
                                                 quota)
       self.Query(query, server_id)


    def UPDATE(self, row_id, old_dict, new_dict):

          # Check if there is anything to do
        if old_dict['name'] == new_dict['name'] and old_dict['username'] == new_dict['username'] and old_dict['password'] == new_dict['password']:
            return

          # Update name, username and password in mail_user on target server
        new_dict.pop('dba_id')
        where = {}
        where['id'] = row_id
        query = QueryBuilder.UPDATE('mail_user', where, old_dict, new_dict);

        server_id = self.Query(self.ServerIDQuery % (row_id))[0][0]
        self.Query(query, server_id)

    def DELETE(self, row_id, old_dict):

        server_id = self.Query(self.ServerIDQuery % (row_id))[0][0]

          # Delete group id translation from primary db
//        query = "DELETE FROM mail_gid_trans WHERE user_id='%d'" % (row_id)
//        self.Query(query, server_id)
// XXX

          # Delete mail_user entry on target server
        query = "DELETE FROM mail_user WHERE gid='%d'" % (row_id)
        self.Query(query, server_id)


class dns_zones(sql.SQLNotifyTable):

    def __name__(self):
        return 'dns_zones'

    def GetCols(self):
        return ['user_id', 'origin', 'ns', 'mbox', 'serial', 'refresh', 'retry',
                'expire', 'minimum', 'ttl']

    def UPDATE(self, row_id, old_dict, new_dict):

          # Check if there is anything to do
        if old_dict['origin'] == new_dict['origin']:
            return

          # Figure out what server we should be operating on
        query = """SELECT user_servers.server_id FROM user_servers, dns_zones WHERE
                   user_servers.user_id=dns_zones.user_id AND dns_zones.id='%s'""" % (row_id)
        server_id = self.Query(query)[0][0]

          # See if we need to go further
        query = """SELECT mail_uid_trans.server_uid FROM mail_uid_trans, mail_users,
                   dns_zones, dns_resources WHERE mail_uid_trans.uid=mail_users.id
                   AND mail_users.dns_resource_id=dns_resources.id AND
                   dns_resources.dns_zone_id=dns_zones.id AND
                   dns_zones.id='%d'""" % (row_id)
        uids = self.Query(query)

        for i in uids:

            uid = i[0]

            query = """SELECT username FROM mail_accounts WHERE uid='%d' AND
                       username LIKE '%%%s'""" % (uid, old_dict['origin'])
            print query
            result = self.Query(query, server_id)

            for j in result:
                print j

              # If true there are accounts on the target server matching the
              # updated zone that we need to fix up ...
            if len(result) > 0:
                for j in result:
                    new_username = string.replace(j[0], old_dict['origin'],
                                                  new_dict['origin'])

                    query = """UPDATE mail_accounts SET username='%s' WHERE uid='%d'
                               AND username='%s'""" % (new_username, uid, j[0])

                    self.Query(query, server_id)



class dns_resources(sql.SQLNotifyTable):

    def __name__(self):
        return 'dns_resources'

    def GetCols(self):
        return ['dns_zone_id', 'name', 'type', 'data', 'aux', 'ttl']

    def UPDATE(self, row_id, old_dict, new_dict):

          # Check if there is anything to do
        if old_dict['name'] == new_dict['name']:
            return

          # Figure out what server we should be operating on
        query = """SELECT user_servers.server_id FROM user_servers, dns_zones,
                   dns_resources WHERE user_servers.user_id=dns_zones.user_id AND
                   dns_zones.id=dns_resources.dns_zone_id AND
                   dns_resources.id='%d'""" % (row_id)
        server_id = self.Query(query)[0][0]

          # Put together domain
        query = "SELECT origin FROM dns_zones WHERE id='%d'" % (new_dict['dns_zone_id'])
        origin = self.Query(query)[0][0]

        domain = ''
        if new_dict['name'] == '':
            domain = origin
        else:
            domain = new_dict['name'] + '.' + origin

        old_domain = ''
        if old_dict['name'] == '':
            old_domain = origin
        else:
            old_domain = old_dict['name'] + '.' + origin

          # See if we need to go further
        query = """SELECT mail_uid_trans.server_uid FROM mail_uid_trans, mail_users,
                   dns_resources WHERE mail_uid_trans.uid=mail_users.id AND
                   mail_users.dns_resource_id=dns_resources.id AND
                   dns_resources.id='%d'""" % (row_id)
        uids = self.Query(query)

        for i in uids:
            uid = i[0]

            query = """SELECT username FROM mail_accounts WHERE uid='%d' AND
                       username LIKE '%%%s'""" % (uid, domain)
            result = self.Query(query, server_id)


              # If true there are accounts on the target server matching the
              # updated zone resource that we need to fix up ...
            if len(result) > 0:
                for j in result:
                    new_username = string.replace(j[0], old_domain, domain)

                    query = """UPDATE mail_accounts SET username='%s' WHERE uid='%d'
                               AND username='%s'""" % (new_username, uid, j[0])
                    self.Query(query, server_id)


class mail_users(sql.SQLNotifyTable):

    # XXX: Add support for dns_resource_id changes

    DomainQuery = """SELECT concat_hosts_func(dns_resources.name, dns_zones.origin)
                   FROM dns_resources, dns_zones WHERE
                   dns_resources.dns_zone_id=dns_zones.id AND
                   dns_resources.id='%s'"""

    ServerIDQuery = """SELECT user_servers.server_id FROM user_servers, servers,
                       server_types WHERE user_servers.server_id=servers.id AND
                       servers.server_type_id=server_types.id AND
                       server_types.type='mail' AND
                       user_servers.user_id='%s'"""

    def __name__(self):
       return 'mail_users'

    def GetCols(self):
       return ['user_id', 'dns_resource_id', 'name', 'username', 'password',
               'quota_bytes', 'spam_thrsh']

    def INSERT(self, row_id, new_dict):

         # Get target server id
       server_id = self.Query(self.ServerIDQuery % (new_dict['user_id']))[0][0]

         # Get max gid for server
       max_uid = self.Query("""SELECT max_uid FROM server_config WHERE
                               server_id='%d'""" % (server_id))[0][0]

         # Get the current max gid on that server
       current_max_uid = self.Query("""SELECT max(server_uid) FROM mail_uid_trans
                                       WHERE server_id='%d'""" % (server_id))[0][0]

       new_uid = 0

         # No users, get min. uid
       if current_max_uid == None:
           new_uid = self.Query("""SELECT min_uid FROM server_config WHERE
                                   server_id='%d'""" % (server_id))[0][0]

         # Cannot put that user on selected server, its full
       elif int(current_max_uid) >= int(max_uid):
             # XXX
           return

         # Calculate new uid
       else:
           new_uid = int(current_max_uid) + 1


         # Update uid translation table
       dict = {}
       dict['uid'] = row_id
       dict['server_uid'] = new_uid
       dict['server_id'] = server_id
       query = QueryBuilder.INSERT('mail_uid_trans', dict)
       self.Query(query)


         # Get domain
       query = self.DomainQuery % (new_dict['dns_resource_id'])
       domain = self.Query(query)[0][0]

         # Build new username
       username = new_dict['username'] + '@' + domain

         # Build path
       fsroot = self.Query("SELECT fsroot FROM mail_config", server_id)[0][0]

       query = """SELECT dba.name FROM dba, users WHERE users.dba_id=dba.id AND
                  users.id='%d'""" % (new_dict['user_id'])
       dba = self.Query(query)[0][0]

       query = "SELECT username FROM users WHERE users.id='%d'" % (new_dict['user_id'])
       groupname = self.Query(query)[0][0]

       path = fsroot + '/' + dba + '/' + groupname + '/' + domain + '/' + new_dict['username'] + '/Maildir'

         # Setup variables for insert query
       new = {}
       new['uid'] = new_uid
       new['username'] = username
       new['password'] = new_dict['password']
       new['name'] = new_dict['name']
       new['path'] = path
       new['spam_thrsh'] = new_dict['spam_thrsh']
       new['quota_bytes'] = new_dict['quota_bytes']

         # Do it
       query = QueryBuilder.INSERT('mail_accounts', new)
       self.Query(query, server_id)


    def UPDATE(self, row_id, old_dict, new_dict):

         # Check if there is anything to do
       if old_dict['name'] == new_dict['name'] and old_dict['username'] == new_dict['username'] and old_dict['password'] == new_dict['password'] and old_dict['quota_bytes'] == new_dict['quota_bytes'] and old_dict['spam_thrsh'] == new_dict['spam_thrsh']:
           return

         # Put together query data
       old = {}
       new = {}

       old['name'] = old_dict['name']
       new['name'] = new_dict['name']

       old['password'] = old_dict['password']
       new['password'] = new_dict['password']

       old['quota_bytes'] = old_dict['quota_bytes']
       new['quota_bytes'] = new_dict['quota_bytes']

       old['spam_thrsh'] = old_dict['spam_thrsh']
       new['spam_thrsh'] = new_dict['spam_thrsh']


       if old_dict['username'] != new_dict['username']:
           old['username'] = ''

             # Get domain
           query = self.DomainQuery % (new_dict['dns_resource_id'])
           domain = self.Query(query)[0][0]

             # Build new username
           new['username'] = new_dict['username'] + '@' + domain


         # Get target server id
       server_id = self.Query(self.ServerIDQuery % (new_dict['user_id']))[0][0]

         # Get uid that we should be on target server
       query = """SELECT server_uid FROM mail_uid_trans WHERE uid='%d' AND
                  server_id='%d'""" % (new_dict['user_id'], server_id)
       uid = self.Query(query)[0][0]

         # XXX: We need to update the fsroot too!

       where = {}
       where['uid'] = uid
       query = QueryBuilder.UPDATE('mail_user', where, old, new);

         # Do it
       self.Query(query, server_id)


    def DELETE(self, row_id, old_dict):

         # Get target server id
       server_id = self.Query(self.ServerIDQuery % (old_dict['user_id']))[0][0]

         # Get uid that we should be on target server
       query = """SELECT server_uid FROM mail_uid_trans WHERE uid='%s' AND
                  server_id='%s'""" % (old_dict['user_id'], server_id)
       uid = self.Query(query)[0][0]

         # Do it
       query = "DELETE FROM mail_accounts WHERE uid='%d'" % (uid)
       self.Query(query, server_id)



class mail_aliases(sql.SQLNotifyTable):

    # XXX: Add support for dns_resource_id changes

    DomainQuery = """SELECT concat_hosts_func(dns_resources.name, dns_zones.origin)
                   FROM dns_resources, dns_zones WHERE
                   dns_resources.dns_zone_id=dns_zones.id AND
                   dns_resources.id='%d'"""

    ServerIDQuery = """SELECT user_servers.server_id FROM user_servers, servers,
                       server_types WHERE user_servers.server_id=servers.id AND
                       servers.server_type_id=server_types.id AND
                       server_types.type='mail' AND
                       user_servers.user_id='%d'"""

    def __name__(self):
       return 'mail_aliases'

    def GetCols(self):
       return ['user_id', 'dns_resource_id', 'username', 'destination']

    def INSERT(self, row_id, new_dict):

         # Get domain
       query = self.DomainQuery % (new_dict['dns_resouce_id'])
       domain = self.Query(query)[0][0]

         # Build address
       address = new_dict['username'] + '@' + domain

         # Get server id
       server_id = self.Query(self.ServerIDQuery % (new_dict['user_id']))[0][0]

         # Do INSERT
       query = """INSERT INTO mail_aliases (address, destination) VALUES
                  ('%s', '%s')""" % (address, new_dict['destination'])
       self.Query(query, server_id)


    def UPDATE(self, row_id, old_dict, new_dict):

         # Check if there is anything to do
       if old_dict['username'] == new_dict['username'] and old_dict['destination'] == new_dict['destination']:
           return

         # Get Domain
       query = self.DomainQuery % (new_dict['dns_resouce_id'])
       domain = self.Query(query)[0][0]

         # Build addresses
       old_address = old_dict['username'] + '@' + domain
       new_address = new_dict['username'] + '@' + domain

         # Setup query and do it
       old = {}
       new = {}
       where = {}

       old['address'] = old_address
       new['address'] = new_address

       old['destination'] = old_dict['destination']
       new['destination'] = new_dict['destination']

       where['address'] = old_address
       where['destination'] = old_dict['destination']

       query = QueryBuilder.UPDATE('mail_aliases', where, old, new);
       self.Query(query, server_id)


    def DELETE(self, row_id, old_dict):

         # Get domain
       query = self.DomainQuery % (old_dict['dns_resouce_id'])
       domain = self.Query(query)[0][0]

         # Build address
       address = old_dict['username'] + '@' + domain

         # Nab server_id
       server_id = self.Query(self.ServerIDQuery % (new_dict['user_id']))[0][0]

         # Do DELETE
       query = """DELETE FROM mail_aliases WHERE address='%s' AND
                  destination='%s'""" % (address, old_dict['destination'])
       self.Query(query, server_id)
