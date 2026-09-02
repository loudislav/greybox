<?php
/*
 * greybox
 * $Id: clovek.inc,v 1.43 2007/03/05 15:31:59 che0 Exp $
 */

include_once("i_domeny.inc");

$clovek_id = $_REQUEST["clovek_id"];

if (!cpdb_fetch_one_row("select * from clovek where clovek_ID = :clovek_id", array(":clovek_id"=>$clovek_id), $clovek_p)) {
	pg_achtung($lang["no record"]);
	return;
}
$clovek = de_html_s($clovek_p);

$GLOBALS["page"]->title = join_name($clovek["jmeno"], $clovek["nick"], $clovek["prijmeni"]);
pg_head();
if (isset($clovek["komentar"])) cph_p($clovek["komentar"], array("class"=>"komentar"));

$can_edit = 
	($GLOBALS["cps_lidi"] >= 2)
	|| (
		$GLOBALS["cps_lidi"] == 1
		&& $clovek["klub_ID"] == $_SESSION["user_klub_ID"]
);

$self_edit = ($clovek["clovek_ID"] == $_SESSION["user_clovek_ID"]);

// basic details
if ($can_edit) {
	// edit all 
	$clovek_f = new cpykhen;
	$clovek_f->form_attr["action"] = "./?page=clovek.edit.exec";
	$clovek_f->form[] = new cpf_hidden("clovek_id",$clovek["clovek_ID"]);
	$clovek_f->form[] = new cpf_text($lang["name"] . ": ");
	$clovek_f->form[] = new cpf_db_textbox("jmeno","jmeno",array("maxlength"=>40));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["surname"] . ": ");
	$clovek_f->form[] = new cpf_db_textbox("prijmeni","prijmeni",array("maxlength"=>60));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["nick"] . ": ");
	$clovek_f->form[] = new cpf_db_textbox("nick","nick",array("maxlength"=>60));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("email: ");
	$clovek_f->form[] = new cpf_db_textbox("email","email",array());
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["club"] . ": ");
	$clovek_f->form[] = new cpf_db_listbox("klub_id",array(
		"select klub_ID, nazev from klub union select 0, :no_club", array(":no_club"=>$lang["no club"])),"klub_ID");
	$clovek_f->form[] = new cpf_db_link("klub_ID","./?page=klub&klub_id=%s",$lang["link"]);
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["born"] . ": ");
	$clovek_f->form[] = new cpf_db_textbox("narozen","narozen",array("maxlength"=>10));
	$clovek_f->form[] = new cpf_text(" " . $lang["yyyy-mm-dd"],array("class"=>"desc"));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("ulice a cislo: ");
	$clovek_f->form[] = new cpf_db_textbox("ulice","ulice",array("class"=>'smartform-street-and-number'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("mesto: ");
	$clovek_f->form[] = new cpf_db_textbox("mesto","mesto",array("class"=>'smartform-city'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("psc: ");
	$clovek_f->form[] = new cpf_db_textbox("psc","psc",array("class"=>'smartform-zip'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("prukaz: ");
	$clovek_f->form[] = new cpf_db_listbox_array("prukaz",array('op' => 'obcansky prukaz', 'pas' => 'cestovni pas', 'jiny' => 'jiny'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("cislo: ");
	$clovek_f->form[] = new cpf_db_textbox("cislo","cislo",array());
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["comment"] . ": ");
	$clovek_f->form[] = new cpf_db_textarea("komentar","komentar",array("rows"=>4, "cols"=>60));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["active debater"] . ": ");
	$clovek_f->form[] = new cpf_db_listbox_array("debater",array(1 => $lang['yes'], 0 => $lang['no']));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["full member"] . ": ");
	if ($GLOBALS["cps_lidi"] >= 3) {
		// only admins can edit membership
		$clovek_f->form[] = new cpf_db_listbox_array("clen", array(1 => $lang['yes'], 0 => $lang['no']));
	} else {
		$clovek_f->form[] = new cpf_db_text("clen", array(), array(1 => $lang['yes'], 0 => $lang['no']));
	}
    if ($GLOBALS["cps_lidi"] >= 3) {
    	// only admins can edit membership
	    $clovek_f->form[] = new cpf_nl;
        $clovek_f->form[] = new cpf_text($lang["member until"] . ": ");
        $clovek_f->form[] = new cpf_db_textbox("clen_do","clen_do",array("maxlength"=>4));

		// only admins can confirm application submission
		$clovek_f->form[] = new cpf_nl;
		$clovek_f->form[] = new cpf_text($lang["application"] . ": ");
		$clovek_f->form[] = new cpf_db_listbox_array("prihlaska", array(1 => $lang['yes'], 0 => $lang['no']));

		// only admins can confirm guardian statement submission
		$clovek_f->form[] = new cpf_nl;
		$clovek_f->form[] = new cpf_text($lang["guardian statement"] . ": ");
		$clovek_f->form[] = new cpf_db_listbox_array("prohlaseni", array(1 => $lang['yes'], 0 => $lang['no']));
    }
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_button("save",$lang["save"]);
	//$clovek_f->form[] = new cpf_button("delete",$lang["delete"]);
	$clovek_f->load_this($clovek_p);
	$clovek_f->render();
	
} elseif ($self_edit) {
	// edit self
	$clovek_f = new cpykhen;
	$clovek_f->form_attr["action"] = "./?page=clovek.self.exec";
	$clovek_f->form[] = new cpf_hidden("clovek_id",$clovek["clovek_ID"]);
	$clovek_f->form[] = new cpf_text($lang["name"] . ": ");
	$clovek_f->form[] = new cpf_db_text("jmeno");
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["surname"] . ": ");
	$clovek_f->form[] = new cpf_db_text("prijmeni");
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["nick"] . ": ");
	$clovek_f->form[] = new cpf_db_textbox("nick","nick",array("maxlength"=>60));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("email: ");
	$clovek_f->form[] = new cpf_db_textbox("email","email",array());
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("ulice a cislo: ");
	$clovek_f->form[] = new cpf_db_textbox("ulice","ulice",array("class"=>'smartform-street-and-number'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("mesto: ");
	$clovek_f->form[] = new cpf_db_textbox("mesto","mesto",array("class"=>'smartform-city'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("psc: ");
	$clovek_f->form[] = new cpf_db_textbox("psc","psc",array("class"=>'smartform-zip'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("prukaz: ");
	$clovek_f->form[] = new cpf_db_listbox_array("prukaz",array('op' => 'obcansky prukaz', 'pas' => 'cestovni pas', 'jiny' => 'jiny'));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text("cislo: ");
	$clovek_f->form[] = new cpf_db_textbox("cislo","cislo",array());
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_text($lang["comment"] . ": ");
	$clovek_f->form[] = new cpf_db_textarea("komentar","komentar",array("rows"=>4, "cols"=>60));
	$clovek_f->form[] = new cpf_nl;
	$clovek_f->form[] = new cpf_button("save",$lang["save"]);
	$clovek_f->load_this($clovek_p);
	$clovek_f->render();
	
	if (is_null($clovek["narozen"])) {
		$clovek_f = new cpykhen;
		$clovek_f->form_attr["action"] = "./?page=clovek.self.exec";
		$clovek_f->form[] = new cpf_hidden("clovek_id",$clovek["clovek_ID"]);
		$clovek_f->form[] = new cpf_text($lang["born"] . ": ");
		$clovek_f->form[] = new cpf_db_textbox("narozen","narozen",array("maxlength"=>10));
		$clovek_f->form[] = new cpf_button("born",$lang["save"]);
		$clovek_f->render_new();
	} else {
		cph_p($lang["born"] . ": " . $clovek["narozen"]);
	}
	
	if (isset($clovek["klub_ID"])) cph_p($lang["club"] . ": " . cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],cpdb_fetch_one_value("select nazev from klub where klub_ID = :klub_id",array(":klub_id"=>$clovek["klub_ID"]))));
	
	cph_p($lang["active debater"] . ": " . ($clovek["debater"] ? $lang["yes"] : $lang["no"]) . 
		cphs_br() . $lang["full member"] . ": " . ($clovek["clen"] ? $lang["yes"] : $lang["no"]));
	
} else {
	// just view
	
	if (isset($clovek["klub_ID"])) cph_p($lang["club"] . ": " . cphs_link("./?page=klub&klub_id=" . $clovek["klub_ID"],cpdb_fetch_one_value("select nazev from klub where klub_ID = :klub_id",array(":klub_id"=>$clovek["klub_ID"]))));
	
	cph_p($lang["active debater"] . ": " . ($clovek["debater"] ? $lang["yes"] : $lang["no"]) . 
		cphs_br() . $lang["full member"] . ": " . ($clovek["clen"] ? $lang["yes"] : $lang["no"]));
}


// club positions (coach)
if (cpdb_fetch_all("
	select
		clovek_klub.role,
		clovek_klub.rocnik,
		klub.klub_ID,
		klub.nazev
	from
		clovek_klub
		left join klub using (klub_ID)
	where
		clovek_klub.clovek_ID = :clovek_id
	order by
		clovek_klub.rocnik
	",
	array(":clovek_id"=>$clovek_id),
	$positions)) {
	
	de_html($positions);
	cph_h2($lang["club positions"]);
	cph_table_head(array($lang["role"],$lang["season"],$lang["club"]));
	foreach ($positions as $pos) {
		cph_table_row(array(
			$domain_klub_role[$pos["role"]],
			season_to_str($pos["rocnik"]),
			cphs_link("./?page=klub&klub_id=" . $pos["klub_ID"],$pos["nazev"])
		));
	}
	cph_table_end();
}

// accreditations
if ($GLOBALS["cps_lidi"] >= 2) {
	cph_h2($lang["accreditations"]);
	
	$accr = new cpykhen;
	$accr->form_attr["action"] = "./?page=clovek.accr.edit.exec";
	$accr->select = "select * from rozhodci where clovek_ID = :clovek_id";
	$accr->select_args = array(":clovek_id"=>$clovek_id);
	$accr->form[] = new cpf_hidden("clovek_id",$clovek_id);
	$accr->form[] = new cpf_db_hidden("rocnik_old","rocnik");
	$accr->form[] = new cpf_text($lang["season"] . ": ");
	$accr->form[] = new cpf_db_listbox_array("rocnik", $GLOBALS["season_domain"]);
	$accr->form[] = new cpf_text(" " . $lang["level"] . ": ");
	$accr->form[] = new cpf_db_listbox_array("uroven", array(0=>"", 1=>1, 2=>2, 3=>3));
	$accr->form[] = new cpf_text(" " . $lang["language"] . ": ");
	$accr->form[] = new cpf_db_textbox("jazyk");
	$accr->form[] = new cpf_text(" " . $lang["separate by commas: en,cz"],array("class"=>"desc"));
	$accr->form[] = new cpf_nl;
	$accr->form[] = new cpf_db_textbox("format");
	$accr->form[] = new cpf_text(" " . $lang["separate by commas: DL,DP,SD,SD2-2"],array("class"=>"desc"));
	$accr->form[] = new cpf_nl;
	$accr->form[] = new cpf_text($lang["place"] . ": ");
	$accr->form[] = new cpf_db_textbox("misto","misto",array("maxlength"=>255));
	$accr->form["b1"] = new cpf_button("save",$lang["save"]);
	//$accr->form["b2"] = new cpf_button("delete",$lang["delete"]);
	$accr->render_select();
	
	$accr->form["b1"] = new cpf_button("add",$lang["add"]);
	unset($accr->form["b2"]);
	$accr->load_this(array("rocnik"=>$GLOBALS["current_season"], "uroven"=>0));
	$accr->render_hidden($lang["new accreditation"]);
} else {
	if (cpdb_fetch_all("
		select rocnik, uroven, format, misto, jazyk from rozhodci where clovek_ID = :clovek_id order by rocnik",
		array(":clovek_id"=>$clovek_id),
		$accrs)) {
		
		de_html($accrs);
		cph_h2($lang["accreditations"]);
		cph_table_head(array($lang["season"],$lang["level"],$lang["format"],$lang["language"],$lang["place"]));
		foreach ($accrs as $accr) {
			cph_table_row(array(
				season_to_str($accr["rocnik"]),
				$accr["uroven"] == 0 ? "" : $accr["uroven"],
				$accr["format"],
				$accr["jazyk"],
				$accr["misto"]
			));
		}
		cph_table_end();
	}
}

// contacts
if ($can_edit || $self_edit) {
	cph_h2($lang["contacts"]);
	
	$cont = new cpykhen;
	$cont->form_attr["action"] = "./?page=clovek.cont.edit.exec";
	$cont->select = "select * from kontakt where clovek_ID = :clovek_id";
	$cont->select_args = array(":clovek_id"=>$clovek_id);
	$cont->form[] = new cpf_hidden("clovek_id",$clovek_id);
	$cont->form[] = new cpf_db_hidden("kontakt_id","kontakt_ID");
	$cont->form[] = new cpf_db_listbox_array("druh", $domain_kontakt_druh);
	$cont->form[] = new cpf_db_textbox("tx","tx",array("size"=>40,"maxlength"=>255));
	$cont->form[] = new cpf_db_listbox_array("viditelnost", $domain_kontakt_viditelnost);
	$cont->form["b1"] = new cpf_button("save",$lang["save"]);
	//$cont->form["b2"] = new cpf_button("delete",$lang["delete"]);
	$cont->render_select();
	
	$cont->form["b1"] = new cpf_button("add",$lang["add"]);
	unset($cont->form["b2"]);
	$cont->load_new();
	$cont->render_hidden($lang["new contact"]);
} else {

	if (!is_null($clovek["klub_ID"]) && $_SESSION["user_klub_ID"] == $clovek["klub_ID"]) {
		$thresh = 2;
	} elseif ($_SESSION["is_logged_in"] == true) {
		$thresh = 1;
	} else {
		$thresh = 0;
	}
	
	if (cpdb_fetch_all("
		select druh, tx from kontakt where clovek_ID = :clovek_id and viditelnost <= :thresh",
		array(":clovek_id"=>$clovek_id, ":thresh"=>$thresh),
		$contacts)) {
		
		de_html($contacts);
		cph_h2($lang["contacts"]);
		cph_table_head(array($lang["type"],$lang["contact"]));
		foreach ($contacts as $cont) {
			switch ($cont["druh"]) {
				case "email":
					$tx = cphs_link("mailto:" . $cont["tx"], $cont["tx"]);
				break;
				
				case "web":
					$tx = cphs_link($cont["tx"], $cont["tx"]);
				break;
				
				default:
					$tx = $cont["tx"];
				break;
			}
			cph_table_row(array(
				$domain_kontakt_druh[$cont["druh"]],
				$tx
			));
		}
		cph_table_end();
	}
}

// access
if ($GLOBALS["cps_lidi"] >= 3) {
	cph_h2($lang["access levels"]);
	$access = new cpykhen;
	$access->form_attr["action"] = "./?page=clovek.edit.exec";
	$access->form[] = new cpf_hidden("clovek_id",$clovek_id);
	$access->form[] = new cpf_text($lang["people"] . ": ");
	$access->form[] = new cpf_db_listbox_array("prava_lidi",$domain_prava_lidi);
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["clubs"] . ": ");
	$access->form[] = new cpf_db_listbox_array("prava_kluby",$domain_prava_kluby);
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["competitions"] . ": ");
	$access->form[] = new cpf_db_listbox_array("prava_souteze",$domain_prava_souteze);
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["debates"] . ": ");
	$access->form[] = new cpf_db_listbox_array("prava_debaty",$domain_prava_debaty);
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_button("access",$lang["save"]);
	$access->load_this($clovek_p);
	$access->render();
	
} elseif ($clovek["prava_lidi"]+$clovek["prava_kluby"]+$clovek["prava_souteze"]+$clovek["prava_debaty"] > 0) {
	cph_h2($lang["access levels"]);
	cph_table_head(array($lang["area"],$lang["access"]));
	cph_table_row(array($lang["people"],$domain_prava_lidi[$clovek["prava_lidi"]]));
	cph_table_row(array($lang["clubs"],$domain_prava_kluby[$clovek["prava_kluby"]]));
	cph_table_row(array($lang["competitions"],$domain_prava_souteze[$clovek["prava_souteze"]]));
	cph_table_row(array($lang["debates"],$domain_prava_debaty[$clovek["prava_debaty"]]));
	cph_table_end();
}

// login
if ($GLOBALS["cps_lidi"] >= 3) {
	cph_h2($lang["login"]);
	
	$access = new cpykhen;
	$access->form_attr["action"] = "./?page=clovek.pw.edit.exec";
	$access->form[] = new cpf_hidden("clovek_id",$clovek_id);
	$access->form[] = new cpf_text($lang["username"] . ": ");
	$access->form[] = new cpf_db_textbox("username","username",array("maxlength"=>32));
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["password"] . ": ");
	$access->form[] = new cpf_db_password("n1");
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["password"] . ": ");
	$access->form[] = new cpf_db_password("n2");
	$access->form[] = new cpf_nl;
	
	if (cpdb_fetch_one_row("select username from login where clovek_ID = :clovek_id",array(":clovek_id"=>$clovek_id),$login)) {
		$access->form[] = new cpf_button("save",$lang["save"]);
		//$access->form[] = new cpf_button("delete",$lang["delete"]);
		$access->load_this($login);
		$access->render();
	} else {
		$access->form[] = new cpf_button("add",$lang["add"]);
		$access->render_new();

	}
	
} elseif ($self_edit) {
	cph_h2($lang["password"]);
	$access = new cpykhen;
	$access->form_attr["action"] = "./?page=clovek.pw.edit.exec";
	$access->form[] = new cpf_hidden("clovek_id",$clovek_id);
	$access->form[] = new cpf_text($lang["old"] . ": ");
	$access->form[] = new cpf_db_password("old");
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["new"] . ": ");
	$access->form[] = new cpf_db_password("n1");
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_text($lang["new"] . ": ");
	$access->form[] = new cpf_db_password("n2");
	$access->form[] = new cpf_nl;
	$access->form[] = new cpf_button("password",$lang["change password"]);
	$access->load_this($clovek_p);
	$access->render();
}

