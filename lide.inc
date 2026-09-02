<?php
/*
 * greybox
 * $Id: lide.inc,v 1.22 2006/08/23 17:07:33 che0 Exp $
 */

$kdo = isset($_REQUEST["kdo"]) ? $_REQUEST["kdo"] : "vsichni";
$filtr_rozhodci = ($_REQUEST["filtr"] == "samosoudci") ? "samosoudci" : "s_panely";
$filtr_ipts = ($_REQUEST["rocnik"] == "letos") ? "letos" : "vsechno";

$kdo_domain = array("vsichni"=>$lang["everyone"], "debateri"=>$lang["debaters"], "rozhodci"=>$lang["judges"], "spravci"=>$lang["administrators"], "ibody"=>$lang["points overview"], "kidy"=>$lang["kidy"], "kidy_rozhodci"=>$lang["kidy"]." - ".$lang["judges"], "stats"=>$lang["stats"]);
if ($GLOBALS["cps_lidi"] >= 3) {
	$kdo_domain["clenove"] =$lang["members"];
}
$filtr_domain = array("s_panely"=>$lang["include panels"], "samosoudci"=>$lang["single judges"]);
$ipts_domain = array("letos"=>season_to_str($GLOBALS["current_season"]), "vsechno"=>$lang["total"]);

$GLOBALS["page"]->title = $kdo_domain[$kdo];


pg_head();

cph_switch($lang["list"],"./?page=lide&kdo=%s", $kdo_domain, $kdo);

