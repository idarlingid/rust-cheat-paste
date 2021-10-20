<?php
include "../AES.php";
$unprotect_request = decrypt(fix_request($_GET["a"]), "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q");

$tempory_cipher_key = substr($unprotect_request, 0, 32);
$tempory_iv_key = substr($unprotect_request, 32, 16);
 
if (isset($_GET["c"])) {
    require('../src/XF.php');
    XF::start( $fileDir );
	$app = \XF::setupApp('XF\App');
    
    $ip = 'localhost';

	if ( isset( $_GET[ "d" ] ) )
	{

        $protect_request = fix_request($_GET["b"]);

	    //decrypt($_GET['k'], "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", $tempory_iv_key)
		$loginService = $app->service('XF:User\Login', decrypt(fix_request($_GET["c"]), $tempory_cipher_key, $tempory_iv_key), $ip);
		$userValidate = $loginService->validate(decrypt(fix_request($_GET['d']), $tempory_cipher_key, $tempory_iv_key), $error);
		if($userValidate)
		{
		    $username = decrypt(fix_request($_GET['c']), $tempory_cipher_key, $tempory_iv_key);
		    $shwid = fix_request($_GET['f']);
		    $hwid = decrypt($shwid, $tempory_cipher_key, $tempory_iv_key);
		    $sql = mysqli_connect("localhost", "sharawin_iforum", "12345678Darling1337!", "sharawin_iforum");
		    $query = mysqli_query($sql, "SELECT * FROM hwids WHERE username='$username'");
		    $count = mysqli_num_rows($query);
		    $row = mysqli_fetch_assoc($query);
		    
		    $user = mysqli_query($sql, "SELECT * FROM xf_user WHERE username='$username'");
		    $roles = mysqli_fetch_assoc($user);
		    $role = $roles['secondary_group_ids']; //encrypt($test2['secondary_group_ids'], "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", $tempory_iv_key);
		    $user_ = decrypt(fix_request($_GET['c']), $tempory_cipher_key, $tempory_iv_key);
		    if ($count > 0) {
		        if ($row['hwid'] == $hwid) {
		            $RuExFull;
		            $RuExCompact;
		            $Spoofer;
                    if(strstr($role, "5")){
                        mysqli_query($sql, "UPDATE hwids SET RuExFull='1' WHERE username='$username'");
                        $RuExFull = "RuExFull";
                        //$query = mysqli_query($sql, "INSERT INTO hwids (username, hwid, full, compact) VALUES ('$username', '$hwid', '1', '1')");
                    }
                    else {
                        mysqli_query($sql, "UPDATE hwids SET RuExFull=NULL WHERE username='$username'");
                        $RuExFull = "REFN";
                    }
                    if(strstr($role, "6")){
                        mysqli_query($sql, "UPDATE hwids SET RuExLow='1' WHERE username='$username'");
                        $RuExCompact = "RuExLow";
                    }
                    else {
                        mysqli_query($sql, "UPDATE hwids SET RuExLow=NULL WHERE username='$username'");
                        $RuExCompact = "RELN";
                    }
                    
                    
                    if(strstr($role, "7")){
                        mysqli_query($sql, "UPDATE hwids SET Spoofer='1' WHERE username='$username'");
                        $Spoofer = "Spoofer";
                    }
                    else {
                        mysqli_query($sql, "UPDATE hwids SET Spoofer=NULL WHERE username='$username'");
                        $Spoofer = "SpN";
                    }
                    //echo $role . $user_;
                   
                        
                    if (!$role) {
                        //echo "NotFound";
                    }
                      echo encrypt(encrypt($user_ . ";" . $RuExFull . ";" . $RuExCompact . ";" . $Spoofer, $tempory_cipher_key, $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
		        }
		        else {
		            echo encrypt(encrypt("hwid_fail", $tempory_cipher_key, $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
		        }
		    }
		    else {
		       $query = mysqli_query($sql, "INSERT INTO hwids (username, hwid) VALUES ('$username', '$hwid')");
		       echo encrypt(encrypt("hwid_installed", $tempory_cipher_key, $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
		    }
		}
		else {
            echo encrypt(encrypt("auth_failed",$tempory_cipher_key , $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
		}
	}
	else {
	    echo encrypt(encrypt("password_not_found", $tempory_cipher_key, $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
	}
}
else {
    echo encrypt(encrypt("unknown_error", $tempory_cipher_key, $tempory_iv_key), $tempory_cipher_key, $tempory_iv_key);
}
?>