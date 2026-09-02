-- MySQL dump 8.23
--
-- Host: localhost    Database: greybox
---------------------------------------------------------
-- Server version	3.23.58

--
-- Table structure for table `klub`
--

CREATE TABLE klub (
  klub_ID int(10) unsigned NOT NULL auto_increment,
  misto varchar(255) default NULL,
  nazev varchar(255) NOT NULL default '',
  kratky_nazev varchar(32) NOT NULL default '',
  komentar text,
  PRIMARY KEY  (klub_ID)
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `tym`
--

CREATE TABLE tym (
  tym_ID int(10) unsigned NOT NULL auto_increment,
  klub_ID int(10) unsigned NOT NULL default '0',
  nazev varchar(255) NOT NULL default '',
  komentar text,
  PRIMARY KEY  (tym_ID),
  KEY klub_ID (klub_ID),
  constraint fk_tym_klub foreign key (klub_ID) references klub (klub_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;


--
-- Table structure for table `clovek`
--

CREATE TABLE clovek (
  clovek_ID int(10) unsigned NOT NULL auto_increment,
  jmeno varchar(40) NOT NULL default '',
  nick varchar(60) default NULL,
  prijmeni varchar(60) NOT NULL default '',
  narozen date default NULL,
  klub_ID int(10) unsigned default NULL,
  debater tinyint(1) NOT NULL default '0',
  clen tinyint(1) NOT NULL default '0',
  komentar text,
  prava_lidi tinyint(3) unsigned NOT NULL default '0',
  prava_kluby tinyint(3) unsigned NOT NULL default '0',
  prava_souteze tinyint(3) unsigned NOT NULL default '0',
  prava_debaty tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (clovek_ID),
  KEY klub_ID (klub_ID),
  KEY prijmeni (prijmeni,jmeno,nick),
  constraint fk_clovek_klub foreign key (klub_ID) references klub (klub_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `soutez`
--

CREATE TABLE soutez (
  soutez_ID int(10) unsigned NOT NULL auto_increment,
  rocnik tinyint(3) unsigned NOT NULL default '0',
  jazyk enum('cz','en','de','fr') character set utf8 NOT NULL default 'cz',
  nazev varchar(255) NOT NULL default '',
  komentar text,
  zamceno tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (soutez_ID)
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `turnaj`
--

CREATE TABLE `liga` (
  liga_ID int(10) unsigned NOT NULL auto_increment,
  rocnik tinyint(3) unsigned NOT NULL default '0',
  nazev varchar(255) NOT NULL default '',
  komentar text,
  PRIMARY KEY  (liga_ID)
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `turnaj`
--

CREATE TABLE turnaj (
  turnaj_ID int(10) unsigned NOT NULL auto_increment,
  soutez_ID int(10) unsigned NOT NULL default '0',
  liga_ID int(10) unsigned default NULL,
  nazev varchar(255) NOT NULL default '',
  datum_od date NOT NULL default '0000-00-00',
  datum_do date NOT NULL default '0000-00-00',
  komentar text,
  PRIMARY KEY  (turnaj_ID),
  KEY soutez_ID (soutez_ID),
  KEY liga_ID (liga_ID),
  KEY datum_od (datum_od),
  CONSTRAINT fk_turnaj_liga FOREIGN KEY (liga_ID) REFERENCES liga (liga_ID) on delete restrict,
  constraint fk_turnaj_soutez foreign key (soutez_ID) references soutez (soutez_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `debata`
--

CREATE TABLE debata (
  debata_ID int(10) unsigned NOT NULL auto_increment,
  soutez_ID int(10) unsigned NOT NULL default '0',
  turnaj_ID int(10) unsigned default NULL,
  teze_ID int(10) unsigned NOT NULL default '0',
  datum datetime NOT NULL default '0000-00-00',
  misto varchar(255) default NULL,
  komentar text,
  achtung tinyint(1) NOT NULL default '0',
  vitez tinyint(1) NOT NULL default '0', -- 0=neg / 1=aff / 2=draw
  presvedcive tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (debata_ID),
  KEY soutez_ID (soutez_ID),
  KEY turnaj_ID (turnaj_ID),
  KEY teze_ID (teze_ID),
  constraint fk_debata_soutez foreign key (soutez_ID) references soutez (soutez_ID) on delete restrict,
  constraint fk_debata_turnaj foreign key (turnaj_ID) references turnaj (turnaj_ID) on delete restrict,
  constraint fk_debata_teze foreign key (teze_ID) references teze (teze_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;


--
-- Table structure for table `clovek_debata`
--

CREATE TABLE clovek_debata (
  debata_ID int(10) unsigned NOT NULL default '0',
  clovek_ID int(10) unsigned NOT NULL default '0',
  role enum('r','o','a1','a2','a3','n1','n2','n3') character set utf8 NOT NULL default 'r',
  kidy tinyint(3) unsigned default NULL,
  rozhodnuti tinyint(1) default NULL,
  presvedcive tinyint(1) default NULL,
  PRIMARY KEY  (debata_ID, clovek_ID, role),
  KEY debata_ID (debata_ID),
  KEY clovek_ID (clovek_ID),
  constraint fk_cd_debata foreign key (debata_ID) references debata (debata_ID) on delete cascade,
  constraint fk_cd_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `clovek_debata_ibody`
--

CREATE TABLE clovek_debata_ibody (
  clovek_ID int(10) unsigned NOT NULL default '0',
  debata_ID int(10) unsigned NOT NULL default '0',
  rocnik tinyint(3) unsigned NOT NULL default '0',
  role enum('debater','rozhodci','trener','organizator') character set utf8 NOT NULL default 'debater',
  ibody decimal(5,3) NOT NULL default '0.000',
  PRIMARY KEY  (clovek_ID, debata_ID, role),
  KEY clovek_ID (clovek_ID),
  KEY debata_ID (debata_ID),
  KEY role (role),
  constraint fk_cdi_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete cascade,
  constraint fk_cdi_debata foreign key (debata_ID) references debata (debata_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `clovek_klub`
--

CREATE TABLE clovek_klub (
  clovek_ID int(10) unsigned NOT NULL default '0',
  klub_ID int(10) unsigned NOT NULL default '0',
  rocnik tinyint(3) unsigned NOT NULL default '0',
  role enum('t') character set utf8 NOT NULL default 't',
  PRIMARY KEY  (clovek_ID, klub_ID, rocnik),
  KEY clovek_ID (clovek_ID),
  KEY klub_ID (klub_ID),
  constraint fk_ck_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete restrict,
  constraint fk_ck_klub foreign key (klub_ID) references klub (klub_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `clovek_turnaj`
--

CREATE TABLE clovek_turnaj (
  clovek_ID int(10) unsigned NOT NULL default '0',
  turnaj_ID int(10) unsigned NOT NULL default '0',
  role enum('o') character set utf8 NOT NULL default 'o',
  mocnost tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (clovek_ID, turnaj_ID),
  KEY clovek_ID (clovek_ID),
  KEY turnaj_ID (turnaj_ID),
  constraint fk_ctu_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete restrict,
  constraint fk_ctu_turnaj foreign key (turnaj_ID) references turnaj (turnaj_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `clovek_tym`
--

CREATE TABLE clovek_tym (
  clovek_ID int(10) unsigned NOT NULL default '0',
  tym_ID int(10) unsigned NOT NULL default '0',
  aktivni tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (clovek_ID, tym_ID),
  KEY clovek_ID (clovek_ID),
  KEY tym_ID (tym_ID),
  constraint fk_cty_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete cascade,
  constraint fk_cty_tym foreign key (tym_ID) references tym (tym_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;


--
-- Table structure for table `debata_tym`
--

CREATE TABLE debata_tym (
  debata_ID int(10) unsigned NOT NULL default '0',
  tym_ID int(10) unsigned NOT NULL default '0',
  pozice tinyint(1) NOT NULL default '0',
  body tinyint(3) unsigned default NULL,
  liga_vytezek decimal(5,3) default NULL,
  PRIMARY KEY  (debata_ID, tym_ID),
  KEY debata_ID (debata_ID),
  KEY tym_ID (tym_ID),
  constraint fk_dt_debata foreign key (debata_ID) references debata (debata_ID) on delete cascade,
  constraint fk_dt_tym foreign key (tym_ID) references tym (tym_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `liga_tym`
--

CREATE TABLE liga_tym (
  liga_ID int(10) unsigned NOT NULL default '0',
  tym_ID int(10) unsigned NOT NULL default '0',
  liga_vytezek decimal(5,3) NOT NULL default '0.000',
  skrtnute_debaty varchar(255) default NULL,
  PRIMARY KEY  (liga_ID,tym_ID),
  KEY tym_ID (tym_ID),
  CONSTRAINT fk_lt_liga FOREIGN KEY (liga_ID) REFERENCES liga (liga_ID) ON DELETE CASCADE,
  CONSTRAINT fk_lt_tym FOREIGN KEY (tym_ID) REFERENCES tym (tym_ID) ON DELETE CASCADE
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `kontakt`
--

CREATE TABLE kontakt (
  kontakt_ID int(10) unsigned NOT NULL auto_increment,
  clovek_ID int(10) unsigned NOT NULL default '0',
  druh enum('telefon','email','adresa','icq','jabber','web') character set utf8 NOT NULL default 'telefon',
  tx varchar(255) NOT NULL default '',
  viditelnost tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (kontakt_ID),
  KEY clovek_ID (clovek_ID),
  constraint fk_kontakt_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `login`
--

CREATE TABLE login (
  clovek_ID int(10) unsigned NOT NULL default '0',
  username varchar(32) NOT NULL default '',
  password varchar(35) NOT NULL default '',
  PRIMARY KEY  (clovek_ID),
  UNIQUE username (username),
  constraint fk_login_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `rozhodci`
--

CREATE TABLE rozhodci (
  clovek_ID int(10) unsigned NOT NULL default '0',
  rocnik tinyint(3) unsigned NOT NULL default '0',
  uroven tinyint(1) unsigned NOT NULL default '0',
  misto varchar(255) default NULL,
  jazyk set('cz','en','de','fr') character set utf8 default NULL,
  format set('DL','DP','SD','SD2-2') character set utf8 default NULL,
  PRIMARY KEY  (clovek_ID, rocnik),
  KEY clovek_ID (clovek_ID),
  constraint fk_rozhodci_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `teze`
--

CREATE TABLE teze (
  teze_ID int(10) unsigned NOT NULL auto_increment,
  jazyk enum('cz','en','de','fr') character set utf8 NOT NULL default 'cz',
  tx varchar(255) NOT NULL default '',
  tx_short varchar(40) NOT NULL default '',
  komentar text,
  PRIMARY KEY  (teze_ID),
  KEY jazyk (jazyk)
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `soutez_teze`
--

CREATE TABLE soutez_teze (
  soutez_ID int(10) unsigned NOT NULL default '0',
  teze_ID int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (soutez_ID, teze_ID),
  KEY soutez_ID (soutez_ID),
  KEY teze_ID (teze_ID),
  constraint fk_st_soutez foreign key (soutez_ID) references soutez (soutez_ID) on delete cascade,
  constraint fk_st_teze foreign key (teze_ID) references teze (teze_ID) on delete restrict
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Table structure for table `clovek_ibody`
--
CREATE TABLE clovek_ibody (
  clovek_ibody_ID int(10) unsigned NOT NULL auto_increment,
  clovek_ID int(10) unsigned NOT NULL default '0',
  rocnik tinyint(3) unsigned NOT NULL default '0',
  ibody decimal(5,3) NOT NULL default '0.000',
  tx varchar(255),
  PRIMARY KEY (clovek_ibody_ID),
  KEY clovek_ID (clovek_ID),
  constraint fk_ibody_clovek foreign key (clovek_ID) references clovek (clovek_ID) on delete cascade
) TYPE=InnoDB CHARSET=utf8 COLLATE=utf8_czech_ci;

ALTER TABLE `clovek`
ADD `prihlaska` tinyint(1) NOT NULL DEFAULT '0' AFTER `clen_do`,
ADD `prohlaseni` tinyint(1) NOT NULL DEFAULT '0' AFTER `prihlaska`;

ALTER TABLE `klub`
ADD `prihlaska` tinyint(1) NOT NULL DEFAULT '0' AFTER `vedouci`;

ALTER TABLE `clovek_turnaj`
CHANGE `role` `role` enum('o','r') COLLATE 'utf8_general_ci' NOT NULL DEFAULT 'o' AFTER `turnaj_ID`,
CHANGE `mocnost` `mocnost` tinyint(3) unsigned NULL DEFAULT '1' AFTER `role`,
ADD `prihlasil` int(10) unsigned NULL,
ADD FOREIGN KEY (`prihlasil`) REFERENCES `clovek` (`clovek_ID`) ON DELETE RESTRICT,
ADD `komentar` text COLLATE 'utf8_general_ci' NULL;

ALTER TABLE `turnaj`
ADD `deadline` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' AFTER `datum_do`;
