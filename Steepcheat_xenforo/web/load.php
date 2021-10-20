<?php
include "../AES.php";
$dllpath = "/loader/DarlingGey.dll"; //JTPzX2X6hwsBSH6.dll
//$dllpath = "/loader/ps.sys";

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

    $ip_request = 'localhost';
    $unprotect_request = decrypt(fix_request($_GET["a"]), "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q");
    $protect_request = fix_request($_GET["b"]);
    
    $tempory_cipher_key = substr($unprotect_request, 0, 32);
    $tempory_iv_key = substr($unprotect_request, 32, 48);

    if ($unprotect_request == decrypt($protect_request, $tempory_cipher_key, $tempory_iv_key))
    {
        $date_register_key = date("Y-m-d H:i:s");
        
        $generated_session_cipher_key = generate_key(64); 
        $session_cipher_key = $data->cipher_key = $generated_session_cipher_key;
        
        $generated_session_iv_key = generate_key(32);
        $session_iv_key = $data->iv_key = $generated_session_iv_key;         
        
        //mysqli_query($helper::connect(),"INSERT INTO aes_keys_injection(cipher, iv, date, ip) VALUES('$generated_session_cipher_key', '$generated_session_iv_key', '$date_register_key', '$ip_request')");

        $attachment_location = $_SERVER["DOCUMENT_ROOT"] . $dllpath;

        if (file_exists($attachment_location)) 
        {
            //header($_SERVER["SERVER_PROTOCOL"] . " 200 OK");
            //header("Cache-Control: public"); // needed for internet explorer
            //header("Content-Type: application/dll");
            //header("Content-Transfer-Encoding: Binary");
            //header("Content-Length:".filesize($attachment_location));
            //header("Content-Disposition: attachment; filename=Full.dll");
            $raw = file_get_contents($attachment_location);
            //echo $raw;
            echo RSACrypt($raw);
        }
        
        unset($tempory_cipher_key);
        unset($tempory_iv_key); 
    }
    else
    {
       die(encrypt("file_error", "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q"));
    }
}
else
{
    die();
}
?>