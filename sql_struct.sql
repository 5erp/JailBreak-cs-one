-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Хост: localhost:3306
-- Время создания: Сен 17 2020 г., 14:47
-- Версия сервера: 5.7.30-0ubuntu0.18.04.1-log
-- Версия PHP: 7.2.24-0ubuntu0.18.04.6

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `admin_jbserver`
--

-- --------------------------------------------------------

--
-- Структура таблицы `amx_admins_servers`
--

CREATE TABLE `amx_admins_servers` (
  `admin_id` int(11) DEFAULT NULL,
  `server_id` int(11) DEFAULT NULL,
  `custom_flags` varchar(32) NOT NULL,
  `use_static_bantime` enum('yes','no') NOT NULL DEFAULT 'yes'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Структура таблицы `amx_amxadmins`
--

CREATE TABLE `amx_amxadmins` (
  `id` int(12) NOT NULL,
  `username` varchar(32) DEFAULT NULL,
  `password` varchar(32) DEFAULT NULL,
  `access` varchar(128) DEFAULT NULL,
  `flags` varchar(32) DEFAULT NULL,
  `steamid` varchar(32) DEFAULT NULL,
  `nickname` varchar(32) DEFAULT NULL,
  `ashow` int(11) DEFAULT NULL,
  `created` int(11) DEFAULT NULL,
  `expired` int(11) DEFAULT NULL,
  `days` int(11) DEFAULT NULL,
  `email` varchar(120) DEFAULT NULL,
  `gamecms` text NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Структура таблицы `amx_bans`
--

CREATE TABLE `amx_bans` (
  `bid` int(11) NOT NULL,
  `player_ip` varchar(32) DEFAULT NULL,
  `player_id` varchar(35) DEFAULT NULL,
  `player_nick` varchar(100) DEFAULT NULL,
  `admin_ip` varchar(32) DEFAULT NULL,
  `admin_id` varchar(35) DEFAULT NULL,
  `admin_nick` varchar(100) DEFAULT NULL,
  `ban_type` varchar(10) DEFAULT 'S',
  `ban_reason` varchar(100) DEFAULT NULL,
  `cs_ban_reason` varchar(100) DEFAULT NULL,
  `ban_created` int(11) DEFAULT NULL,
  `ban_length` int(11) DEFAULT NULL,
  `server_ip` varchar(32) DEFAULT NULL,
  `server_name` varchar(100) DEFAULT NULL,
  `cookie` text,
  `ban_kicks` int(11) NOT NULL DEFAULT '0',
  `expired` int(1) NOT NULL DEFAULT '0',
  `unban_type` int(1) DEFAULT NULL,
  `ban_closed` int(7) DEFAULT NULL,
  `imported` int(1) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Структура таблицы `amx_serverinfo`
--

CREATE TABLE `amx_serverinfo` (
  `id` int(11) NOT NULL,
  `timestamp` int(11) DEFAULT NULL,
  `hostname` varchar(100) DEFAULT 'Unknown',
  `address` varchar(100) DEFAULT NULL,
  `gametype` varchar(32) DEFAULT NULL,
  `rcon` varchar(32) DEFAULT NULL,
  `amxban_version` varchar(32) DEFAULT NULL,
  `amxban_motd` varchar(250) DEFAULT NULL,
  `motd_delay` int(10) DEFAULT '10',
  `amxban_menu` int(10) NOT NULL DEFAULT '1',
  `reasons` int(10) DEFAULT NULL,
  `timezone_fixx` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Дамп данных таблицы `amx_serverinfo`
--

INSERT INTO `amx_serverinfo` (`id`, `timestamp`, `hostname`, `address`, `gametype`, `rcon`, `amxban_version`, `amxban_motd`, `motd_delay`, `amxban_menu`, `reasons`, `timezone_fixx`) VALUES
(1, 1477562508, 'Маленькая тюрьма', '37.230.210.119:27015', 'cstrike', '', '6.13', '', 10, 1, 0, 0);

-- --------------------------------------------------------

--
-- Структура таблицы `jbe_grblocks`
--

CREATE TABLE `jbe_grblocks` (
  `id` int(11) NOT NULL,
  `auth` tinytext CHARACTER SET utf8 NOT NULL,
  `ip` tinytext CHARACTER SET utf8 NOT NULL,
  `name` tinytext CHARACTER SET utf8 NOT NULL,
  `time_start` int(10) UNSIGNED NOT NULL,
  `time_end` int(10) UNSIGNED NOT NULL,
  `length` int(11) NOT NULL,
  `reason` varchar(255) CHARACTER SET utf8 NOT NULL,
  `admin_auth` tinytext CHARACTER SET utf8 NOT NULL,
  `admin_ip` tinytext CHARACTER SET utf8 NOT NULL,
  `admin_name` tinytext CHARACTER SET utf8 NOT NULL,
  `remove_type` int(11) NOT NULL DEFAULT '0',
  `ban_count` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Структура таблицы `jbe_quests`
--

CREATE TABLE `jbe_quests` (
  `save_id` int(11) NOT NULL,
  `i1` int(3) DEFAULT NULL,
  `s1` int(3) DEFAULT NULL,
  `n1` int(11) DEFAULT NULL,
  `e1` int(11) DEFAULT NULL,
  `i2` int(3) DEFAULT NULL,
  `s2` int(3) DEFAULT NULL,
  `n2` int(11) DEFAULT NULL,
  `e2` int(11) DEFAULT NULL,
  `i3` int(3) DEFAULT NULL,
  `s3` int(3) DEFAULT NULL,
  `n3` int(11) DEFAULT NULL,
  `e3` int(11) DEFAULT NULL,
  `i4` int(3) DEFAULT NULL,
  `s4` int(3) DEFAULT NULL,
  `n4` int(11) DEFAULT NULL,
  `e4` int(11) DEFAULT NULL,
  `i5` int(3) DEFAULT NULL,
  `s5` int(3) DEFAULT NULL,
  `n5` int(11) DEFAULT NULL,
  `e5` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Структура таблицы `jbe_save`
--

CREATE TABLE `jbe_save` (
  `id` int(11) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `auth` varchar(32) DEFAULT NULL,
  `ip` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `pass` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `exp` int(11) DEFAULT '0',
  `money` int(11) NOT NULL DEFAULT '0',
  `gametime` int(11) NOT NULL DEFAULT '0',
  `br_type` int(3) DEFAULT '0',
  `br_time` int(11) DEFAULT '0',
  `setting` varchar(128) DEFAULT NULL,
  `limits` varchar(64) DEFAULT NULL,
  `timestamp` int(11) NOT NULL DEFAULT '0',
  `month_save` int(3) DEFAULT '0',
  `month_time` int(11) DEFAULT '0',
  `month_last` int(11) DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;


--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `amx_amxadmins`
--
ALTER TABLE `amx_amxadmins`
  ADD PRIMARY KEY (`id`),
  ADD KEY `steamid` (`steamid`);

--
-- Индексы таблицы `amx_bans`
--
ALTER TABLE `amx_bans`
  ADD PRIMARY KEY (`bid`),
  ADD KEY `player_id` (`player_id`);

--
-- Индексы таблицы `amx_serverinfo`
--
ALTER TABLE `amx_serverinfo`
  ADD PRIMARY KEY (`id`);

--
-- Индексы таблицы `jbe_grblocks`
--
ALTER TABLE `jbe_grblocks`
  ADD PRIMARY KEY (`id`);

--
-- Индексы таблицы `jbe_quests`
--
ALTER TABLE `jbe_quests`
  ADD UNIQUE KEY `save_id` (`save_id`);

--
-- Индексы таблицы `jbe_save`
--
ALTER TABLE `jbe_save`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `amx_amxadmins`
--
ALTER TABLE `amx_amxadmins`
  MODIFY `id` int(12) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
--
-- AUTO_INCREMENT для таблицы `amx_bans`
--
ALTER TABLE `amx_bans`
  MODIFY `bid` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
--
-- AUTO_INCREMENT для таблицы `amx_serverinfo`
--
ALTER TABLE `amx_serverinfo`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
--
-- AUTO_INCREMENT для таблицы `jbe_grblocks`
--
ALTER TABLE `jbe_grblocks`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
--
-- AUTO_INCREMENT для таблицы `jbe_save`
--
ALTER TABLE `jbe_save`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;