// teams
if (cpdb_fetch_all("select tym.tym_ID, tym.nazev, clovek_tym.aktivni from clovek_tym left join tym using (tym_ID) where clovek_tym.clovek_ID = :clovek_id order by clovek_tym.aktivni desc, tym.nazev", array(":clovek_id"=>$clovek_id),$tymy)) {
	cph_h2($lang["teams"]);
	cph_table_head(array($lang["status"],$lang["team"]));
	foreach ($tymy as $tym) {
		cph_table_row(array(
			$tym["aktivni"] ? $lang["active"] : $lang["disabled"],
			cphs_link("./?page=tym&tym_id=" . $tym["tym_ID"], $tym["nazev"])
		), $tym["aktivni"] ? array() : array("class"=>"inactive"));
	}
	cph_table_end();
}

// tournaments (org)
if (cpdb_fetch_all("select clovek_turnaj.role, turnaj.nazev, turnaj.datum_od, turnaj.turnaj_ID from clovek_turnaj left join turnaj using (turnaj_ID) where clovek_turnaj.clovek_ID = :clovek_id order by turnaj.datum_od", array(":clovek_id"=>$clovek_id), $turnaje)) {
	de_html($turnaje);
	cph_h2($lang["tournaments"]);
	cph_table_head(array($lang["role"],$lang["tournament"],$lang["date"]));
	foreach ($turnaje as $turnaj) {
		cph_table_row(array(
			$domain_turnaj_role[$turnaj["role"]],
			cphs_link("./?page=turnaj&turnaj_id=" . $turnaj["turnaj_ID"], $turnaj["nazev"]),
			format_date($turnaj["datum_od"])
		));
	}
	cph_table_end();
}

