/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50716
Source Host           : localhost:3306
Source Database       : p2chatserver

Target Server Type    : MYSQL
Target Server Version : 50716
File Encoding         : 65001

Date: 2017-07-10 15:27:34
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for encrypt_repo
-- ----------------------------
DROP TABLE IF EXISTS `encrypt_repo`;
CREATE TABLE `encrypt_repo` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `algorithm_name` varchar(30) NOT NULL COMMENT '算法名',
  `is_key_pair` tinyint(1) unsigned zerofill NOT NULL COMMENT '是否含有双钥',
  `public_key` blob COMMENT '公钥(非对称加密)或秘钥(对称加密)',
  `private_key` blob COMMENT '私钥',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for group_info
-- ----------------------------
DROP TABLE IF EXISTS `group_info`;
CREATE TABLE `group_info` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `name` varchar(30) NOT NULL COMMENT '群组名',
  `icon` varchar(256) NOT NULL COMMENT '群组图标',
  `profile` varchar(512) DEFAULT NULL COMMENT '群组简介',
  `owner_user_info_id` int(4) unsigned NOT NULL COMMENT '群主id',
  `assistant_user_info_id_list` varchar(1024) DEFAULT NULL COMMENT '群管id列表',
  `file_zone_list` varchar(4096) DEFAULT NULL COMMENT '文件列表',
  `setting_xml` varchar(4096) DEFAULT NULL COMMENT '配置信息xml',
  `created_date` datetime NOT NULL COMMENT '群组创建时间',
  `encrypt_repo_id` int(4) unsigned NOT NULL COMMENT '加密库id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for group_member_data
-- ----------------------------
DROP TABLE IF EXISTS `group_member_data`;
CREATE TABLE `group_member_data` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `group_info_id` int(4) unsigned NOT NULL COMMENT '群组信息id',
  `user_info_id` int(4) unsigned NOT NULL COMMENT '用户信息id',
  `group_member_permission_id_list` varchar(1024) DEFAULT NULL COMMENT '群组成员权限id列表',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for group_member_permission
-- ----------------------------
DROP TABLE IF EXISTS `group_member_permission`;
CREATE TABLE `group_member_permission` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `name` varchar(30) NOT NULL COMMENT '群组成员权限名',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for group_msg_data
-- ----------------------------
DROP TABLE IF EXISTS `group_msg_data`;
CREATE TABLE `group_msg_data` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `group_info_id` int(4) NOT NULL COMMENT '群组信息id',
  `post_date` datetime NOT NULL COMMENT '发送时间',
  `user_info_id` int(4) unsigned NOT NULL COMMENT '用户信息id',
  `msg_data` blob COMMENT '消息数据',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for log_data
-- ----------------------------
DROP TABLE IF EXISTS `log_data`;
CREATE TABLE `log_data` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `log_type_id` int(4) unsigned NOT NULL COMMENT '日志类型id',
  `record_date` datetime NOT NULL COMMENT '记录时间',
  `log_data` varchar(4096) DEFAULT NULL COMMENT '日志数据',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for log_type
-- ----------------------------
DROP TABLE IF EXISTS `log_type`;
CREATE TABLE `log_type` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `name` varchar(30) NOT NULL COMMENT '日志类型名',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for user_info
-- ----------------------------
DROP TABLE IF EXISTS `user_info`;
CREATE TABLE `user_info` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `username` varchar(30) NOT NULL COMMENT '用户名',
  `password` varchar(128) NOT NULL COMMENT '密码',
  `nickname` varchar(30) NOT NULL COMMENT '昵称',
  `age` smallint(2) unsigned zerofill DEFAULT NULL COMMENT '年龄',
  `sex` char(1) DEFAULT NULL COMMENT '性别(''f'',''m'')',
  `icon` varchar(256) NOT NULL COMMENT '头像',
  `profile` varchar(512) DEFAULT NULL COMMENT '个人简介',
  `qq` varchar(32) NOT NULL COMMENT 'QQ',
  `email` varchar(64) NOT NULL COMMENT '邮箱',
  `last_login` datetime NOT NULL COMMENT '最近一次登录时间',
  `date_joined` datetime NOT NULL COMMENT '创建时间',
  `is_locked` tinyint(1) unsigned zerofill DEFAULT NULL COMMENT '是否已锁',
  `try_times` tinyint(1) unsigned zerofill DEFAULT NULL COMMENT '尝试登录次数',
  `user_status_id` int(4) unsigned NOT NULL COMMENT '用户状态id',
  `encryt_repo_id` int(4) unsigned NOT NULL COMMENT '加密库id',
  `is_deleted` tinyint(1) unsigned zerofill NOT NULL COMMENT '是否已删除',
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`),
  UNIQUE KEY `qq` (`qq`),
  UNIQUE KEY `email` (`email`),
  UNIQUE KEY `encryt_repo_id` (`encryt_repo_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for user_msg_data
-- ----------------------------
DROP TABLE IF EXISTS `user_msg_data`;
CREATE TABLE `user_msg_data` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `user_info_id` int(4) unsigned NOT NULL COMMENT '用户信息id',
  `post_date` datetime NOT NULL COMMENT '发送时间',
  `msg_data` blob COMMENT '消息数据',
  `is_lost` tinyint(1) unsigned zerofill NOT NULL COMMENT '是否未收到',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for user_permission
-- ----------------------------
DROP TABLE IF EXISTS `user_permission`;
CREATE TABLE `user_permission` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `name` varchar(30) NOT NULL COMMENT '权限名',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Table structure for user_status
-- ----------------------------
DROP TABLE IF EXISTS `user_status`;
CREATE TABLE `user_status` (
  `id` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id号',
  `name` varchar(30) NOT NULL COMMENT '状态名',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
