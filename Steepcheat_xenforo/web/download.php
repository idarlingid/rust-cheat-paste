<?php   
$attachment_location = $_SERVER["DOCUMENT_ROOT"] . "/loader/Release.exe";
if (file_exists($attachment_location)) {

    header($_SERVER["SERVER_PROTOCOL"] . " 200 OK");
    header("Cache-Control: public"); // needed for internet explorer
    header("Content-Type: application/exe");
    header("Content-Transfer-Encoding: Binary");
    header("Content-Length:".filesize($attachment_location));
    header("Content-Disposition: attachment; filename=SteepCheat.exe");
    readfile($attachment_location);
    die();        
} 
else 
{
    die("Loader not found.");
}
?>