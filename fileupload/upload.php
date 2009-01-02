<?php

require('upload_handler.php');

header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");

switch ($_SERVER['REQUEST_METHOD']) {
    case 'GET':
        $handle = popen('./ajax-upload.php', 'r');
        $buffer = fgets($handle);
        print $buffer;
        pclose($handle);
        exit();
        break;
    case 'POST':
        upload_handler($_POST, $_FILES);
        break;
    default:
        exit('ERR');
}

?>
