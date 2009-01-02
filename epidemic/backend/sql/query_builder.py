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
