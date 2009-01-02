from notify import *

class SQLNotifyBootstrap(SQLNotify):

    SchemaName    = 'epidemic'
    SchemaCheck   = "SELECT n.name FROM pg_catalog.pg_namespace n WHERE n.name='" + SchemaName + "'"
    SchemaCreate  = 'CREATE SCHEMA ' + SchemaName
    SchemaDestroy = 'DROP SCHEMA ' + SchemaName

    DomainName    = SchemaName + '.notify_state'
    DomainCheck   = """SELECT t.typname FROM pg_catalog.pg_type t WHERE t.typtype='d'
                       AND t.typname='""" + DomainName + "'"
    DomainCreate  = 'CREATE DOMAIN ' + DomainName + ' AS VARCHAR(8) CHECK' + \
                    "(VALUE IN ('INSERT', 'UPDATE', 'DELETE'))"
    DomainDestroy = ''

    TableName    = SchemaName + '.notify_%s'
    TableCheck   = """SELECT relname FROM pg_class WHERE relname !~ '^pg_' AND
                      relkind='r' AND relname='""" + TableName + "'"
    TableCreate  = 'CREATE TABLE ' + TableName + ' (' + \
                   """  id         SERIAL8 PRIMARY KEY,
                        row_id     INT8 NOT NULL,
                        table_name VARCHAR(255) NOT NULL,
                        state      EPIDEMIC.NOTIFY_STATE NOT NULL,
                        old_row    TEXT DEFAULT NULL)"""
    TableDestroy = 'DROP TABLE ' + TableName

    FunctionName    = SchemaName + '.notify_%s_%s_func()'
    FunctionCheck   = ''
    FunctionData    = "old_data := old_data || '%s=('' || OLD.%s || '')%s';\n"
    FunctionCreate  = 'CREATE OR REPLACE FUNCTION ' + FunctionName + 'RETURNS TRIGGER AS $$' + \
                      """DECLARE
                           old_data TEXT := '';
                           id INTEGER;
                         BEGIN
                           IF TG_OP = 'UPDATE' OR TG_OP = 'DELETE' THEN
                             %s
                           END IF;

                           IF TG_OP = 'INSERT' OR TG_OP = 'UPDATE' THEN
                             id := NEW.id;
                           END IF;

                           IF TG_OP = 'DELETE' THEN
                             id := OLD.id;
                           END IF;

                           INSERT INTO """ + TableName + \
                           """(row_id, table_name, state, old_row) VALUES (id, TG_RELNAME, TG_OP, old_data);

                         RETURN NULL;
                         END;
                       $$ LANGUAGE plpgsql"""
    FunctionDestroy = 'DROP FUNCTION %s()'

    TriggerName    = SchemaName + '.notify_%s_%s_trigger'
    TriggerCheck   = ''
    TriggerCreate  = 'CREATE TRIGGER ' + TriggerName + \
                     'AFTER INSERT OR UPDATE OR DELETE ON %s FOR EACH ROW EXECUTE PROCEDURE' + \
                     FunctionName
    TriggerDestroy = 'DROP TRIGGER %s ON %s'

    RuleName    = SchemaName + '.notify_%s_rule'
    RuleCheck   = ''
    RuleCreate  = 'CREATE OR REPLACE RULE ' + RuleName + \
                  ' AS ON INSERT TO ' + TableName + ' DO (NOTIFY ' + \
                  'notify_%s)'
    RuleDestroy = ''


    def __init__(self, name, database, username, password='',
                 hostname='localhost', port='5432'):
        SQLNotify.__init__(self, name, database, username, password,
                           hostname, port)

    def Bootstrap(self):
        self.Query('BEGIN')

        try:
            # Check for schema
            result = self.Query(self.SchemaCheck)
            if len(result) == 0:
                self.Query(self.SchemaCreate)

            # Check for domain
            result = self.Query(self.DomainCheck)
            if len(result) == 0:
                self.Query(self.DomainCreate)

            # Check for log table
            result = self.Query(self.TableCheck % (self.Name))
            if (len(result)) == 0:
                self.Query(self.TableCreate % (self.Name))

            for dict in self.Data:
                # Build function
                function_data = ''
                i = 1
                for var in dict['cols']:
                    comma = ','
                    if i == len(vars):
                        comma = ''
                    function_data = function_data + self.FunctionData % (var, var, comma)
                    i = i + 1

                query = self.FunctionCreate % (self.Name, dict['table_name'], function_data, self.Name)
                self.Query(query)

                # Check log function for each registration
                #result = self.Query()
                #if (len(result)) == 0:
                #    self.Query()

                # Check log function trigger for each registration
                #result = self.Query()
                #if (len(result)) == 0:
                query = self.TriggerCreate % (self.Name, dict['table_name'], dict['table_name'],
                                              self.Name, dict['table_name'])
                self.Query(query)

                # Check notification rule for each registration
                #result = self.Query()
                #if (len(result)) == 0:
                query = self.RuleCreate % (self.Name, self.Name, self.Name)
                self.Query()

            self.Query('COMMIT')
        except:
            self.Query('ROLLBACK')