// debates
$cur_debaty = cpdb_fetch_all("
	select
		tym_aff.tym_ID as aff_ID,
		tym_aff.nazev as aff_tym,
		tym_neg.tym_ID as neg_ID,
		tym_neg.nazev as neg_tym,
		teze.tx as teze,
		teze.tx_short as teze_short,
		teze.teze_ID as teze_ID,
		debata.datum,
		debata.vitez,
		debata.presvedcive,
		debata.achtung,
		debata.debata_ID,
		clovek_debata.role,
		clovek_debata.kidy,
		clovek_debata.rozhodnuti as adj_rozhodnuti,
		clovek_debata.presvedcive as adj_presvedcive
	from
		clovek_debata
		left join debata on debata.debata_ID = clovek_debata.debata_ID
		left join soutez on debata.soutez_ID = soutez.soutez_ID
		left join debata_tym dt_aff on dt_aff.debata_ID = debata.debata_ID and dt_aff.pozice = 1
		left join debata_tym dt_neg on dt_neg.debata_ID = debata.debata_ID and dt_neg.pozice = 0
		left join tym tym_aff on dt_aff.tym_ID = tym_aff.tym_ID
		left join tym tym_neg on dt_neg.tym_ID = tym_neg.tym_ID
		left join teze on teze.teze_ID = debata.teze_ID
	where
		clovek_debata.clovek_ID = :clovek_id
		and soutez.rocnik = :rocnik
	order by
		datum desc
	", array(":clovek_id"=>$clovek_id, ":rocnik"=>$GLOBALS["current_season"]),$r_debaty
);

