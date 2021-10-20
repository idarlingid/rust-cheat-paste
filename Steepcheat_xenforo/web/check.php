<?php
include "../AES.php";
$unprotect_request = decrypt(fix_request($_GET["a"]), "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q");
    
$tempory_cipher_key = substr($unprotect_request, 0, 32);
$tempory_iv_key = substr($unprotect_request, 32, 48);
require_once('config.php');
if (fix_request($_GET['c'])) {
    if (decrypt($_GET['c'], $tempory_cipher_key, $tempory_iv_key) == $config['client_key']) 
    {
        if (decrypt(fix_request($_GET['d']), $tempory_cipher_key, $tempory_iv_key) == 1) {
            echo encrypt(encrypt($config['version'], $tempory_cipher_key, $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
        }
    }
    else {
        echo encrypt(encrypt("incorrect_client_key", $tempory_cipher_key, $tempory_iv_key) , $tempory_cipher_key, $tempory_iv_key);
    }
}
?>