switch ($kdo) {
	case 'vsichni':
	// show list of people
		if ($GLOBALS["cps_lidi"] >= 1) {
			// if user can add new people, this is the page he can do it
			cph_p(cphs_link("./?page=clovek.add&klub_id=" . $_SESSION["user_klub_ID"], $lang['add person']));
		}
		
		cpdb_fetch_all("
			select
				clovek_ID,
				jmeno,
				nick,
				prijmeni,
				klub_ID,
				clen,
				(select kratky_nazev from klub where klub_ID = clovek.klub_ID) as klub,
				(
					ifnull((select sum(ibody) from clovek_debata_ibody where clovek_ID = clovek.clovek_ID and rocnik = :rocnik),
					0)
					+ ifnull((select sum(ibody) from clovek_ibody where clovek_ID = clovek.clovek_ID and rocnik = :rocnik), 0)
				) as ibody
			from
				clovek
			order by
				prijmeni, jmeno, nick
		", array(":rocnik"=>$GLOBALS["current_season"]), $lide);
		
		de_html($lide);
		cph_table_head(
			array($lang["name"], $lang["club"], $lang["full member"], 	$lang["ipoints"])
		);
		
		foreach ($lide as $clovek) {
			cph_table_row(array(
				cphs_link("./?page=clovek&clovek_id=" . $clovek["clovek_ID"],join_name($clovek["jmeno"],$clovek["nick"],$clovek["prijmeni"])),
				cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],$clovek["klub"]),
				$clovek["clen"] ? $lang["yes"] : $lang["no"],
				($clovek["ibody"] > 0) ? $clovek["ibody"] : ""
			),
				$clovek["clen"] ? array() : array("class"=>"inactive")
			);
		}
		
		cph_table_end();
	break;
	
	case 'ibody':
	// show individual points rankings
		cph_switch($lang["season"],"./?page=lide&kdo=ibody&rocnik=%s", $ipts_domain, $filtr_ipts);
		
		switch ($filtr_ipts) {
			case "letos":
				$season_limit = "and rocnik = :rocnik";
			break;
			
			default:
				$season_limit = "";
			break;
		}
		
		if (! cpdb_fetch_all(sprintf("
			select
				clovek_ID,
				jmeno,
				nick,
				prijmeni,
				klub_ID,
				(select kratky_nazev from klub where klub_ID = clovek.klub_ID) as klub,
				(
					ifnull((select sum(ibody) from clovek_debata_ibody where clovek_ID = clovek.clovek_ID %s),
					0)
					+ ifnull((select sum(ibody) from clovek_ibody where clovek_ID = clovek.clovek_ID %s), 0)
				) as ibody
			from
				clovek
			where
				clen = 1
			group by
				clovek_ID, jmeno, nick,
				prijmeni, klub_ID, klub
			having
				ibody > 0
			order by
				ibody desc, prijmeni, jmeno, nick
		", $season_limit, $season_limit), array(":rocnik"=>$GLOBALS["current_season"]), $lide)) {
		
			pg_achtung($lang["no record"]);
			return;
		}
		
		
		cph_p($lang["ranks only for full members"], array("class"=>"note"));
		
		de_html($lide);
		cph_table_head(
			array($lang["rank"], $lang["name"], $lang["club"], $lang["ipoints"])
		);
		
		$count = 0; $rank = 1; $pts = -1;
		foreach ($lide as $clovek) {
			$count++;
			if ($pts != $clovek["ibody"]) {
				$pts = $clovek["ibody"];
				$rank = $count;
			}
			cph_table_row(array(
				$rank,
				cphs_link("./?page=clovek&clovek_id=" . $clovek["clovek_ID"],join_name($clovek["jmeno"],$clovek["nick"],$clovek["prijmeni"])),
				cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],$clovek["klub"]),
				cphs_link(sprintf("./?page=clovek&clovek_id=%s#ibody", $clovek["clovek_ID"]), $clovek["ibody"])
			));
		}
		
		cph_table_end();
	break;
	
	case 'kidy':
		// show kidy rankings
		
		if (! cpdb_fetch_all("
			select
				clovek.clovek_ID,
				clovek.jmeno,
				clovek.nick,
				clovek.prijmeni,
				klub.kratky_nazev as klub,
				klub.klub_ID,
				avg(kidy) as avg,
				count(kidy) as samples
			from
				clovek
				left join klub on clovek.klub_ID = klub.klub_ID
				left join clovek_debata on clovek.clovek_ID = clovek_debata.clovek_ID
					and find_in_set(clovek_debata.role,'a1,a2,a3,n1,n2,n3')
				left join debata on clovek_debata.debata_ID = debata.debata_ID
				left join soutez on debata.soutez_ID = soutez.soutez_ID
			where
				clovek.debater = 1
				and clovek.clen = 1
				and soutez.rocnik = :rocnik
			group by
				clovek_ID, jmeno, nick, prijmeni, klub, klub_ID
			having
				samples > 0
			order by
				avg desc
			", array(":rocnik"=>$GLOBALS["current_season"]), $r_kidy)) {
			
			pg_achtung($lang["no record"]);
			return;
		}
		
		cph_p($lang["ranks only for full members"], array("class"=>"note"));
		
		de_html($r_kidy);
		cph_table_head(array($lang["rank"], $lang["rank"], $lang["debater"], $lang["club"], $lang["average"], $lang["samples"]));
		$count = 0; $rank = 1; $pts = -1;
		$countF = 0; $rankF = 1; $ptsF = -1;
		foreach ($r_kidy as $kidy) {
			$count++;
			if ($pts != $kidy["avg"]) {
				$pts = $kidy["avg"];
				$rank = $count;
			}
			$fullrank = ($kidy["samples"] >= $GLOBALS["cp_config"]["min_kidy_rank"]);
			if ($fullrank) {
				$countF++;
				if ($ptsF != $kidy["avg"]) {
				$ptsF = $kidy["avg"];
				$rankF = $countF;
				}
			}
			cph_table_row(array(
				$fullrank ? $rankF : "",
				$rank,
				cphs_link("./?page=clovek&clovek_id=" . $kidy["clovek_ID"], join_name($kidy["jmeno"],$kidy["nick"],$kidy["prijmeni"])),
				cphs_link("./?page=klub&klub_id=" . $kidy["klub_ID"], $kidy["klub"]),
				cphs_link(sprintf("./?page=clovek&clovek_id=%s#debaty", $kidy["clovek_ID"]), $kidy["avg"]),
				$kidy["samples"]
			),
				$fullrank ? array() : array("class"=>"inactive") 
			);
		}
		cph_table_end();

	break;
	
	case 'kidy_rozhodci':
		// show average ranks given by the judges
		
		switch ($filtr_rozhodci) {
			case "samosoudci":
				$filtr_sql = "
					and (
						select count(*) 
						from clovek_debata cd_r2
						where
							cd_r2.debata_ID = debata.debata_ID
							and cd_r2.role = 'r'
					) = 1
				";
			break;
			
			case "s_panely":
				$filtr_sql = "";
			break;
		}
		
		cph_switch($lang["filter"],"./?page=lide&kdo=kidy_rozhodci&filtr=%s", $filtr_domain, $filtr_rozhodci);
		
		if (! cpdb_fetch_all(sprintf("
			select
				clovek.clovek_ID,
				jmeno,
				nick,
				prijmeni,
				avg(cd_hodnoceni.kidy) as avg,
				count(cd_hodnoceni.kidy) as samples,
				std(cd_hodnoceni.kidy) as stddev
			from
				rozhodci
				left join clovek
					on rozhodci.clovek_ID = clovek.clovek_ID
				left join clovek_debata cd_rozhoduje
					on clovek.clovek_ID = cd_rozhoduje.clovek_ID and cd_rozhoduje.role = 'r'
				left join debata
					on cd_rozhoduje.debata_ID = debata.debata_ID
				left join soutez
					on debata.soutez_ID = soutez.soutez_ID
				left join clovek_debata cd_hodnoceni
					on cd_rozhoduje.debata_ID = cd_hodnoceni.debata_ID and find_in_set(cd_hodnoceni.role,'a1,a2,a3,n1,n2,n3')
			where
				rozhodci.rocnik = :rocnik
				and soutez.rocnik = :rocnik
				%s
			group by
				clovek.clovek_ID, jmeno, nick, prijmeni
			having
				samples > 0
			order by
				avg desc
			",$filtr_sql), array(":rocnik"=>$GLOBALS["current_season"]), $r_kidy)) {
			
			// when there are no results
			pg_achtung($lang["no record"]);
			return;
		}
		
		
		de_html($r_kidy);
		cph_table_head(array($lang["name"], $lang["average"], $lang["samples"], $lang["stddev"]));
		foreach ($r_kidy as $kidy) {
			cph_table_row(array(
				cphs_link("./?page=clovek&clovek_id=" . $kidy["clovek_ID"], join_name($kidy["jmeno"],$kidy["nick"],$kidy["prijmeni"])),
				$kidy["avg"],
				$kidy["samples"],
				$kidy["stddev"]
			),
				$kidy["samples"] >= $GLOBALS["cp_config"]["min_kidy_judge"] ?
					array() : array("class"=>"inactive") 
			);
		}
		cph_table_end();
	break;
	
	case 'debateri':
		// show list of debaters
		
		if (! cpdb_fetch_all("
			select
				clovek.clovek_ID,
				clovek.jmeno,
				clovek.nick,
				clovek.prijmeni,
				clovek.klub_ID,
				klub.kratky_nazev as klub
			from 
				clovek
				left join klub using (klub_ID)
			where
				clovek.debater = 1
			order by
				clovek.prijmeni, clovek.jmeno, clovek.nick
		", array(), $lide)) {
			
			pg_achtung($lang["no record"]);
			return;
		}
		
		de_html($lide);
		cph_table_head(
			array($lang["name"], $lang["club"], $lang["team"], $lang["ipoints"])
		);
		
		foreach ($lide as $clovek) {
			cpdb_fetch_all("
				select
					tym.tym_ID, tym.nazev
				from
					clovek_tym left join tym using (tym_ID)
				where
					clovek_tym.clovek_ID = :clovek_id
					and clovek_tym.aktivni = 1
			", array(":clovek_id"=>$clovek["clovek_ID"]), $tymy);
			de_html($tymy);
			$prvni = true; $f_tymy = "";
			foreach ($tymy as $tym) {
				if ($prvni) {
					$prvni = false;
				} else {
					$f_tymy .= ", ";
				}
				$f_tymy .= cphs_link("./?page=tym&tym_id=" . $tym["tym_ID"], $tym["nazev"]);
			}
			
			cph_table_row(array(
				cphs_link("./?page=clovek&clovek_id=" . $clovek["clovek_ID"],join_name($clovek["jmeno"],$clovek["nick"],$clovek["prijmeni"])),
				cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],$clovek["klub"]),
				$f_tymy,
				cpdb_fetch_one_value("select sum(ibody) from clovek_debata_ibody where clovek_ID = :clovek_id and rocnik = :rocnik and role = 'debater'", array(":clovek_id"=>$clovek["clovek_ID"], ":rocnik"=>$GLOBALS["current_season"]))
			));
		}
		
		cph_table_end();
	break;

	
	case 'rozhodci':
		cpdb_fetch_all("
			select
				clovek.clovek_ID,
				clovek.jmeno,
				clovek.nick,
				clovek.prijmeni,
				clovek.klub_ID,
				rozhodci.misto,
				rozhodci.jazyk,
				rozhodci.format,
				rozhodci.uroven,
				klub.kratky_nazev as klub
			from 
				rozhodci
				left join clovek on clovek.clovek_ID = rozhodci.clovek_ID
				left join klub on klub.klub_ID = clovek.klub_ID
			where
				rozhodci.rocnik = :rocnik
			order by
				clovek.prijmeni, clovek.jmeno, clovek.nick
		", array(":rocnik"=>$GLOBALS["current_season"]), $lide);
		
		de_html($lide);
		cph_table_head(
			array($lang["name"], $lang["club"], $lang["level"], $lang["format"], $lang["languages"], $lang["place"], $lang["ipoints"])
		);
		
		foreach ($lide as $clovek) {
			cph_table_row(array(
				cphs_link("./?page=clovek&clovek_id=" . $clovek["clovek_ID"],join_name($clovek["jmeno"],$clovek["nick"],$clovek["prijmeni"])),
				cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],$clovek["klub"]),
				$clovek["uroven"],
				$clovek["format"],
				$clovek["jazyk"],
				$clovek["misto"],
				cpdb_fetch_one_value("select sum(ibody) from clovek_debata_ibody where clovek_ID = :clovek_id and rocnik = :rocnik and role = 'rozhodci'", array(":clovek_id"=>$clovek["clovek_ID"], ":rocnik"=>$GLOBALS["current_season"]))
			));
		}
		cph_table_end();
	break;
	
	case 'spravci':
		// show list of admins
		
		include_once("i_domeny.inc");
		
		cpdb_fetch_all("
			select
				clovek.clovek_ID,
				clovek.jmeno,
				clovek.nick,
				clovek.prijmeni,
				clovek.klub_ID,
				klub.kratky_nazev as klub,
				clovek.prava_lidi,
				clovek.prava_kluby,
				clovek.prava_souteze,
				clovek.prava_debaty
			from 
				clovek
				left join klub using (klub_ID)
			where
				clovek.prava_lidi +	clovek.prava_kluby + clovek.prava_souteze + clovek.prava_debaty > 0
			order by
				clovek.prijmeni, clovek.jmeno, clovek.nick
		", array(), $lide);
		
		de_html($lide);
		cph_table_head(
			array($lang["administrator"], $lang["people"], $lang["clubs"], $lang["competitions"], $lang["debates"])
		);
		
		foreach ($lide as $clovek) {
			if ($clovek["prava_lidi"] == 1) {
				$prava_lidi = cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"], $clovek["klub"]);
			} else {
				$prava_lidi = $domain_prava_lidi[$clovek["prava_lidi"]];
			}
			if ($clovek["prava_kluby"] == 1) {
				$prava_kluby = cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"], $clovek["klub"]);
			} else {
				$prava_kluby = $domain_prava_kluby[$clovek["prava_kluby"]];
			}
			
			cph_table_row(array(
				cphs_link("./?page=clovek&clovek_id=" . $clovek["clovek_ID"],join_name($clovek["jmeno"],$clovek["nick"],$clovek["prijmeni"])),
				$prava_lidi,
				$prava_kluby,
				$domain_prava_souteze[$clovek["prava_souteze"]],
				$domain_prava_debaty[$clovek["prava_debaty"]]
			));
		}
		
		cph_table_end();
	break;
	
	case 'stats':
		// show people stats
		
		cph_tag_start("table");
		cph_table_row(array(
			$lang["total number of people in the database"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek")
		));
		cph_table_row(array(
			$lang["active debaters"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where debater = 1")
		));
		cph_table_row(array(
			$lang["active judges"],
			cpdb_fetch_one_value("select count(distinct clovek_ID) from rozhodci where rocnik = :rocnik", array(":rocnik"=>$GLOBALS["current_season"]))
		));
		cph_table_row(array(
			$lang["full members"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where clen = 1")
		));
		cph_table_end();
		
		cph_h2($lang["full members by age"]);
		cph_tag_start("table");
		cph_table_row(array(
			$lang["<15"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where clen = 1 and narozen > date_sub(now(), interval 15 year)")
		));
		cph_table_row(array(
			$lang["15-18"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where clen = 1 and narozen <= date_sub(now(), interval 15 year) and narozen > date_sub(now(), interval 18 year)")
		));
		cph_table_row(array(
			$lang["18-26"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where clen = 1 and narozen <= date_sub(now(), interval 18 year) and narozen > date_sub(now(), interval 26 year)")
		));
		cph_table_row(array(
			$lang["26+"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where clen = 1 and narozen <= date_sub(now(), interval 26 year)")
		));
		cph_table_row(array(
			$lang["unset"],
			cpdb_fetch_one_value("select count(clovek_ID) from clovek where clen = 1 and narozen is null")
		));
		cph_table_end();
	break;
	case 'clenove':
		// show list of members with details
		if ($GLOBALS["cps_lidi"] < 3)
		{
			pg_achtung($lang["access denied"]);
			return;
		}
		cpdb_fetch_all("
			select
				clovek_ID,
				jmeno,
				nick,
				prijmeni,
				klub_ID,
				narozen,
				prihlaska,
				prohlaseni,
				clen,
				(select kratky_nazev from klub where klub_ID = clovek.klub_ID) as klub,
				(
					ifnull((select sum(ibody) from clovek_debata_ibody where clovek_ID = clovek.clovek_ID and rocnik = :rocnik),
					0)
					+ ifnull((select sum(ibody) from clovek_ibody where clovek_ID = clovek.clovek_ID and rocnik = :rocnik), 0)
				) as ibody
			from
				clovek
			where
				clen = 1
			order by
				prijmeni, jmeno, nick
		", array(":rocnik"=>$GLOBALS["current_season"]), $lide);
		
		de_html($lide);
		cph_table_head(
			array($lang["name"], $lang["club"], $lang["application"], $lang["guardian statement"])
		);
		
		$eighteen = date("Y-m-d", strtotime("- 18 years"));
		foreach ($lide as $clovek) {
			$prohlaseni = $clovek["prohlaseni"] ? $lang["yes"] : $lang["no"];
			if (!is_null($clovek["narozen"]) and $clovek["narozen"] != "0000-00-00")
			{
				if($clovek["narozen"] < $eighteen)
				{
					$prohlaseni = $lang["unnecessary"];
				}
			}
			cph_table_row(array(
				cphs_link("./?page=clovek&clovek_id=" . $clovek["clovek_ID"],join_name($clovek["jmeno"],$clovek["nick"],$clovek["prijmeni"])),
				cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],$clovek["klub"]),
				$clovek["prihlaska"] ? $lang["yes"] : $lang["no"],
				$prohlaseni
			)
			);
		}
		
		cph_table_end();
	break;
}

?>