$total_debaty = cpdb_fetch_one_value(
	"select count(distinct debata_ID) from clovek_debata where clovek_ID = :clovek_id",
	array(":clovek_id"=>$clovek_id));

if ($total_debaty) {
	// if there are any debates at all...
	$season_debaty = cpdb_fetch_one_value(
		"select count(distinct cd.debata_ID) from clovek_debata cd left join debata using (debata_ID) left join soutez using (soutez_ID) where clovek_ID = :clovek_id and rocnik = :rocnik",
		array(":clovek_id"=>$clovek_id, ":rocnik"=>$GLOBALS["current_season"]));
	
	cph_h2($lang["debates"], array("id"=>"debaty"));
	cph_p(
		sprintf(
			$lang["%s total debates, listing %s from season %s."],
			$total_debaty, $season_debaty, season_to_str($GLOBALS["current_season"])
		)
		. (($total_debaty > $cur_debaty) ? " " . cphs_link("./?page=clovek.debaty&clovek_id=" . $clovek_id, $lang["show all"]) : "")
	);
}

if ($cur_debaty) {
	// if there are any current debates
	
	de_html($r_debaty);
	cph_table_head(array($lang["date"],$lang["aff"],$lang["neg"],$lang["resolution"],$lang["result"],"",$lang["role"],""));
	foreach ($r_debaty as $debata) {
		switch ($debata["role"]) {
			case "o": // organizer
				$detaily = "";
				$result = result_to_str($debata["vitez"], $debata["presvedcive"]);
			break;
			case "r": // adjudicator
				$detaily = ($debata["adj_rozhodnuti"] ? "A" : "N") . ($debata["adj_presvedcive"] ? "+" : "-");
				$result = result_to_str($debata["vitez"], $debata["presvedcive"]);
			break;
			default: // debater
				$detaily = $debata["kidy"];
				switch (substr($debata["role"], 0, 1)) {
					case "a": // affirmative debater
						$result = result_for_side($debata["vitez"], $debata["presvedcive"], 1);
					break;
					
					case "n": // negative debater
						$result = result_for_side($debata["vitez"], $debata["presvedcive"], 0);
					break;
				}
			break;
		}
		cph_table_row(array(
			format_date($debata["datum"]),
			cphs_link("./?page=tym&tym_id=" . $debata["aff_ID"],$debata["aff_tym"]),
			cphs_link("./?page=tym&tym_id=" . $debata["neg_ID"],$debata["neg_tym"]),
			cphs_link("./?page=teze.detaily&teze_id=" . $debata["teze_ID"],$debata["teze_short"],array("title"=>$debata["teze"])),
			$result,
			cphs_link("./?page=debata&debata_id=" . $debata["debata_ID"], $lang["debate details"]),
			$domain_debata_role[$debata["role"]],
			$detaily
		),
			$debata["achtung"] ? array("class"=>"achtung") : array()
		);
	}
	cph_table_end();
}

