-- MySQL dump 8.23
--
-- Host: localhost    Database: greybox
---------------------------------------------------------
-- Server version	3.23.58

--
-- Table structure for table `clovek`
--

CREATE TABLE clovek (
  clovek_ID int(10) unsigned NOT NULL auto_increment,
  jmeno varchar(40) NOT NULL default '',
  nick varchar(255) default NULL,
  prijmeni varchar(60) NOT NULL default '',
  login_name varchar(32) default NULL,
  login_pw varchar(32) default NULL,
  narozen date default NULL,
  klub_ID int(10) unsigned default NULL,
  debater tinyint(1) NOT NULL default '0',
  komentar blob,
  prava_lidi tinyint(3) unsigned NOT NULL default '0',
  prava_kluby tinyint(3) unsigned NOT NULL default '0',
  prava_souteze tinyint(3) unsigned NOT NULL default '0',
  prava_debaty tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (clovek_ID),
  KEY klub_ID (klub_ID),
  KEY prijmeni (prijmeni,jmeno,nick)
) TYPE=MyISAM;

--
-- Table structure for table `clovek_tym`
--

CREATE TABLE clovek_tym (
  ct_ID int(10) unsigned NOT NULL auto_increment,
  clovek_ID int(10) unsigned NOT NULL default '0',
  tym_ID int(10) unsigned NOT NULL default '0',
  aktivni tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (ct_ID),
  KEY clovek_ID (clovek_ID),
  KEY tym_ID (tym_ID)
) TYPE=MyISAM;

--
-- Table structure for table `debata`
--

CREATE TABLE debata (
  debata_ID int(10) unsigned NOT NULL auto_increment,
  soutez_ID int(10) unsigned NOT NULL default '0',
  teze_ID int(10) unsigned NOT NULL default '0',
  datum date NOT NULL default '0000-00-00',
  misto varchar(255) default NULL,
  komentar blob,
  vitez tinyint(1) NOT NULL default '0',
  presvedcive tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (debata_ID),
  KEY soutez_ID (soutez_ID)
) TYPE=MyISAM;

--
-- Table structure for table `debata_clovek`
--

CREATE TABLE debata_clovek (
  dc_ID int(10) unsigned NOT NULL auto_increment,
  debata_ID int(10) unsigned NOT NULL default '0',
  clovek_ID int(10) unsigned NOT NULL default '0',
  pozice enum('r','a1','a2','a3','n1','n2','n3') NOT NULL default 'r',
  kidy tinyint(3) unsigned default NULL,
  rozhodnuti tinyint(1) default NULL,
  presvedcive tinyint(1) default NULL,
  PRIMARY KEY  (dc_ID),
  KEY debata_ID (debata_ID),
  KEY clovek_ID (clovek_ID)
) TYPE=MyISAM;

--
-- Table structure for table `debata_tym`
--

CREATE TABLE debata_tym (
  dt_ID int(10) unsigned NOT NULL auto_increment,
  debata_ID int(10) unsigned NOT NULL default '0',
  tym_ID int(10) unsigned NOT NULL default '0',
  pozice tinyint(1) NOT NULL default '0',
  body tinyint(3) unsigned default NULL,
  PRIMARY KEY  (dt_ID),
  KEY debata_ID (debata_ID),
  KEY tym_ID (tym_ID)
) TYPE=MyISAM;

--
-- Table structure for table `klub`
--

CREATE TABLE klub (
  klub_ID int(10) unsigned NOT NULL auto_increment,
  misto varchar(255) default NULL,
  nazev varchar(255) NOT NULL default '',
  komentar blob,
  PRIMARY KEY  (klub_ID)
) TYPE=MyISAM;

--
-- Table structure for table `kontakt`
--

CREATE TABLE kontakt (
  kontakt_ID int(10) unsigned NOT NULL auto_increment,
  clovek_ID int(10) unsigned NOT NULL default '0',
  druh enum('telefon','email','adresa','icq','jabber','web') NOT NULL default 'telefon',
  tx varchar(255) NOT NULL default '',
  PRIMARY KEY  (kontakt_ID),
  KEY clovek_ID (clovek_ID)
) TYPE=MyISAM;

--
-- Table structure for table `rozhodci`
--

CREATE TABLE rozhodci (
  rozhodci_ID int(10) unsigned NOT NULL auto_increment,
  clovek_ID int(10) unsigned NOT NULL default '0',
  misto varchar(255) default NULL,
  jazyk set('cz','sk','en','de','fr') default NULL,
  rocnik tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (rozhodci_ID),
  KEY clovek_ID (clovek_ID)
) TYPE=MyISAM;

--
-- Table structure for table `soutez`
--

CREATE TABLE soutez (
  soutez_ID int(10) unsigned NOT NULL auto_increment,
  rocnik tinyint(3) unsigned NOT NULL default '0',
  jazyk enum('cz','sk','en','de','fr') default NULL,
  nazev varchar(255) NOT NULL default '',
  komentar blob,
  zamceno tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (soutez_ID)
) TYPE=MyISAM;

--
-- Table structure for table `soutez_teze`
--

CREATE TABLE soutez_teze (
  st_ID int(10) unsigned NOT NULL auto_increment,
  soutez_ID int(10) unsigned NOT NULL default '0',
  teze_ID int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (st_ID),
  KEY soutez_ID (soutez_ID),
  KEY teze_ID (teze_ID)
) TYPE=MyISAM;

--
-- Table structure for table `teze`
--

CREATE TABLE teze (
  teze_ID int(10) unsigned NOT NULL auto_increment,
  tx varchar(255) NOT NULL default '',
  komentar blob,
  PRIMARY KEY  (teze_ID)
) TYPE=MyISAM;

--
-- Table structure for table `tym`
--

CREATE TABLE tym (
  tym_ID int(10) unsigned NOT NULL auto_increment,
  klub_ID int(10) unsigned NOT NULL default '0',
  nazev varchar(255) NOT NULL default '',
  komentar blob,
  PRIMARY KEY  (tym_ID),
  KEY klub_ID (klub_ID)
) TYPE=MyISAM;
