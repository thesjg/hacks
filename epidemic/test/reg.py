#!/usr/local/bin/python

import re


#Regex = re.compile(r'([a-z_]+)=\(\'([\w_/\ ]+),*\'\)')
Regex = re.compile(r'([a-z_]+)=\(\'([\w_/\ ]*),*\'\)')
teststring = "foobar=(''),server_id=('1'),path=('/home/mail/mail0'),min_uid=('5000'),max_uid=('10000'),min_gid=('5000'),max_gid=('5500')"

count = 0
while 1:

    if count >= len(teststring):
        break

    match = Regex.search(teststring[count:])
    #print match.group('key') + '=' + match.group('value')
    print match.group(0)
    count = count + match.end()