// ipoints overview
$have_ipoints = cpdb_fetch_all("
	select
		rocnik,
		role,
		sum(ibody) as ipts
	from (select rocnik, role, ibody from clovek_debata_ibody where clovek_ID = :clovek_id
		union all
		select rocnik, 'bonus' as role, ibody from clovek_ibody where clovek_ID = :clovek_id) ibt
	group by rocnik, role with rollup
	",
	array(":clovek_id"=>$clovek_id, ":rocnik"=>$GLOBALS["current_season"]), $r_sum);

if ($have_ipoints) {
	// there are some ipts globally
	cph_h2($lang["points overview"], array("id"=>"ibody"));
	cph_table_head(array($lang["season"], $lang["role"],$lang["ipoints"]));
	$prev_season = null;
	foreach ($r_sum as $sum) {
		$row_param = ($sum["rocnik"] === $prev_season) ? array() : array("class"=>"hr");
		$prev_season = $sum["rocnik"];
		
		cph_table_row(array(
			is_null($sum["rocnik"]) ? $lang["total"] : season_to_str($sum["rocnik"]),
			is_null($sum["role"])
				? (is_null($sum["rocnik"]) ? "" : $lang["total"])
				: $domain_ibody_role[$sum["role"]],
			$sum["ipts"]
		),
			$row_param
		);
	}
	cph_table_end();
}

