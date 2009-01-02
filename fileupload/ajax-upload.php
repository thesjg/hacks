#!/usr/local/bin/php
<?php

require('minihttpd.php');
require('upload_handler.php');

function my_exit() {
    exit();
}

$ss = new SocketServer();
$ss->SetHandler('MiniHTTPD', 'upload_handler');
$ss->Setup();

$pid = pcntl_fork();
if ($pid) {
    pcntl_wait($status);
    print $ss->Port . "\n";
    exit();
} else {
    $pid = pcntl_fork();
    if ($pid) {
        exit();
    } else {
        $ss->Listen();
        try {
            $ss->Run(60, 'my_exit'); // exit() isn't a real function
        } catch (Exception $e) {
            // I dunno what we done do what where how when stuff what? huh?
        }
    }
}

?>
