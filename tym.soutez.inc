<?php
/*
 * greybox
 * $Id: tym.soutez.inc,v 1.12 2007/03/05 15:31:59 che0 Exp $
 */

$tym_id = $_REQUEST["tym_id"];
$soutez_id = $_REQUEST["soutez_id"];

$tym_name = de_html_s(cpdb_fetch_one_value("select nazev from tym where tym_ID = :tym_id",array(":tym_id"=>$tym_id)));
$soutez_name = de_html_s(cpdb_fetch_one_value("select nazev from soutez where soutez_ID = :soutez_id",array(":soutez_id"=>$soutez_id)));

$GLOBALS["page"]->title = $tym_name . " / " . $soutez_name;

pg_head();

cph_p(
	$lang["team"] . ": "
	. cphs_link("./?page=tym&tym_id=" . $tym_id, $tym_name) . cphs_br()
	. $lang["competition"] . ": "
	. cphs_link("./?page=soutez&soutez_id=" . $soutez_id, $soutez_name) . cphs_br()
	. $lang["total points"] . ": "
	. cpdb_fetch_one_value("select sum(body) from debata_tym left join debata on debata.debata_ID = debata_tym.debata_ID where debata.soutez_ID = :soutez_id and debata_tym.tym_ID = :tym_id", array(":soutez_id"=>$soutez_id, ":tym_id"=>$tym_id))
);

cpdb_fetch_all("
	select
		dt_my.body,
		dt_my.pozice,
		tym_oni.tym_ID as oni_ID,
		tym_oni.nazev as oni_tym,
		teze.tx as teze,
		teze.tx_short as teze_short,
		teze.teze_ID as teze_ID,
		datum,
		vitez,
		presvedcive,
		achtung,
		debata.debata_ID
	from
		debata_tym dt_my
		left join debata on dt_my.debata_ID = debata.debata_ID
		left join debata_tym dt_oni on dt_oni.debata_ID = debata.debata_ID and dt_oni.pozice != dt_my.pozice
		left join tym tym_oni on dt_oni.tym_ID = tym_oni.tym_ID
		left join teze on teze.teze_ID = debata.teze_ID
	where
		debata.soutez_ID = :soutez_id
		and dt_my.tym_ID = :tym_id
	order by
		datum desc
	", array(":soutez_id"=>$soutez_id, ":tym_id"=>$tym_id),$r_debaty);
	
de_html($r_debaty);
cph_table_head(array($lang["date"],$lang["position"],$lang["opponent"],$lang["resolution"],$lang["result"],$lang["pts"],""));
foreach ($r_debaty as $debata) {
	cph_table_row(array(
		format_date($debata["datum"]),
		($debata["pozice"] == 1) ? $lang["aff"] : $lang["neg"],
		cphs_link("./?page=tym&tym_id=" . $debata["oni_ID"],$debata["oni_tym"]),
		cphs_link("./?page=teze.detaily&teze_id=" . $debata["teze_ID"],$debata["teze_short"],array("title"=>$debata["teze"])),
		result_for_side($debata["vitez"], $debata["presvedcive"], $debata["pozice"]),
		$debata["body"],
		cphs_link("./?page=debata&debata_id=" . $debata["debata_ID"], $lang["debate details"])
	),
		$debata["achtung"] ? array("class"=>"achtung") : array()
	);
}
cph_table_end();
?>