if ($GLOBALS["cps_lidi"] >= 2) {
	// can edit all people == can edit bonus ipoints
	
	cph_tag("h3", array(), $lang["bonus ipoints"]);
	
	$bipts = new cpykhen;
	$bipts->form_attr["action"] = "./?page=clovek.ibody.edit.exec";
	$bipts->select = "select * from clovek_ibody where clovek_ID = :clovek_id";
	$bipts->select_args = array(":clovek_id"=>$clovek_id);
	$bipts->form[] = new cpf_hidden("clovek_id",$clovek_id);
	$bipts->form[] = new cpf_db_hidden("clovek_ibody_id", "clovek_ibody_ID");
	$bipts->form[] = new cpf_text($lang["season"] . ": ");
	$bipts->form[] = new cpf_db_listbox_array("rocnik", $GLOBALS["season_domain"]);
	$bipts->form[] = new cpf_nl();
	$bipts->form[] = new cpf_text($lang["ipoints"] . ": ");
	$bipts->form[] = new cpf_db_textbox("ibody","ibody",array("maxlength"=>6));
	$bipts->form[] = new cpf_nl();
	$bipts->form[] = new cpf_text($lang["comment"] . ": ");
	$bipts->form[] = new cpf_db_textbox("tx","tx",array("maxlength"=>255));
	$bipts->form["b1"] = new cpf_button("save",$lang["save"]);
	//$bipts->form["b2"] = new cpf_button("delete",$lang["delete"]);
	$bipts->render_select();
	
	$bipts->form["b1"] = new cpf_button("add",$lang["add"]);
	unset($bipts->form["b2"]);
	$bipts->load_this(array("rocnik"=>$GLOBALS["current_season"]));
	$bipts->render_hidden($lang["add ipoints"]);
	
} elseif (cpdb_fetch_all("select rocnik, ibody, tx from clovek_ibody where clovek_ID = :clovek_id order by rocnik desc", array(":clovek_id"=>$clovek_id), $r_ibody)) {
	
	de_html($r_ibody);
	cph_tag("h3", array(), $lang["bonus ipoints"]);
	cph_table_head(array($lang["season"], $lang["ipts"], $lang["comment"]));

	foreach ($r_ibody as $ib) {
		cph_table_row(array(
			season_to_str($ib["rocnik"]),
			$ib["ibody"],
			$ib["tx"]
		));
	}
	cph_table_end();
}

