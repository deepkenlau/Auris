#!/usr/bin/php
<?php

$context = null;
$opts = array(
	"-h" => "localhost:8080"
);
$filename = null;
foreach ($argv as $id => $arg) {
	if ($id == 0) continue;
	if ($arg[0] == '-')	$context = $arg;
	else if ($context) {
		$opts[$context] = $arg;
		$context = null;
	}
	else {
		$filename = $arg;
	}
}

if (!$filename) die("usage: add-file.php [-h hostname] FILE\n");

//Read the file.
if (!file_exists($filename)) die ("file does not exist\n");
$contents = file_get_contents($filename);
echo strlen($contents)." bytes\n";

//Upload the file.
$contextData = array ('method' => 'POST',
	'header' =>
		"Connection: close\r\n".
		"Content-Length: ".strlen($contents)."\r\n",
	'content'=> $contents
);
$context = stream_context_create(array('http' => $contextData));
$result = file_get_contents("http://".$opts["-h"]."/add", false, $context);
echo "uploaded file. result: $result\n";