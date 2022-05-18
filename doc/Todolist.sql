-- phpMyAdmin SQL Dump
-- version 4.9.5deb2
-- https://www.phpmyadmin.net/
--
-- 主机： localhost:3306
-- 生成日期： 2022-05-09 09:33:37
-- 服务器版本： 8.0.22-0ubuntu0.20.04.3
-- PHP 版本： 7.4.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- 数据库： `TodoList`
--

-- --------------------------------------------------------

--
-- 表的结构 `_tb_authLog`
--

CREATE TABLE `_tb_authLog` (
  `id` int NOT NULL COMMENT '编号',
  `remote_host` varchar(128) DEFAULT NULL COMMENT '远程机器',
  `module_tag` varchar(128) DEFAULT NULL COMMENT '模块标签',
  `log_info` varchar(1024) DEFAULT NULL COMMENT '日志信息',
  `save_timestamp` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '保存时间戳'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='数据接收日志表';

-- --------------------------------------------------------

--
-- 表的结构 `_tb_taskUser`
--

CREATE TABLE `_tb_taskUser` (
  `id` int NOT NULL COMMENT '自动编号',
  `userName` varchar(256) DEFAULT NULL COMMENT '用户名',
  `nickName` varchar(128) DEFAULT NULL COMMENT '昵称',
  `accKey` varchar(128) DEFAULT NULL COMMENT '访问秘钥',
  `device_id` varchar(128) DEFAULT NULL COMMENT '设备编号',
  `lastVersion` int DEFAULT '1' COMMENT '任务版本',
  `imgUrl` varchar(512) DEFAULT NULL COMMENT '用户头像地址',
  `enabled` int DEFAULT '1' COMMENT '是否有效',
  `createTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户信息';

--
-- 转存表中的数据 `_tb_taskUser`
--

INSERT INTO `_tb_taskUser` (`id`, `userName`, `nickName`, `accKey`, `device_id`, `lastVersion`, `imgUrl`, `enabled`, `createTime`) VALUES
(4, 'test', 'test', 'test', NULL, 1, NULL, 1, '2021-11-22 08:51:26'),
(6, 'admin', 'admin', 'admin', 'device_id', 1, NULL, 1, '2022-05-09 01:31:04');

-- --------------------------------------------------------

--
-- 表的结构 `_tb_todoTask`
--

CREATE TABLE `_tb_todoTask` (
  `id` int NOT NULL COMMENT '自动编号',
  `uid` int DEFAULT NULL COMMENT '用户编号',
  `taskInfo` varchar(1024) DEFAULT NULL COMMENT '工作任务',
  `taskTag` varchar(512) DEFAULT NULL COMMENT '任务标签',
  `category` varchar(512) DEFAULT NULL COMMENT '工作分类',
  `priority` int DEFAULT '0' COMMENT '优先级',
  `taskRemark` varchar(512) DEFAULT NULL COMMENT '备注',
  `position` varchar(128) DEFAULT NULL COMMENT '地点',
  `beginTime` timestamp NULL DEFAULT NULL COMMENT '开始时间',
  `endTime` timestamp NULL DEFAULT NULL COMMENT '结束时间',
  `taskState` int DEFAULT '0' COMMENT '任务状态',
  `isDeleted` int NOT NULL DEFAULT '0' COMMENT '是否删除',
  `saveTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '保存时间',
  `task_guid` varchar(64) DEFAULT NULL COMMENT '任务标签'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='工作任务清单';

--
-- 转存表中的数据 `_tb_todoTask`
--

INSERT INTO `_tb_todoTask` (`id`, `uid`, `taskInfo`, `taskTag`, `category`, `priority`, `taskRemark`, `position`, `beginTime`, `endTime`, `taskState`, `isDeleted`, `saveTime`, `task_guid`) VALUES
(138, 1, '查看任务文字显示使用view示图显示。', '', '', 4, '墨水屏', 'ShangHai', '2021-11-15 14:00:00', '2021-11-15 15:00:00', 0, 0, '2021-11-15 14:50:50', 'hello'),
(164, 1, '实现工作任务删除功能。p', '墨水屏', '电子日历', 1, '墨水屏', 'ShangHai', '2021-11-18 06:00:00', '2021-11-18 07:00:00', 0, 1, '2021-11-18 06:29:12', 'hello');

-- --------------------------------------------------------

--
-- 表的结构 `_tb_weatherInfo`
--

CREATE TABLE `_tb_weatherInfo` (
  `id` int NOT NULL,
  `city_id` varchar(128) DEFAULT NULL COMMENT '城市名称',
  `weather_type` varchar(64) DEFAULT NULL COMMENT '天气类型',
  `weather_info` varchar(2048) DEFAULT NULL COMMENT '天气信息',
  `delete_flag` int DEFAULT '0' COMMENT '删除标志',
  `createTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='天气气象信息';

--
-- 转储表的索引
--

--
-- 表的索引 `_tb_authLog`
--
ALTER TABLE `_tb_authLog`
  ADD PRIMARY KEY (`id`);

--
-- 表的索引 `_tb_taskUser`
--
ALTER TABLE `_tb_taskUser`
  ADD PRIMARY KEY (`id`);

--
-- 表的索引 `_tb_todoTask`
--
ALTER TABLE `_tb_todoTask`
  ADD PRIMARY KEY (`id`),
  ADD KEY `task_guid` (`task_guid`);

--
-- 表的索引 `_tb_weatherInfo`
--
ALTER TABLE `_tb_weatherInfo`
  ADD PRIMARY KEY (`id`),
  ADD KEY `city_id` (`city_id`),
  ADD KEY `weather_type` (`weather_type`);

--
-- 在导出的表使用AUTO_INCREMENT
--

--
-- 使用表AUTO_INCREMENT `_tb_authLog`
--
ALTER TABLE `_tb_authLog`
  MODIFY `id` int NOT NULL AUTO_INCREMENT COMMENT '编号';

--
-- 使用表AUTO_INCREMENT `_tb_taskUser`
--
ALTER TABLE `_tb_taskUser`
  MODIFY `id` int NOT NULL AUTO_INCREMENT COMMENT '自动编号', AUTO_INCREMENT=7;

--
-- 使用表AUTO_INCREMENT `_tb_todoTask`
--
ALTER TABLE `_tb_todoTask`
  MODIFY `id` int NOT NULL AUTO_INCREMENT COMMENT '自动编号', AUTO_INCREMENT=745;

--
-- 使用表AUTO_INCREMENT `_tb_weatherInfo`
--
ALTER TABLE `_tb_weatherInfo`
  MODIFY `id` int NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