if (cpdb_fetch_one_value("select exists (select 1 from clovek_debata_ibody where clovek_ID = :clovek_id and rocnik = :rocnik)", array(":clovek_id"=>$clovek_id, ":rocnik"=>$GLOBALS["current_season"]))) {
	// there are debate ipoints
	if(cpdb_fetch_all("
		select
			tym_aff.tym_ID as aff_ID,
			tym_aff.nazev as aff_tym,
			tym_neg.tym_ID as neg_ID,
			tym_neg.nazev as neg_tym,
			teze.tx as teze,
			teze.tx_short as teze_short,
			teze.teze_ID as teze_ID,
			debata.datum,
			debata.vitez,
			debata.presvedcive,
			debata.debata_ID,
			clovek_debata_ibody.role,
			clovek_debata_ibody.ibody
		from
			clovek_debata_ibody
			left join debata on debata.debata_ID = clovek_debata_ibody.debata_ID
			left join debata_tym dt_aff on dt_aff.debata_ID = debata.debata_ID and dt_aff.pozice = 1
			left join debata_tym dt_neg on dt_neg.debata_ID = debata.debata_ID and dt_neg.pozice = 0
			left join tym tym_aff on dt_aff.tym_ID = tym_aff.tym_ID
			left join tym tym_neg on dt_neg.tym_ID = tym_neg.tym_ID
			left join teze on teze.teze_ID = debata.teze_ID
		where
			clovek_debata_ibody.clovek_ID = :clovek_id
			and clovek_debata_ibody.rocnik = :rocnik
		order by
			datum desc
		", array(":clovek_id"=>$clovek_id, ":rocnik"=>$GLOBALS["current_season"]),$r_debaty)) {
		
		de_html($r_debaty);
		cph_tag("h3", array(), $lang["debate ipoints"]);
		cph_table_head(array($lang["date"],$lang["aff"],$lang["neg"],$lang["resolution"],$lang["result"],"",$lang["role"],$lang["ipoints"]));

		foreach ($r_debaty as $debata) {
			if ($debata["vitez"] == 1) { $result = $lang["aff"]; } else { $result = $lang["neg"]; }
			$result .= " ";
			if ($debata["presvedcive"] == 1) { $result .= $lang["3:0"]; } else { $result .= $lang["2:1"]; }
			cph_table_row(array(
				format_date($debata["datum"]),
				cphs_link("./?page=tym&tym_id=" . $debata["aff_ID"],$debata["aff_tym"]),
				cphs_link("./?page=tym&tym_id=" . $debata["neg_ID"],$debata["neg_tym"]),
				cphs_link("./?page=teze.detaily&teze_id=" . $debata["teze_ID"],$debata["teze_short"],array("title"=>$debata["teze"])),
				$result,
				cphs_link("./?page=debata&debata_id=" . $debata["debata_ID"], $lang["debate details"]),
				$domain_ibody_role[$debata["role"]],
				$debata["ibody"]
			));
		}
		cph_table_end();
	} // end of "by debates"
} // end of "ipts"

?>
