#!/usr/local/bin/python

import getpass, sys
from mail import server
from backend import sql

dispatch = sql.SQLNotifyDispatch('mail', 'mail0', 'sjg')
db = sql.SQL('mail0', 'sjg')

if getpass.getuser() != 'root':
    sys.exit('This process must be run as user root')


mu = server.mail_users(db)
ma = server.mail_accounts(db)

dispatch.Register('mail_users', mu)
dispatch.Register('mail_accounts', ma)

dispatch.Dispatch()
