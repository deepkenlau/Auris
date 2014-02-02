#!/usr/bin/php
<?php
/* Copyright (c) 2014 Fabian Schuiki */
/* Import an entire iTunes library into an Auris database. */

if (count($argv) < 2) {
	echo "usage: ".$argv[0]." XML\n";
	exit(1);
}
$xml_path = $argv[1];
$adb = __DIR__."/../build/source/tools";

// read the library xml
echo "reading library... ";
$xml = simplexml_load_file($xml_path);
if (!$xml) {
	echo "unable to open xml file\n";
	exit(2);
}
echo "done\n";

// read the root keys
$root_children = array();
$last_key = null;
foreach ($xml->dict->children() as $c) {
	if ($c->getName() == "key") {
		$last_key = strtolower($c);
	} else if ($last_key != null) {
		$root_children[$last_key] = $c;
		$last_key = null;
	}
}

function decode_path($path) { return preg_replace('/^file:\/\/(localhost)?/', "", urldecode($path)); }
$music_folder = decode_path($root_children["music folder"]);
$library_folder = dirname($xml_path);

// read the tracks
$tracks = array();
$metadata = array();
foreach ($root_children["tracks"]->children() as $c) {
	if ($c->getName() != "dict") continue;
	$track = array();
	foreach ($c->children() as $d) {
		if ($d->getName() == "key") {
			$last_key = str_replace(" ", "-", strtolower($d));
		} else if ($last_key != null) {
			switch ($d->getName()) {
				case "true":  $value = true; break;
				case "false": $value = false; break;
				case "date":  $value = new DateTime("$d"); break;
				default: $value = "$d"; break;
			}
			$track[$last_key] = $value;
			if (!in_array($last_key, $metadata))
				$metadata[] = $last_key;
			$last_key = null;
		}
	}
	if (@$track["has-video"] || @$track["podcast"]) continue;
	$tracks[] = $track;
}
echo "loaded ".count($tracks)." tracks\n";
// print_r($metadata);
$accepted_metadata = array("name", "artist", "genre", "date-added", "play-count", "play-date", "skip-count", "album", "year", "comments", "rating", "track-number", "track-count", "disc-number", "disc-count", "composer", "release-date");

// add the tracks
$i = 0;
$last_percentage = null;
$skipped = array();
foreach ($tracks as $track) {
	$i++;
	// if ($i >= 10) break;
	$loc = @$track["location"];
	if (!$loc) {
		echo "skipping ".$track["track-id"]." (no location specified)\n";
		continue;
	}
	$loc = decode_path($loc);
	if (!file_exists($loc)) {
		$loc = preg_replace('/^'.preg_quote($music_folder, "/").'\/?/', rtrim($library_folder, "/")."/iTunes Music/", $loc);
		if (!file_exists($loc)) {
			$skipped[] = $loc;
			continue;
		}
	}

	// gather the metadata that will be added
	$md = array();
	foreach ($track as $k => $v) {
		if (!in_array($k, $accepted_metadata)) continue;
		if ($k == "name") $k = "title";
		if ($k == "date-added") $k = "added";
		if ($v instanceof DateTime) $v = $v->getTimestamp();
		$md[$k] = $v;
	}

	// add the track and adjust its metadata
	$output = array();
	$return_var = 0;
	exec("$adb/db-add ".escapeshellarg($loc), $output, $return_var);
	if ($return_var != 0) {
		echo "failed to import $loc\n";
		continue;
	}
	preg_match('/^[^\s]*/', @$output[0], $matches);
	$id = @$matches[0];
	if (!$id) {
		echo "'adb add' returned no track id for $loc\n";
		continue;
	}

	$output = array();
	$return_var = 0;
	$cmd = "$adb/db-meta $id";
	foreach ($md as $k => $v) {
		$cmd .= " -s ".escapeshellarg("$k=$v");
	}
	exec($cmd, $output, $return_var);
	if ($return_var != 0) {
		echo "failed to set metadata for $loc\n";
		continue;
	}

	// show some progress indication
	$percentage = sprintf("%.0f%%", floor(100.0*$i/count($tracks)));
	if ($percentage != $last_percentage) {
		echo $i."/".count($tracks)." imported ($percentage)\n";
		$last_percentage = $percentage;
	}
}