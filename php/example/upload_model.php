<?
/*
 * TWS SDK for PHP Example
 * upload model
 * 2014. 8. 20
 * Inseok Lee (inseok.lee@3dsystems.com)
 * 3D Systems
 */

require_once "../tws.php";

$api_key = 'CHANGE_THIS_API_KEY_TO_YOURS';
$api_secret = 'CHANGE_THIS_API_SECRET_TO_YOURS';

$tws = new Tws(array('api_key' => $api_key, 'api_secret'=> $api_secret));
$tws->authenticate();

// Upload model from file
$file_path = './file.zip'; // Actually this file not exist in example
var_dump($tws->upload_model($file_path, array('method'=>'file', 'meta' => array('filename'=>'file.zip'))));

// Upload model from memory
$myfile = fopen('./file.zip','rb');
$read_file = fread($myfile, filesize('./file.zip'));
fclose($myfile);
var_dump($tws->upload_model($read_file, array('method'=>'data', 'meta' => array('filename'=>'file.zip'))));
