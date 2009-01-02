<?php

$pid = pcntl_fork();
if ($pid) {
    pcntl_wait($status);
} else {
    $pid = pcntl_fork();
    if ($pid) {
        exit();
    } else {
        $counter = 1;
        while (1) {
            sleep(5);
            syslog(LOG_CRIT, "hello $counter");
            $counter++;
        }
    }
}

?>
