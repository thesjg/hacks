class SQLNotifyTable:

    DB = None
    ServerDB = None

    NameError = '__name__ method must exist in child'

    def __init__(self, db, serverdb=None):
        self.DB = db
        self.ServerDB = serverdb

    def __name__(self):
        raise self.NameError

    def GetCols(self):
        return []

    def Query(self, query, server_id=None):
        if (server_id != None)
            return self.ServerDB.Query(server_id, query)
        else
            return self.DB.Query(query)

    def INSERT(self, row_id, new_dict):
        pass

    def UPDATE(self, row_id, old_dict, new_dict):
        pass

    def DELETE(self, row_id, old_dict):
        pass
