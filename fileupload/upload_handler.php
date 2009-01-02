<?php

function upload_handler($post, $files, $x=NULL) {
    ob_start();
    print_r($post);
    print_r($files);
    $result = ob_get_contents();
    ob_end_clean();

    if ($x === NULL)
        print $result;
    else
        $x->Write($result);
}

?>
