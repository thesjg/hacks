from notify import *
from select import *

class SQLNotifyDispatch(SQLNotify):

    Poll = None
    Notifies = None
    NotifyCursor = None

    Regex = re.compile(r'(?P<key>[a-z_]+)=\(\'(?P<value>[\w_/\ \.]*),*\'\)')

    def __init__(self, name, database, username, password='',
                 hostname='localhost', port='5432'):
        SQLNotify.__init__(self, name, database, username, password,
                           hostname, port)

        self.Poll = poll()
        self.NotifyCursor = self.Connection.cursor()
        self.Poll.register(self.NotifyCursor.fileno(), POLLIN)

    def Dispatch(self):
        while 1:
            self.Listen()
            notifies = self.Get()
            for i in notifies:

                for j in self.Data:
                    if j['table_name'] == i['table_name']:

                          # Pull new row
                        new_dict = {}
                        if i['state'] != 'DELETE':
                            cols = string.join(j['cols'], ', ')

                              # XXX: Abuse
                            self.Commit()
                            result = self.Query("SELECT %s FROM %s WHERE id='%s'" %
                                                (cols, i['table_name'], i['row_id']))

                              # Temporary dictionary for new row
                            new_dict_vals = result[0]
                            l = 0
                            for k in j['cols']:
                                new_dict[k] = new_dict_vals[l]
                                l = l + 1

                          # Temporary dictionary for old row
                        old_dict = {}
                          # Extract cols that class is interested in
                        if i['state'] == 'UPDATE' or i['state'] == 'DELETE':
                            for k in j['cols']:
                                old_dict[k] = i['old_row'][k]

                          # Call class member for table operation
                        if i['state'] == 'INSERT':
                            j['class'].INSERT(i['row_id'], new_dict)

                        elif i['state'] == 'UPDATE':
                            j['class'].UPDATE(i['row_id'], old_dict, new_dict)

                        elif i['state'] == 'DELETE':
                            j['class'].DELETE(i['row_id'], old_dict)

    def Listen(self, time=-1):
        self.Query('LISTEN notify_%s' % (self.Name))
        self.Poll.poll(time)

    def Get(self):
        """ """

        notifies = self.NotifyCursor.notifies()

        if len(notifies) == 0:
            self.Notifies = None
            return None

        self.Notifies = notifies

        s_result = self.Query('SELECT * FROM notify_%s' % (self.Name))

        ret = []
        for i in s_result:
            row = {}

            self.Query("DELETE FROM notify_%s WHERE id='%d'" %
                       (self.Name, i[0]))

            row['row_id'] = i[1]
            row['table_name'] = i[2]
            row['state'] = i[3]

            old_data = {}

            count = 0
            while 1:

                if count >= len(i[4]):
                    break

                match = self.Regex.search(i[4][count:])
                old_data[match.group('key')] = match.group('value')
                count = count + match.end()

            row['old_row'] = old_data
            ret.append(row)

        return ret
