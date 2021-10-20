<?php
 function generate_random_string($length) 
{
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    
    for ($i = 0; $i < $length; $i++) 
    {
        $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
        
    return $randomString;
}   
$random = generate_random_string(48);	
$encrypted = base64_encode(openssl_encrypt($random, 'aes-256-cfb', "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", true, "H1ggF9foFGLerr8q")); 
echo($encrypted);
?>