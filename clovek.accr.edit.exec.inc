<?php
/*
 * greybox
 * $Id: clovek.accr.edit.exec.inc,v 1.6 2006/08/23 17:07:33 che0 Exp $
 */

$clovek_id = $_REQUEST["clovek_id"];
$rocnik_old = $_REQUEST["rocnik_old"];
$jazyk = $_REQUEST["jazyk"];
$misto = $_REQUEST["misto"];
$rocnik = $_REQUEST["rocnik"];
$format = $_REQUEST["format"];
$uroven = isset($_REQUEST["uroven"]) ? (string) $_REQUEST["uroven"] : "";

if (! ($GLOBALS["cps_lidi"] >= 2)) {
	pg_achtung($lang["access denied"]);
	return;
}

if ((isset($_REQUEST["add"]) || isset($_REQUEST["save"])) && !in_array($uroven, array("0", "1", "2", "3"), true)) {
	pg_achtung($lang["accreditation level invalid"]);
	include("clovek.inc");
	return;
}

$uroven = (int) $uroven;

if (isset($_REQUEST["add"])) {
	// add new
	if (cpdb_exec("insert into rozhodci (clovek_ID, jazyk, misto, rocnik, format, uroven) values (:clovek_id, :jazyk, :misto, :rocnik, :format, :uroven)", array(":rocnik"=>$rocnik, ":jazyk"=>$jazyk, ":misto"=>$misto, ":format"=>$format, ":uroven"=>$uroven, ":clovek_id"=>$clovek_id))) {
		pg_achtung($lang["accrediation update ok"]);
		include("clovek.inc");
	}
	
} elseif (isset($_REQUEST["save"])) {
	// update
	
	if (cpdb_exec("update rozhodci set jazyk = :jazyk, misto = :misto, rocnik = :rocnik, format = :format, uroven = :uroven where clovek_id = :clovek_id and rocnik = :rocnik_old", array(":clovek_id"=>$clovek_id, ":rocnik_old"=>$rocnik_old, ":rocnik"=>$rocnik, ":jazyk"=>$jazyk, ":misto"=>$misto, ":format"=>$format, ":uroven"=>$uroven))) {
		pg_achtung($lang["accrediation update ok"]);
		include("clovek.inc");
	}
	
} elseif (isset($_REQUEST["delete"])) {
	// delete
	
	if (cpdb_exec("delete from rozhodci where clovek_id = :clovek_id and rocnik = :rocnik_old", array(":clovek_id"=>$clovek_id, ":rocnik_old"=>$rocnik_old))) {
		pg_achtung($lang["accrediation update ok"]);
		include("clovek.inc");
	}
}

?>
